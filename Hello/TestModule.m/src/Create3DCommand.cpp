#include "Create3DCommand.h"
#include "CATApplicationFrame.h"
#include "CATCreateExternalObject.h"
#include "CATDlgNotify.h"
#include "CATDocument.h"
#include "CATDocumentServices.h"
#include "CATIDocRoots.h"
#include "CATIDocId.h"
#include "CATIProduct.h"
#include "CATIModelEvents.h"
#include "CATCreate.h"
#include "CATIRedrawEvent.h"
#include "CATFrmEditor.h"
#include "CATInit.h"
#include "CATIPrtContainer.h"
#include "CATIPrtPart.h"
#include "CATISketchFactory.h"
#include "CATI2DWFFactory.h"
#include "CATISketch.h"
#include "CATIPrtFactory.h"
#include "CATIPad.h"
#include "CATLimitDefs.h"
#include "CATMathDirection.h"

CATImplementClass(Create3DCommand, Implementation, CATCommand, CATNull);
CATCreateClass(Create3DCommand);

CATDocument* Create3DCommand::_pCreatedPart = NULL;

namespace {

void ShowCreate3DMessage(const char* iMessage) {
    CATApplicationFrame* pApplication = CATApplicationFrame::GetFrame();
    CATDlgWindow*        pWindow      = pApplication == NULL ? NULL : pApplication->GetMainWindow();
    if (pWindow == NULL) {
        return;
    }

    CATDlgNotify* pNotify = new CATDlgNotify(pWindow, "Create3DMessage",
                                             CATDlgNfyInformation | CATDlgNfyOK | CATDlgWndModal);
    if (pNotify != NULL) {
        pNotify->DisplayBlocked(iMessage, "CATIA CAA Test");
        pNotify->RequestDelayedDestruction();
    }
}

CATIProduct_var GetRootProduct(CATDocument* iDocument) {
    CATIProduct_var rootProduct = NULL_var;
    CATIDocRoots*   pDocRoots   = NULL;
    if (iDocument != NULL &&
        SUCCEEDED(iDocument->QueryInterface(IID_CATIDocRoots, (void**)&pDocRoots)) &&
        pDocRoots != NULL) {
        CATListValCATBaseUnknown_var* pRoots = pDocRoots->GiveDocRoots();
        if (pRoots != NULL) {
            if (pRoots->Size() > 0) {
                rootProduct = (*pRoots)[ 1 ];
            }
            delete pRoots;
        }
        pDocRoots->Release();
    }
    return rootProduct;
}

HRESULT CreateBox(CATDocument* iPartDocument) {
    CATInit*          pInit          = NULL;
    CATIPrtContainer* pPartContainer = NULL;
    HRESULT           hr             = iPartDocument->QueryInterface(IID_CATInit, (void**)&pInit);
    if (FAILED(hr)) {
        return hr;
    }

    pPartContainer = (CATIPrtContainer*)pInit->GetRootContainer("CATIPrtContainer");
    pInit->Release();
    if (pPartContainer == NULL) {
        return E_FAIL;
    }
    // I have to add for defining work place.

    CATISketchFactory_var        sketchFactory   = pPartContainer;
    CATIPrtPart_var              part            = pPartContainer->GetPart();
    CATListValCATISpecObject_var referencePlanes = part->GetReferencePlanes();
    if (referencePlanes.Size() == 0) {
        pPartContainer->Release();
        return E_FAIL;
    }

    CATISpecObject_var sketchPlane = referencePlanes[ 1 ];
    CATISketch_var     sketch      = sketchFactory->CreateSketch(sketchPlane);
    if (sketch == NULL_var) {
        pPartContainer->Release();
        return E_FAIL;
    }

    CATI2DWFFactory_var geometryFactory = sketch;
    double              point1[ 2 ]     = {10.0, 5.0};
    double              point2[ 2 ]     = {-10.0, 5.0};
    double              point3[ 2 ]     = {-10.0, -5.0};
    double              point4[ 2 ]     = {10.0, -5.0};
    sketch->OpenEdition();
    geometryFactory->CreateLine(point1, point2);
    geometryFactory->CreateLine(point2, point3);
    geometryFactory->CreateLine(point3, point4);
    geometryFactory->CreateLine(point4, point1);
    sketch->CloseEdition();

    CATIPrtFactory_var partFactory = pPartContainer;
    CATISpecObject_var padObject   = partFactory->CreatePad(sketch);
    pPartContainer->Release();
    if (padObject == NULL_var) {
        return E_FAIL;
    }

    CATIPad_var      pad = padObject;
    CATMathDirection direction(0.0, 0.0, 1.0);
    pad->ModifyDirection(direction);
    pad->ModifyEndType(catOffsetLimit);
    pad->ModifyEndOffset(20.0);
    pad->ModifyStartType(catOffsetLimit);
    pad->ModifyStartOffset(0.0);
    padObject->Update();
    return S_OK;
}

} // namespace

Create3DCommand::Create3DCommand()
    : CATCommand("Create3DCommand", CATCommandModeExclusive) {}

Create3DCommand::~Create3DCommand() {}

CATDocument* Create3DCommand::GetCreatedPart() {
    return _pCreatedPart;
}

CATStatusChangeRC Create3DCommand::Activate(CATCommand* iFromClient, CATNotification* iEvent) {
    CATFrmEditor* pEditor   = CATFrmEditor::GetCurrentEditor();
    CATDocument*  pAssembly = pEditor == NULL ? NULL : pEditor->GetDocument();
    if (pAssembly == NULL) {
        ShowCreate3DMessage("Open a CATProduct document before creating a part.");
        return CATStatusChangeRCCompleted;
    }

    CATIDocId*       pDocId = NULL;
    CATUnicodeString documentType;
    HRESULT          typeResult = pAssembly->GetDocId(&pDocId);
    if (SUCCEEDED(typeResult) && pDocId != NULL) {
        typeResult = pDocId->GetType(documentType);
    }
    else {
        typeResult = E_FAIL;
    }
    if (pDocId != NULL) {
        pDocId->Release();
    }
    if (FAILED(typeResult)) {
        ShowCreate3DMessage("The active document's type could not be retrieved.");
        return CATStatusChangeRCCompleted;
    }
    if (documentType != CATUnicodeString("CATProduct")) {
        ShowCreate3DMessage("Activate a CATProduct document before creating a part.");
        return CATStatusChangeRCCompleted;
    }

    CATIProduct_var assemblyRoot = GetRootProduct(pAssembly);
    if (assemblyRoot == NULL_var) {
        ShowCreate3DMessage("The active assembly's root product could not be found.");
        return CATStatusChangeRCCompleted;
    }

    // The part is displayed through its assembly instance, without a separate window.
    CATDocument* pPart = NULL;
    HRESULT      hr    = CATDocumentServices::New("Part", pPart);
    if (FAILED(hr) || pPart == NULL) {
        ShowCreate3DMessage("The CATPart could not be created.");
        return CATStatusChangeRCCompleted;
    }

    hr = CreateBox(pPart);
    if (FAILED(hr)) {
        CATDocumentServices::Remove(*pPart);
        ShowCreate3DMessage("The sample box could not be built.");
        return CATStatusChangeRCCompleted;
    }

    const int       childCountBefore = assemblyRoot->GetChildrenCount();
    CATIProduct_var instance         = NULL_var;
    {
        CATIProduct_var partRoot = GetRootProduct(pPart);
        if (partRoot != NULL_var) {
            instance = assemblyRoot->AddProduct(partRoot);
        }
    }
    if (instance == NULL_var) {
        CATDocumentServices::Remove(*pPart);
        ShowCreate3DMessage("The part could not be added to the active assembly.");
        return CATStatusChangeRCCompleted;
    }

    _pCreatedPart = pPart;
    if (assemblyRoot->GetChildrenCount() != childCountBefore + 1) {
        ShowCreate3DMessage(
            "CATIA returned a component, but the assembly child count did not increase.");
        return CATStatusChangeRCCompleted;
    }

    // Notify both the 3D representation and the specification tree.
    CATIModelEvents* pModelEvents = NULL;
    HRESULT visuResult = assemblyRoot->QueryInterface(IID_CATIModelEvents, (void**)&pModelEvents);
    if (SUCCEEDED(visuResult) && pModelEvents != NULL) {
        CATCreate created(instance.operator->(), assemblyRoot.operator->());
        pModelEvents->Dispatch(created);
        pModelEvents->Release();
    }
    else {
        visuResult = E_FAIL;
    }

    CATIRedrawEvent* pRedraw = NULL;
    HRESULT treeResult       = assemblyRoot->QueryInterface(IID_CATIRedrawEvent, (void**)&pRedraw);
    if (SUCCEEDED(treeResult) && pRedraw != NULL) {
        pRedraw->Redraw();
        pRedraw->Release();
    }
    else {
        treeResult = E_FAIL;
    }

    if (FAILED(visuResult) || FAILED(treeResult)) {
        ShowCreate3DMessage(
            "The assembly contains the new part, but its display could not be refreshed.");
    }
    RequestDelayedDestruction();
    return CATStatusChangeRCCompleted;
}

CATStatusChangeRC Create3DCommand::Desactivate(CATCommand* iFromClient, CATNotification* iEvent) {
    return CATStatusChangeRCCompleted;
}

CATStatusChangeRC Create3DCommand::Cancel(CATCommand* iFromClient, CATNotification* iEvent) {
    RequestDelayedDestruction();
    return CATStatusChangeRCCompleted;
}
