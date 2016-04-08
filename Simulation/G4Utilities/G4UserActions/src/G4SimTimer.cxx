/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/G4SimTimer.h"
#include <iostream>
#include <iomanip>


void G4SimTimer::BeginOfEvent(const G4Event*)
{
	nrOfEntries++;
	eventTimer->Start();
}
void G4SimTimer::EndOfEvent(const G4Event*)
{
	eventTimer->Stop();
	eventTime=eventTimer->GetUserElapsed()+eventTimer->GetSystemElapsed();
	if(nrOfEntries>1){ // discard first event
	  accumulatedEventTime+=eventTime;
	  accumulatedEventTimeSquared+=eventTime*eventTime;
	}
	ATH_MSG_INFO("\t Event nr. "<<nrOfEntries<<" took " << std::setprecision(4) << eventTime << " s. New average " << 
               std::setprecision(4) << averageTimePerEvent()<<" +- "<<std::setprecision(4) << Sigma());
}
void G4SimTimer::BeginOfRun(const G4Run*)
{
	runTimer->Start();
}
void G4SimTimer::EndOfRun(const G4Run*)
{
	runTimer->Stop();
	runTime=runTimer->GetUserElapsed()+runTimer->GetSystemElapsed();
	ATH_MSG_INFO("*****************************************");
	ATH_MSG_INFO("**                                     **");
	ATH_MSG_INFO("    End of run - time spent is "<<std::setprecision(4) << runTime);
	ATH_MSG_INFO("    Average time per event was "<<std::setprecision(4) << averageTimePerEvent()<<" +- "<< std::setprecision(4) << Sigma());
	ATH_MSG_INFO("**                                     **");
	ATH_MSG_INFO("*****************************************");
}

double G4SimTimer::averageTimePerEvent()
{
	if (nrOfEntries<2) return -1;
	return accumulatedEventTime/(nrOfEntries-1);
}

double G4SimTimer::Sigma()
{
	if (nrOfEntries<3) return -1;
	return sqrt((accumulatedEventTimeSquared/(nrOfEntries-1)-averageTimePerEvent()*averageTimePerEvent())/(nrOfEntries-2));
}

StatusCode G4SimTimer::initialize()
{
	return StatusCode::SUCCESS;
}


StatusCode G4SimTimer::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IUserAction::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IUserAction*>(this);
    addRef();
  } else {
    // Interface is not directly available : try out a base class
    return UserActionBase::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}
