/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsActions/FadsSteppingAction.h"
#include "FadsActions/UserAction.h"
#include "G4Step.hh"

namespace FADS {

FadsSteppingAction* FadsSteppingAction::thePointer=0;

FadsSteppingAction* FadsSteppingAction::GetSteppingAction()
{
	if (!thePointer) thePointer=new FadsSteppingAction;
	return thePointer;
}

void FadsSteppingAction::UserSteppingAction(const G4Step* aStep)
{
	static bool first=true;
	static std::string act;
	if (first)
	{
		first=false;
		char *chtemp=getenv("GOOFY_STEP_MACRO");
		if (chtemp) act=chtemp;
	}
	if (!act.empty()) UI->ApplyCommand("/macro/execute "+act);
//	std::cout<<"the general purpose stepping action "<<std::endl;
        if(mctAction) {
	  mctAction-> SetSteppingManagerPointer(fpSteppingManager);
	  mctAction-> UserSteppingAction(aStep);
	}
	actionMap::const_iterator it;
	for (it=theMap.begin();it!=theMap.end();it++)
		(*it).second->SteppingAction(aStep);
}

void FadsSteppingAction::RegisterAction(UserAction *a,int priority)
{
	theMap.insert(std::make_pair(priority,a));
}

}	// end namespace
