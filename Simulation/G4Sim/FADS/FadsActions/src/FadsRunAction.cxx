/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsActions/FadsRunAction.h"
#include "FadsActions/UserAction.h"
#include "G4Run.hh"

namespace FADS {

FadsRunAction* FadsRunAction::thePointer=0;

FadsRunAction* FadsRunAction::GetRunAction()
{
	if (!thePointer) thePointer=new FadsRunAction;
	return thePointer;
}

void FadsRunAction::EndOfRunAction(const G4Run* aRun)
{
	if (applAction) applAction->EndOfRunAction(aRun);
	actionMap::const_iterator it;
	for (it=theEnd.begin();it!=theEnd.end();it++)
		(*it).second->EndOfRunAction(aRun);
}

void FadsRunAction::BeginOfRunAction(const G4Run* aRun)
{
	if (applAction) applAction->BeginOfRunAction(aRun);
	actionMap::const_iterator it;
	for (it=theBegin.begin();it!=theBegin.end();it++)
		(*it).second->BeginOfRunAction(aRun);
}

void FadsRunAction::RegisterAction(UserAction *a,actionType t,int priority)
{
	if (t==BeginOfRun)
		theBegin.insert(std::make_pair(priority,a));
	else if (t==EndOfRun)
		theEnd.insert(std::make_pair(priority,a));
}

}	// end namespace
