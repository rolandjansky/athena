/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasAlg/PreEventAction.h"
#include "G4AtlasAlg/PreEventActionManager.h"

PreEventAction::PreEventAction(std::string n):name(n)
{
	PreEventActionManager::GetPreEventActionManager()->
				RegisterAction(this);
}
std::string PreEventAction::GetName()
{
	return name;
}
