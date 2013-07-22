/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsActions/FadsEventAction.h"
#include "FadsActions/UserAction.h"
#include "G4Event.hh"

namespace FADS {

FadsEventAction* FadsEventAction::thePointer=0;

FadsEventAction* FadsEventAction::GetEventAction()
{
	if (!thePointer) thePointer=new FadsEventAction;
	return thePointer;
}

void FadsEventAction::EndOfEventAction(const G4Event* anEvent)
{
	static bool first=true;
	static std::string act;
	if (first)
	{
		first=false;
		char *chtemp=getenv("GOOFY_EOE_MACRO");
		if (chtemp) act=chtemp;
	}
 	if (!act.empty()) UI->ApplyCommand("/macro/execute "+act);
//	std::cout<<"the general purpose endevent action "<<std::endl;
        if (applAction) applAction-> EndOfEventAction(anEvent);
	actionMap::const_iterator it;
	for (it=theEnd.begin();it!=theEnd.end();it++)
		(*it).second->EndOfEventAction(anEvent);
}

void FadsEventAction::BeginOfEventAction(const G4Event* anEvent)
{
	static bool first=true;
	static std::string act;
	if (first)
	{
		first=false;
		char *chtemp=getenv("GOOFY_BOE_MACRO");
		if (chtemp) act=chtemp;
	}
	if (!act.empty()) UI->ApplyCommand("/macro/execute "+act);
//	std::cout<<"the general purpose beginevent action "<<std::endl;
        if(applAction) applAction-> BeginOfEventAction(anEvent);
	actionMap::const_iterator it;
	for (it=theBegin.begin();it!=theBegin.end();it++)
		(*it).second->BeginOfEventAction(anEvent);
}

void FadsEventAction::RegisterAction(UserAction *a,actionType t,int priority)
{
	if (t==BeginOfEvent)
		theBegin.insert(std::make_pair(priority,a));
	else if (t==EndOfEvent)
		theEnd.insert(std::make_pair(priority,a));
}

}	// end namespace
