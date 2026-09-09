#ifndef Project3DCommand_h
#define Project3DCommand_h

#include "CATCommand.h"

class Project3DCommand : public CATCommand {
    CATDeclareClass;

public:
    Project3DCommand();
    virtual ~Project3DCommand();

    virtual CATStatusChangeRC Activate(CATCommand* iFromClient, CATNotification* iEvent);
    virtual CATStatusChangeRC Desactivate(CATCommand* iFromClient, CATNotification* iEvent);
    virtual CATStatusChangeRC Cancel(CATCommand* iFromClient, CATNotification* iEvent);
};

#endif
