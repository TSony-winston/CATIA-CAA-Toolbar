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

HRESULT ProjectPartInDrawing(CATDocument* iDrawingDocument, CATDocument* iPartDocument) {
    CATIDftDocumentServices* pDftServices    = NULL;
    CATIDrawing*             pDrawing        = NULL;
    CATISpecObject*          pDrawingSpec    = NULL;
    CATIDrwFactory*          pDrawingFactory = NULL;
    CATILinkableObject*      pPartLink       = NULL;
    CATIView*                pNewView        = NULL;
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
        hr = pDrawingFactory->CreateView(IID_CATIView, (void**)&pNewView);
    }
    if (SUCCEEDED(hr) && pNewView != NULL) {
        pNewView->SetDoc(CATILinkableObject_var(pPartLink));
        CATISheet_var currentSheet = pDrawing->GetCurrentSheet();
        if (currentSheet != NULL_var) {
            CATIView_var newView     = pNewView;
            double       origin[ 2 ] = {150.0, 150.0};
            currentSheet->AddView(newView, origin);
        }
        else {
            hr = E_FAIL;
        }
    }

    if (pNewView != NULL) {
        pNewView->Release();
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
            ShowProject3DMessage("The 3D view was projected into the drawing.");
        }
        else {
            ShowProject3DMessage("Projection requires an active CATPart or CATProduct and "
                                 "an active CATDrawing.");
        }
    }

    return CATStatusChangeRCCompleted;
}

CATStatusChangeRC Project3DCommand::Desactivate(CATCommand* iFromClient, CATNotification* iEvent) {
    return CATStatusChangeRCCompleted;
}

CATStatusChangeRC Project3DCommand::Cancel(CATCommand* iFromClient, CATNotification* iEvent) {
    RequestDelayedDestruction();
    return CATStatusChangeRCCompleted;
}
