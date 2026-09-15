// COPYRIGHT Dassault Systemes 2026
#ifndef HelloGeneralWksAddin_H
#define HelloGeneralWksAddin_H

//===========================================================================
//  Abstract of the class:
//  ----------------------
//
//  Class which represents an addin of the General Workshop.
//  It implements the CATIAfrGeneralWksAddin interface which is
//  specified by the General workshop as the interface to implement in its
//  addins.
//
//  The addin adds a toolbar (with one button bound to the InitMessage
//  command) in all the workbenches of the application.
//
//===========================================================================

// System Framework
#include "CATBaseUnknown.h"

class CATCmdContainer;

class HelloGeneralWksAddin : public CATBaseUnknown {
    // Used in conjunction with CATImplementClass in the .cpp file
    CATDeclareClass;

public:
    HelloGeneralWksAddin();
    virtual ~HelloGeneralWksAddin();

    // Instantiates the command headers for the commands of the addin
    void CreateCommands();

    // Creates toolbars and arranges the commands inside
    CATCmdContainer* CreateToolbars();

private:
    // Copy constructor, not implemented
    // Set as private to prevent from compiler automatic creation as public.
    HelloGeneralWksAddin(const HelloGeneralWksAddin& iObjectToCopy);

    // Assignment operator, not implemented
    // Set as private to prevent from compiler automatic creation as public.
    HelloGeneralWksAddin& operator=(const HelloGeneralWksAddin& iObjectToCopy);
};

#endif
