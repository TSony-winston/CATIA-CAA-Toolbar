// NewBomAssemblyTreeCmd.cpp
// CATIA CAA Assembly Tree Command Implementation
//
// This command creates and displays the assembly tree dialog.
// Follows the same pattern as NewBomCmd.

#include "NewBomAssemblyTreeCmd.h"
#include "NewBomAssemblyTreeDlg.h"
#include "CATApplicationFrame.h"
#include "CATCreateExternalObject.h"

//=============================================================================
// CLASS REGISTRATION
//=============================================================================
CATImplementClass(NewBomAssemblyTreeCmd, Implementation, CATCommand, CATNull);

CATCreateClass(NewBomAssemblyTreeCmd);

//=============================================================================
// CONSTRUCTOR
//=============================================================================
NewBomAssemblyTreeCmd::NewBomAssemblyTreeCmd()
    : CATCommand(NULL, "NewBomAssemblyTreeCmd") {

    m_pDialog = NULL;
    RequestStatusChange(CATCommandMsgRequestSharedMode);
}

//=============================================================================
// DESTRUCTOR
//=============================================================================
NewBomAssemblyTreeCmd::~NewBomAssemblyTreeCmd() {
    if (m_pDialog) {
        m_pDialog->RequestDelayedDestruction();
        m_pDialog = NULL;
    }
}

//=============================================================================
// Activate - COMMAND START
//=============================================================================
CATStatusChangeRC NewBomAssemblyTreeCmd::Activate(CATCommand* iCmd, CATNotification* iNotif) {
    // Step 1: Get CATIA Application Frame
    CATApplicationFrame* pFrame = CATApplicationFrame::GetFrame();
    if (!pFrame) {
        return CATStatusChangeRCCompleted;
    }

    // Step 2: Get Main Window
    CATDialog* pWindow = pFrame->GetMainWindow();
    if (!pWindow) {
        return CATStatusChangeRCCompleted;
    }

    // Step 3: Clean up any existing dialog
    if (m_pDialog) {
        m_pDialog->RequestDelayedDestruction();
        m_pDialog = NULL;
    }

    // Step 4: Create New Dialog
    m_pDialog = new NewBomAssemblyTreeDlg(pWindow, "AssemblyTreeDialog");

    if (!m_pDialog) {
        return CATStatusChangeRCCompleted;
    }

    // Step 5: Connect Dialog Events
    m_pDialog->SetFather(this);

    AddAnalyseNotificationCB(m_pDialog, m_pDialog->GetDiaCLOSENotification(),
                             (CATCommandMethod)&NewBomAssemblyTreeCmd::CloseDialog, NULL);

    AddAnalyseNotificationCB(m_pDialog, m_pDialog->GetWindCloseNotification(),
                             (CATCommandMethod)&NewBomAssemblyTreeCmd::CloseDialog, NULL);

    // Step 6: Show Dialog
    m_pDialog->SetVisibility(CATDlgShow);

    return CATStatusChangeRCCompleted;
}

//=============================================================================
// CloseDialog - HANDLE DIALOG CLOSE
//=============================================================================
void NewBomAssemblyTreeCmd::CloseDialog(CATCommand* iCmd, CATNotification* iNotif,
                                        CATCommandClientData iClientData) {
    if (m_pDialog) {
        m_pDialog->SetVisibility(CATDlgHide);
    }

    RequestDelayedDestruction();
}

//=============================================================================
// Desactivate - COMMAND PAUSE
//=============================================================================
CATStatusChangeRC NewBomAssemblyTreeCmd::Desactivate(CATCommand* iCmd, CATNotification* iNotif) {
    return CATStatusChangeRCCompleted;
}

//=============================================================================
// Cancel - COMMAND ABORT
//=============================================================================
CATStatusChangeRC NewBomAssemblyTreeCmd::Cancel(CATCommand* iCmd, CATNotification* iNotif) {
    if (m_pDialog) {
        m_pDialog->RequestDelayedDestruction();
        m_pDialog = NULL;
    }

    return CATStatusChangeRCCompleted;
}