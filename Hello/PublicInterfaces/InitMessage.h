// COPYRIGHT Dassault Systemes 2026
//===================================================================
//
// InitMessage.h
// The command: InitMessage
//
//===================================================================

#ifndef InitMessage_h
#define InitMessage_h

#include "CATCommand.h"
#include "CATIDrawing.h"

class InitMessage : public CATCommand {
public:
    InitMessage();

    virtual ~InitMessage();

    virtual CATStatusChangeRC Activate(CATCommand* iFromClient, CATNotification* iEvtDat);

    virtual CATStatusChangeRC Desactivate(CATCommand* iFromClient, CATNotification* iEvtDat);

    virtual CATStatusChangeRC Cancel(CATCommand* iFromClient, CATNotification* iEvtDat);

    virtual CATIDrawing* GetDrawingFromDocument(CATDocument* iDocument);
};

#endif