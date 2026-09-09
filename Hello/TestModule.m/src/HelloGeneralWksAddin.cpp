// COPYRIGHT Dassault Systemes 2026

// Local Framework
#include "HelloGeneralWksAddin.h"

// ApplicationFrame Framework
#include "CATCreateWorkshop.h" // To use NewAccess, SetAccessCommand ...

// Creates the HelloGeneralWksAddinHeader command header class
#include "CATCommandHeader.h"
MacDeclareHeader(HelloGeneralWksAddinHeader);

//--------------------------------------------------------------------------
// To create the TIE Object
//--------------------------------------------------------------------------
#include <TIE_CATIAfrGeneralWksAddin.h>
TIE_CATIAfrGeneralWksAddin(HelloGeneralWksAddin);

//--------------------------------------------------------------------------
// To declare that the class is a DataExtension of (late type)
// HelloGeneralWksAddin
//--------------------------------------------------------------------------
CATImplementClass(HelloGeneralWksAddin, DataExtension, CATBaseUnknown, HelloGeneralWksAddin);

//--------------------------------------------------------------------------
// To declare that HelloGeneralWksAddin implements CATIAfrGeneralWksAddin,
// insert the following lines in the interface dictionary (Hello.dico):
//
//   HelloGeneralWksAddin  CATIWorkbenchAddin      libTestModule
//   HelloGeneralWksAddin  CATIAfrGeneralWksAddin  libTestModule
//
//--------------------------------------------------------------------------

HelloGeneralWksAddin::HelloGeneralWksAddin() {}

HelloGeneralWksAddin::~HelloGeneralWksAddin() {}

void HelloGeneralWksAddin::CreateCommands() {
    new HelloGeneralWksAddinHeader("InitMessageHdr", "TestModule", "InitMessage", (void*)NULL);
    new HelloGeneralWksAddinHeader("Create3DCommandHdr", "TestModule", "Create3DCommand",
                                   (void*)NULL);
    new HelloGeneralWksAddinHeader("Project3DCommandHdr", "TestModule", "Project3DCommand",
                                   (void*)NULL);
}

CATCmdContainer* HelloGeneralWksAddin::CreateToolbars() {
    //----------------------
    // Hello Toolbar
    //----------------------
    NewAccess(CATCmdContainer, pHelloGeneralWksTlb, HelloGeneralWksTlb);

    NewAccess(CATCmdStarter, pHelloInitMessageStr, HelloInitMessageStr);
    NewAccess(CATCmdStarter, pCreate3DCommandStr, Create3DCommandStr);
    NewAccess(CATCmdStarter, pProject3DCommandStr, Project3DCommandStr);

    SetAccessCommand(pHelloInitMessageStr, "InitMessageHdr");
    SetAccessCommand(pCreate3DCommandStr, "Create3DCommandHdr");
    SetAccessCommand(pProject3DCommandStr, "Project3DCommandHdr");

    SetAccessChild(pHelloGeneralWksTlb, pHelloInitMessageStr);
    SetAccessNext(pHelloInitMessageStr, pCreate3DCommandStr);
    SetAccessNext(pCreate3DCommandStr, pProject3DCommandStr);

    // Visible toolbar docked at the right of the application window
    AddToolbarView(pHelloGeneralWksTlb, 1, Right);

    // The pointer of the first toolbar
    return pHelloGeneralWksTlb;
}