#ifndef Create3DCommand_h
#define Create3DCommand_h

#include "CATCommand.h"

class CATDocument;

class Create3DCommand : public CATCommand {
    CATDeclareClass;

public:
    Create3DCommand();
    virtual ~Create3DCommand();

    virtual CATStatusChangeRC Activate(CATCommand* iFromClient, CATNotification* iEvent);
    virtual CATStatusChangeRC Desactivate(CATCommand* iFromClient, CATNotification* iEvent);
    virtual CATStatusChangeRC Cancel(CATCommand* iFromClient, CATNotification* iEvent);

    static CATDocument* GetCreatedPart();

private:
    static CATDocument* _pCreatedPart;
};

#endif
