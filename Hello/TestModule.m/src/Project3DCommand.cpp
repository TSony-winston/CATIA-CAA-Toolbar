#include "Project3DCommand.h"
#include "Create3DCommand.h"
#include "CATApplicationFrame.h"
#include "CATCreateExternalObject.h"
#include "CATDlgNotify.h"
#include "CATFrmEditor.h"
#include "CATDocument.h"
#include "CATInit.h"
#include "CATIDftDocumentServices.h"
#include "CATIDrawing.h"
#include "CATISheet.h"
#include "CATIDrwFactory.h"
#include "CATIContainer.h"
#include "CATISpecObject.h"
#include "CATIView.h"
#include "CATILinkableObject.h"
#include "CATIGenerSpec.h"
#include "CATIDftViewMakeUp.h"
#include "CATIAlias.h"
#include "CATMathPlane.h"
#include "CATMathPoint.h"
#include "CATMathVector.h"
#include "CATIRedrawEvent.h"
#include "CATIDftView.h"
#include "CATIDftGenView.h"
#include "CATIDftGenGeomAccess.h"
#include "CATIDftGenGeom.h"
#include "CATIUnknownList.h"
#include "CATIModelEvents.h"
#include "CATCreate.h"
#include "CATIADrawingView.h"
#include "CATIAGenerativeViewBehavior.h"
#include "CATIADocument.h"

CATImplementClass(Project3DCommand, Implementation, CATCommand, CATNull);
CATCreateClass(Project3DCommand);

namespace {

void ShowProject3DMessage(const char* iMessage) {
    CATApplicationFrame* pApplication = CATApplicationFrame::GetFrame();
    CATDlgWindow*        pWindow      = pApplication == NULL ? NULL : pApplication->GetMainWindow();
    if (pWindow == NULL) {
        return;
    }

    CATDlgNotify* pNotify = new CATDlgNotify(pWindow, "Project3DMessage",
                                             CATDlgNfyInformation | CATDlgNfyOK | CATDlgWndModal);
    if (pNotify != NULL) {
        pNotify->DisplayBlocked(iMessage, "CATIA CAA Test");
        pNotify->RequestDelayedDestruction();
    }
}

HRESULT GenerateView(CATIView* iView, CATIDftViewMakeUp* iMakeUp,
                     CATILinkableObject* iPart, const CATMathVector& iHorizontal,
                     const CATMathVector& iVertical) {
    CATIADrawingView* pAutomationView = NULL;
    CATIAGenerativeViewBehavior* pBehavior = NULL;
    CATIADocument* pSourceDocument = NULL;
    HRESULT hr = iView->QueryInterface(IID_CATIADrawingView, (void**)&pAutomationView);
    if (FAILED(hr) || pAutomationView == NULL)
        hr = iMakeUp->QueryInterface(IID_CATIADrawingView, (void**)&pAutomationView);
    if (SUCCEEDED(hr) && pAutomationView != NULL)
        hr = pAutomationView->get_GenerativeBehavior(pBehavior);
    else hr = E_FAIL;
    if (SUCCEEDED(hr) && pBehavior != NULL)
        hr = iPart->QueryInterface(IID_CATIADocument, (void**)&pSourceDocument);
    else hr = E_FAIL;
    if (SUCCEEDED(hr) && pSourceDocument != NULL)
        hr = pBehavior->put_Document(pSourceDocument);
    else hr = E_FAIL;
    if (SUCCEEDED(hr))
        hr = pBehavior->DefineFrontView(iHorizontal.GetX(), iHorizontal.GetY(), iHorizontal.GetZ(),
                                       iVertical.GetX(), iVertical.GetY(), iVertical.GetZ());
    if (SUCCEEDED(hr)) hr = pBehavior->ForceUpdate();
    if (pSourceDocument != NULL) pSourceDocument->Release();
    if (pBehavior != NULL) pBehavior->Release();
    if (pAutomationView != NULL) pAutomationView->Release();
    return hr;
}

HRESULT CreateProjection(CATIDrwFactory* iFactory, CATISheet_var iSheet,
                         CATILinkableObject* iPart, CATDrwViewType iType,
                         const char* iName, double iX, double iY,
                         const CATMathVector& iHorizontal, const CATMathVector& iVertical) {
    CATIDftViewMakeUp* pMakeUp = NULL;
    HRESULT hr = iFactory->CreateViewWithMakeUp(IID_CATIDftViewMakeUp, (void**)&pMakeUp);
    if (FAILED(hr) || pMakeUp == NULL) return E_FAIL;

    CATIView* pView = NULL;
    hr = pMakeUp->GetView(&pView);
    if (SUCCEEDED(hr) && pView != NULL) {
        pView->SetViewType(iType);
        pView->SetDoc(CATILinkableObject_var(iPart));
        CATIGenerSpec_var generSpec = pView->GetGenerSpec();
        if (generSpec != NULL_var) {
            CATMathPlane plane;
            plane.Set(CATMathPoint(0.0, 0.0, 0.0), iHorizontal, iVertical);
            generSpec->SetProjPlane(plane);
            pMakeUp->SetAxisData(iX, iY);
            hr = iSheet->AddView(pMakeUp);
            if (SUCCEEDED(hr)) {
                CATIAlias* pAlias = NULL;
                if (SUCCEEDED(pView->QueryInterface(IID_CATIAlias, (void**)&pAlias)) && pAlias != NULL) {
                    pAlias->SetAlias(iName);
                    pAlias->Release();
                }
                hr = GenerateView(pView, pMakeUp, iPart, iHorizontal, iVertical);
                if (FAILED(hr)) {
                    ShowProject3DMessage("CATIA could not define or update the generative view from the source part.");
                    pView->Release();
                    pMakeUp->Release();
                    return hr;
                }
                // DefineFrontView establishes the projection; preserve the requested label/type.
                pView->SetViewType(iType);
                CATIDftView* pDftView = NULL;
                IUnknown* pGenView = NULL;
                CATIDftGenGeomAccess* pGeometry = NULL;
                CATIUnknownList* pItems = NULL;
                unsigned int itemCount = 0;
                hr = pView->QueryInterface(IID_CATIDftView, (void**)&pDftView);
                if (SUCCEEDED(hr) && pDftView != NULL)
                    hr = pDftView->GetApplicativeExtension(IID_CATIDftGenView, &pGenView);
                if (SUCCEEDED(hr) && pGenView != NULL)
                    hr = pGenView->QueryInterface(IID_CATIDftGenGeomAccess, (void**)&pGeometry);
                if (SUCCEEDED(hr) && pGeometry != NULL)
                    hr = pGeometry->GetAllGeneratedItems(IID_CATIDftGenGeom, &pItems);
                if (SUCCEEDED(hr) && pItems != NULL) hr = pItems->Count(&itemCount);
                if (pItems != NULL) pItems->Release();
                if (pGeometry != NULL) pGeometry->Release();
                if (pGenView != NULL) pGenView->Release();
                if (pDftView != NULL) pDftView->Release();
                if (itemCount == 0) {
                    hr = E_FAIL;
                    ShowProject3DMessage("The drawing view contains no generated geometry. The projection is incomplete.");
                }

                CATIModelEvents* pEvents = NULL;
                if (SUCCEEDED(iSheet->QueryInterface(IID_CATIModelEvents, (void**)&pEvents)) && pEvents != NULL) {
                    CATCreate created(pView, iSheet.operator->());
                    pEvents->Dispatch(created);
                    pEvents->Release();
                }
            }
        }
        else hr = E_FAIL;
    }
    else hr = E_FAIL;
    if (pView != NULL) pView->Release();
    pMakeUp->Release();
    return hr;
}

HRESULT ProjectPartInDrawing(CATDocument* iDrawingDocument, CATDocument* iPartDocument) {
    CATIDftDocumentServices* pDftServices    = NULL;
    CATIDrawing*             pDrawing        = NULL;
    CATISpecObject*          pDrawingSpec    = NULL;
    CATIDrwFactory*          pDrawingFactory = NULL;
    CATILinkableObject*      pPartLink       = NULL;

    HRESULT                  hr =
        iDrawingDocument->QueryInterface(IID_CATIDftDocumentServices, (void**)&pDftServices);

    if (SUCCEEDED(hr)) {
        hr = pDftServices->GetDrawing(IID_CATIDrawing, (void**)&pDrawing);
    }
    if (SUCCEEDED(hr)) {
        hr = pDrawing->QueryInterface(IID_CATISpecObject, (void**)&pDrawingSpec);
    }
    if (SUCCEEDED(hr)) {
        CATIContainer_var drawingContainer = pDrawingSpec->GetFeatContainer();
        if (drawingContainer != NULL_var) {
            hr = drawingContainer->QueryInterface(IID_CATIDrwFactory, (void**)&pDrawingFactory);
        }
        else {
            hr = E_FAIL;
        }
    }
    if (SUCCEEDED(hr)) {
        hr = iPartDocument->QueryInterface(IID_CATILinkableObject, (void**)&pPartLink);
    }
    if (SUCCEEDED(hr)) {
        CATISheet_var sheet = pDrawing->GetCurrentSheet();
        if (sheet == NULL_var) hr = E_FAIL;
        else {
            hr = CreateProjection(pDrawingFactory, sheet, pPartLink, FrontView,
                                  "Front View", 110.0, 90.0,
                                  CATMathVector(1.0, 0.0, 0.0), CATMathVector(0.0, 0.0, 1.0));
            if (SUCCEEDED(hr))
                hr = CreateProjection(pDrawingFactory, sheet, pPartLink, TopView,
                                      "Top View", 110.0, 150.0,
                                      CATMathVector(1.0, 0.0, 0.0), CATMathVector(0.0, 1.0, 0.0));
            if (SUCCEEDED(hr))
                hr = CreateProjection(pDrawingFactory, sheet, pPartLink, LeftView,
                                      "Left View", 50.0, 90.0,
                                      CATMathVector(0.0, -1.0, 0.0), CATMathVector(0.0, 0.0, 1.0));
            CATIRedrawEvent* pRedraw = NULL;
            if (SUCCEEDED(sheet->QueryInterface(IID_CATIRedrawEvent, (void**)&pRedraw)) && pRedraw != NULL) {
                pRedraw->Redraw();
                pRedraw->Release();
            }
        }
    }

    if (pPartLink != NULL) {
        pPartLink->Release();
    }
    if (pDrawingFactory != NULL) {
        pDrawingFactory->Release();
    }
    if (pDrawingSpec != NULL) {
        pDrawingSpec->Release();
    }
    if (pDrawing != NULL) {
        pDrawing->Release();
    }
    if (pDftServices != NULL) {
        pDftServices->Release();
    }
    return hr;
}

} // namespace

Project3DCommand::Project3DCommand()
    : CATCommand("Project3DCommand", CATCommandModeExclusive) {}

Project3DCommand::~Project3DCommand() {}

CATStatusChangeRC Project3DCommand::Activate(CATCommand* iFromClient, CATNotification* iEvent) {
    CATDocument*  pPart    = Create3DCommand::GetCreatedPart();
    CATFrmEditor* pEditor  = CATFrmEditor::GetCurrentEditor();
    CATDocument*  pDrawing = pEditor == NULL ? NULL : pEditor->GetDocument();

    if (pPart == NULL) {
        ShowProject3DMessage("Create a 3D part first.");
    }
    else if (pDrawing == NULL) {
        ShowProject3DMessage("Open a CATDrawing document before projecting.");
    }
    else {
        HRESULT hr = ProjectPartInDrawing(pDrawing, pPart);
        if (SUCCEEDED(hr)) {
            ShowProject3DMessage("Front, top, and left views were generated from the last created part.");
        }
        else {
            ShowProject3DMessage("Could not generate all three views. Activate a CATDrawing and ensure "
                                 "the last created part is still open and contains a solid. Partial views may remain.");
        }
    }

    // Release exclusive command control after this one-shot operation.
    RequestDelayedDestruction();
    return CATStatusChangeRCCompleted;
}

CATStatusChangeRC Project3DCommand::Desactivate(CATCommand* iFromClient, CATNotification* iEvent) {
    return CATStatusChangeRCCompleted;
}

CATStatusChangeRC Project3DCommand::Cancel(CATCommand* iFromClient, CATNotification* iEvent) {
    RequestDelayedDestruction();
    return CATStatusChangeRCCompleted;
}
