#include "InitMessage.h"
#include "CATCreateExternalObject.h"
#include "CATApplicationFrame.h"
#include "CATDlgNotify.h"
#include "CATFrmEditor.h"
#include "CATDocument.h"
#include <windows.h>
#include "CATIDocId.h"
#include "CATIDrawing.h"
#include "CATISheet.h"
#include "CATIDrwFactory.h"
#include "CATIDftSheetFormat.h"
#include "CATISpecObject.h"
#include "CATIContainer.h"
#include "DraftingItfCPP.h"
#include "CATInit.h"
#include "CATIDftDocumentServices.h"

CATCreateClass(InitMessage);

namespace {

void DisplayMessage(CATDlgWindow* iParent, const char* iMessage) {
    CATDlgNotify* pNotifyDlg = new CATDlgNotify(
        iParent, "HelloMessage", CATDlgNfyInformation | CATDlgNfyOK | CATDlgWndModal);
    if (pNotifyDlg != NULL) {
        pNotifyDlg->DisplayBlocked(iMessage, "CATIA CAA Test");
        pNotifyDlg->RequestDelayedDestruction();
    }
}

CATISheet* GetFirstSheet(CATIDrawing* iDrawing) {
    CATISheet* pFirstSheet                 = NULL;
    CATLISTV(CATISpecObject_var) sheetList = iDrawing->GetSheetList();

    if (sheetList.Size() > 0) {
        CATISpecObject_var firstSheetSpec = sheetList[ 1 ];
        if (firstSheetSpec != NULL_var) {
            firstSheetSpec->QueryInterface(IID_CATISheet, (void**)&pFirstSheet);
        }
    }

    return pFirstSheet;
}

void CopyViewProperties(CATISheet* iSourceSheet, CATISheet* iTargetSheet) {
    CATIView_var sourceBackgroundView = iSourceSheet->GetBackgroundView();
    CATIView_var sourceMainView       = iSourceSheet->GetMainView();
    CATIView_var targetBackgroundView = iTargetSheet->GetBackgroundView();
    CATIView_var targetMainView       = iTargetSheet->GetMainView();

    if (sourceBackgroundView != NULL_var && targetBackgroundView != NULL_var) {
        targetBackgroundView->SetComment(sourceBackgroundView->GetComment());
        targetBackgroundView->SetViewType(sourceBackgroundView->GetViewType());
    }

    if (sourceMainView != NULL_var && targetMainView != NULL_var) {
        targetMainView->SetComment(sourceMainView->GetComment());
        targetMainView->SetViewType(sourceMainView->GetViewType());
    }
}

void CopySheetProperties(CATISheet* iSourceSheet, CATISheet* iTargetSheet) {
    boolean isDetail = FALSE;
    iSourceSheet->IsDetail(&isDetail);

    iTargetSheet->SetComment(iSourceSheet->GetComment());
    iTargetSheet->SetScale(iSourceSheet->GetScale());
    if (isDetail) {
        iTargetSheet->SetAsDetail();
    }

    CATIDftSheetFormat* pSourceFormat = NULL;
    CATIDftSheetFormat* pTargetFormat = NULL;
    CATSheetOrientation orientation;
    CATBoolean          displayMode = TRUE;

    if (SUCCEEDED(iSourceSheet->QueryInterface(IID_CATIDftSheetFormat, (void**)&pSourceFormat))) {
        pSourceFormat->GetSheetOrientation(orientation);
        pSourceFormat->GetSheetDisplayMode(displayMode);

        if (SUCCEEDED(
                iTargetSheet->QueryInterface(IID_CATIDftSheetFormat, (void**)&pTargetFormat))) {
            pTargetFormat->SetSheetOrientation(orientation);
            pTargetFormat->SetSheetDisplayMode(displayMode);
            pTargetFormat->Release();
        }
        pSourceFormat->Release();
    }

    CopyViewProperties(iSourceSheet, iTargetSheet);
}

HRESULT AddSheetCopy(CATIDrawing* iDrawing) {
    CATISheet* pFirstSheet = GetFirstSheet(iDrawing);
    if (pFirstSheet == NULL) {
        return E_FAIL;
    }

    CATISpecObject* pSpecDrawing    = NULL;
    CATIDrwFactory* pDrawingFactory = NULL;
    CATISheet*      pNewSheet       = NULL;
    HRESULT         hr = iDrawing->QueryInterface(IID_CATISpecObject, (void**)&pSpecDrawing);

    if (SUCCEEDED(hr)) {
        CATIContainer_var drawingContainer = pSpecDrawing->GetFeatContainer();
        if (drawingContainer != NULL_var) {
            hr = drawingContainer->QueryInterface(IID_CATIDrwFactory, (void**)&pDrawingFactory);
        }
    }

    if (SUCCEEDED(hr) && pDrawingFactory != NULL) {
        hr = pDrawingFactory->CreateSheet(IID_CATISheet, (void**)&pNewSheet);
    }

    if (SUCCEEDED(hr) && pNewSheet != NULL) {
        CATISheet_var newSheet = pNewSheet;
        hr = iDrawing->AppendSheet(newSheet, CATIDrawing::Design, CATIDrawing::Current);
        if (SUCCEEDED(hr)) {
            CopySheetProperties(pFirstSheet, pNewSheet);
        }
    }
    else if (SUCCEEDED(hr)) {
        hr = E_FAIL;
    }

    if (pNewSheet != NULL) {
        pNewSheet->Release();
    }
    if (pDrawingFactory != NULL) {
        pDrawingFactory->Release();
    }
    if (pSpecDrawing != NULL) {
        pSpecDrawing->Release();
    }
    pFirstSheet->Release();
    return hr;
}

}

//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------
InitMessage::InitMessage()
    : CATCommand("InitMessage", CATCommandModeExclusive) {}
//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------
InitMessage::~InitMessage() {}

//-------------------------------------------------------------------------
// Activate
//-------------------------------------------------------------------------
CATStatusChangeRC InitMessage::Activate(CATCommand* iFromClient, CATNotification* iEvtDat) {
    CATApplicationFrame* pApplication = CATApplicationFrame::GetFrame();
    if (pApplication == NULL) {
        return CATStatusChangeRCCompleted;
    }

    CATDlgWindow* pMainWindow = pApplication->GetMainWindow();
    if (pMainWindow == NULL) {
        return CATStatusChangeRCCompleted;
    }

    CATFrmEditor* pEditor = CATFrmEditor::GetCurrentEditor();
    if (pEditor == NULL) {
        DisplayMessage(pMainWindow, "Hello World!");
        return CATStatusChangeRCCompleted;
    }

    CATDocument* pDocument = pEditor->GetDocument();
    CATIDrawing* pDrawing  = pDocument == NULL ? NULL : GetDrawingFromDocument(pDocument);
    if (pDrawing == NULL) 
    {
        DisplayMessage(pMainWindow, "This is not a CATDrawing document.");
        return CATStatusChangeRCCompleted;
    }

    HRESULT hr = AddSheetCopy(pDrawing);
    pDrawing->Release();
    DisplayMessage(pMainWindow,
                   SUCCEEDED(hr) ? "A new sheet was added." : "The sheet could not be added.");

    return CATStatusChangeRCCompleted;
}

//-------------------------------------------------------------------------
// Desactivate
//-------------------------------------------------------------------------
CATStatusChangeRC InitMessage::Desactivate(CATCommand* iFromClient, CATNotification* iEvtDat) {
    return CATStatusChangeRCCompleted;
}

//-------------------------------------------------------------------------
// Cancel
//-------------------------------------------------------------------------
CATStatusChangeRC InitMessage::Cancel(CATCommand* iFromClient, CATNotification* iEvtDat) {
    RequestDelayedDestruction();

    return CATStatusChangeRCCompleted;
}

// -------------------------------------------------------------------------
// GetDrawingFromDocument
// -------------------------------------------------------------------------

CATIDrawing* InitMessage::GetDrawingFromDocument(CATDocument* iDocument) {
    if (iDocument == NULL) {
        return NULL;
    }

    CATIDftDocumentServices* pDftDocServices;
    HRESULT hr = iDocument->QueryInterface(IID_CATIDftDocumentServices, (void**)&pDftDocServices);

    if (FAILED(hr) || pDftDocServices == NULL) {
        return NULL;
    }

    CATIDrawing* pDrawing = NULL;
    hr                    = pDftDocServices->GetDrawing(IID_CATIDrawing, (void**)&pDrawing);

    pDftDocServices->Release();
    pDftDocServices = NULL;

    if (FAILED(hr) || pDrawing == NULL) {
        return NULL;
    }

    return pDrawing;
}