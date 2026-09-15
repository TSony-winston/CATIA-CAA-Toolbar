// =============================================================================
// NewBomAssemblyTreeDlg.h
// CATIA CAA Assembly Tree Dialog Header File
// =============================================================================
//
// This dialog displays the assembly tree structure using CATDlgTree control.
//
// =============================================================================

#ifndef __NewBomAssemblyTreeDlg_h__
#define __NewBomAssemblyTreeDlg_h__

#include "CATDlgDialog.h"
// #include "CATDlgTree.h"
#include "CATDlgLabel.h"
#include "CATUnicodeString.h"
#include "CATI18NTypes.h"

/**
 * @class NewBomAssemblyTreeDlg
 * @brief Dialog for displaying assembly tree structure
 *
 * Uses CATDlgTree to show hierarchical product/part structure.
 * Retrieves current document's assembly structure via CATIProduct.
 */
class NewBomAssemblyTreeDlg : public CATDlgDialog {
    CATDeclareClass;

private:
    // CATDlgTree*  m_pAssemblyTree; // Tree control for hierarchy
    CATDlgLabel* m_pStatusLabel; // Status information label

public:
    NewBomAssemblyTreeDlg(CATDialog* iParent, const CATString& iObjectName);
    virtual ~NewBomAssemblyTreeDlg();

    /**
     * BuildDialog - Create and layout dialog controls
     */
    void BuildDialog();

private:
    /**
     * GetAssemblyStructure - Retrieve assembly hierarchy from current document
     */
    void GetAssemblyStructure();

    /**
     * PopulateTree - Fill tree control with assembly nodes
     */
    void PopulateTree();
};

#endif // __NewBomAssemblyTreeDlg_h__
