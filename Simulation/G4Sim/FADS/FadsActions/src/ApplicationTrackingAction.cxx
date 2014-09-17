/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsActions/ApplicationTrackingAction.h"
#include "FadsActions/FadsTrackingAction.h"

namespace FADS {

G4TrackingManager* ApplicationTrackingAction::GetTrackingManager()
{
	return FadsTrackingAction::GetTrackingAction()->GetTrackingManager();
}
	
void ApplicationTrackingAction::Register()
{
	FadsTrackingAction::GetTrackingAction()->RegisterApplicationAction(this);
}

}	// end namespace
