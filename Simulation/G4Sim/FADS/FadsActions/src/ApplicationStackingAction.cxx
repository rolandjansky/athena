/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsActions/ApplicationStackingAction.h"
#include "FadsActions/FadsStackingAction.h"

namespace FADS {

G4StackManager* ApplicationStackingAction::GetStackManager()
{
	return FadsStackingAction::GetStackingAction()->GetStackManager();
}

void ApplicationStackingAction::Register()
{
	FadsStackingAction::GetStackingAction()->RegisterApplicationAction(this);
}

}	// end namespace
