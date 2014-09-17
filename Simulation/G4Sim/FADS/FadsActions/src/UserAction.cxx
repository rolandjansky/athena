/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsActions/UserAction.h"
#include "FadsActions/ActionsStore.h"
#include "FadsActions/FadsSteppingAction.h"
#include "FadsActions/FadsEventAction.h"
#include "FadsActions/FadsRunAction.h"
#include <stdexcept>

namespace FADS {

void UserAction::RegisterToStore()
{
	ActionsStore::GetActionsStore()->Register(this);
}

void UserAction::UnregisterFromStore()
{
	ActionsStore::GetActionsStore()->Unregister(this);
}

void UserAction::ResetPriority(int /*i*/)
{;}

void UserAction::RegisterFor(actionType t)
{
	if (t==Step)
	{
		FadsSteppingAction::GetSteppingAction()->RegisterAction(this);
	}
	else if (t==BeginOfEvent || t==EndOfEvent)
	{
		FadsEventAction::GetEventAction()->RegisterAction(this,t);
	}
	else if (t==BeginOfRun || t==EndOfRun)	
	{
		FadsRunAction::GetRunAction()->RegisterAction(this,t);
	}
}

void UserAction::RegisterFor(std::string s)
{
	if (s=="Step") RegisterFor(Step);
	else if (s=="BeginOfEvent") RegisterFor(BeginOfEvent);
	else if (s=="EndOfEvent") RegisterFor(EndOfEvent);
	else if (s=="EndOfRun") RegisterFor(EndOfRun);
	else if (s=="BeginOfRun") RegisterFor(BeginOfRun);
	else throw std::invalid_argument( "Don't know what to do for "+s );
}

void UserAction::SetProperty(std::string propName,std::string propValue){
  theProperties[propName]=propValue;
}

}	// end namespace
