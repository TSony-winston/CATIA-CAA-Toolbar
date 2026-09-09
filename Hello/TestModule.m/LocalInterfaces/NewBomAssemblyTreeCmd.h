// =============================================================================
// NewBomAssemblyTreeCmd.h
// CATIA CAA Assembly Tree Command Header File
// =============================================================================
//
// This command is triggered when user clicks "获取装配树" menu item.
// It creates and displays the AssemblyTree dialog showing product hierarchy.
//
// =============================================================================

#ifndef __NewBomAssemblyTreeCmd_h__
#define __NewBomAssemblyTreeCmd_h__

#include "CATCommand.h"

class NewBomAssemblyTreeDlg;

/**
 * @class NewBomAssemblyTreeCmd
 * @brief Command for displaying assembly tree structure
 *
 * Lifecycle:
 *   1. User clicks menu item
 *   2. CATIA creates command instance
 *   3. Activate() creates and shows dialog
 *   4. User closes dialog
 *   5. CloseDialog() destroys command
 */
class NewBomAssemblyTreeCmd : public CATCommand {
    CATDeclareClass;

private:
    NewBomAssemblyTreeDlg* m_pDialog; // Dialog pointer

public:
    NewBomAssemblyTreeCmd();
    virtual ~NewBomAssemblyTreeCmd();

    /**
     * Activate - Called when command starts
     * Creates and displays the assembly tree dialog
     */
    virtual CATStatusChangeRC Activate(CATCommand* iCmd, CATNotification* iNotif);

    /**
     * CloseDialog - Called when dialog closes
     * Hides dialog and requests command destruction
     */
    virtual void CloseDialog(CATCommand* iCmd, CATNotification* iNotif,
                             CATCommandClientData iClientData);

    /**
     * Desactivate - Called when another command takes focus
     */
    virtual CATStatusChangeRC Desactivate(CATCommand* iCmd, CATNotification* iNotif);

    /**
     * Cancel - Called when command is cancelled
     */
    virtual CATStatusChangeRC Cancel(CATCommand* iCmd, CATNotification* iNotif);
};

#endif // __NewBomAssemblyTreeCmd_h__