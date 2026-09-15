// NewBomAssemblyTreeDlg.cpp
// CATIA CAA Assembly Tree Dialog Implementation
//
// This dialog displays the assembly tree structure using CATDlgTree control.

#include "NewBomAssemblyTreeDlg.h"
#include "CATDlgGridConstraints.h"
#include "CATUnicodeString.h"
#include "CATFrmEditor.h"
// #include "CATIDocument.h"
#include "CATIProduct.h"

// Implement the class
CATImplementClass(NewBomAssemblyTreeDlg, Implementation, CATDlgDialog, CATNull);

// Helper function to build Unicode string from wchar_t
static CATUnicodeString U(const wchar_t* iText) {
    CATUnicodeString text;
    text.BuildFromWChar(iText);
    return text;
}

//=============================================================================
// Constructor
//=============================================================================
NewBomAssemblyTreeDlg::NewBomAssemblyTreeDlg(CATDialog* iParent, const CATString& iObjectName)
    : CATDlgDialog(iParent, iObjectName, CATDlgWndModal | CATDlgWndBtnClose | CATDlgGridLayout) {
    // m_pAssemblyTree = NULL;
    m_pStatusLabel = NULL;

    BuildDialog();
}

//=============================================================================
// Destructor
//=============================================================================
NewBomAssemblyTreeDlg::~NewBomAssemblyTreeDlg() {}

//=============================================================================
// BuildDialog - Create and layout dialog controls
//=============================================================================
void NewBomAssemblyTreeDlg::BuildDialog() {
    // Set dialog title
    SetTitle(U(L"\x83B7\x53D6\x88F6\x914D\x6811"));

    // Row 0: Tree control label
    CATDlgLabel* pTreeLabel = new CATDlgLabel(this, "TreeLabel");
    pTreeLabel->SetTitle(U(L"\x83C5\x914D\x6811\x7ED3\x6784"));
    pTreeLabel->SetGridConstraints(0, 0, 1, 1, CATGRID_LEFT);

    // Row 1: Tree control (main content)
    // CATDlgTree displays hierarchical structure
    // m_pAssemblyTree = new CATDlgTree(this, "AssemblyTree", CATDlgTreeMultiSelect);
    // m_pAssemblyTree->SetGridConstraints(1, 0, 1, 2, CATGRID_LEFT | CATGRID_TOP);
    // // Set tree size
    // m_pAssemblyTree->SetVisibleRowCount(15);
    // m_pAssemblyTree->SetVisibleColumnWidth(40);

    // Row 2: Status label
    m_pStatusLabel = new CATDlgLabel(this, "StatusLabel");
    m_pStatusLabel->SetTitle(
        U(L"\x51C6\x5907\x83B7\x53D6\x83C5\x914D\x6811...")); // 
    m_pStatusLabel->SetGridConstraints(2, 0, 1, 2, CATGRID_CENTER);

    // Set column resize properties
    SetGridColumnResizable(0, 0);
    SetGridColumnResizable(1, 0);

    // Now retrieve and populate assembly structure
    // GetAssemblyStructure();
}
