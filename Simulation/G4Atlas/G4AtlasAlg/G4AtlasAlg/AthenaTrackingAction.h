/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaTrackingAction_H
#define AthenaTrackingAction_H

#include "FadsActions/ApplicationTrackingAction.h"

class AthenaTrackingAction: public FADS::ApplicationTrackingAction {
public:
	AthenaTrackingAction(): FADS::ApplicationTrackingAction()
	{
	}
	void PreUserTrackingAction(const G4Track*);
	void PostUserTrackingAction(const G4Track*);
private:

};

#endif
