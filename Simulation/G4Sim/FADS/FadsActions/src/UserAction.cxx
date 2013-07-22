/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsActions/UserAction.h"
#include "FadsActions/ActionsStore.h"
#include "FadsActions/FadsSteppingAction.h"
#include "FadsActions/FadsEventAction.h"
#include "FadsActions/FadsRunAction.h"

#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

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
{
}

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
	else
		std::cout<<"Don't know wat to do for "<<s<<std::endl;
}



void UserAction::SetProperty(std::string propName,std::string propValue){

  theProperties[propName]=propValue;

}

MsgStream UserAction::log()
{
    if (m_log) return *m_log;
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    IMessageSvc* p_msgSvc = 0;
    if (svcLocator->service("MessageSvc", p_msgSvc).isFailure() || !p_msgSvc)
        std::cout << "FadsSensitiveDetector: Trouble getting the message service.  Should never happen.  Will crash now." << std::endl;
    m_log = new MsgStream(p_msgSvc,name);
    return *m_log;
}

}	// end namespace
