/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/G4SimTimer.h"
#include <iostream>
#include <iomanip>

static G4SimTimer ts1("G4SimTimer");

void G4SimTimer::BeginOfEventAction(const G4Event*)
{
	nrOfEntries++;
	eventTimer->Start();
}
void G4SimTimer::EndOfEventAction(const G4Event*)
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
void G4SimTimer::BeginOfRunAction(const G4Run*)
{
	runTimer->Start();
}
void G4SimTimer::EndOfRunAction(const G4Run*)
{
	runTimer->Stop();
	runTime=runTimer->GetUserElapsed()+runTimer->GetSystemElapsed();
	ATH_MSG_INFO("*****************************************"<<std::endl<<
	             "**                                     **"<<std::endl<<
			     "    End of run - time spent is "<<std::setprecision(4) << runTime<<std::endl<<
    	         "    Average time per event was "<<std::setprecision(4) << averageTimePerEvent()<<" +- "<< std::setprecision(4) << Sigma()<<std::endl<<
		   		 "**                                     **"<<std::endl<<
		   		 "*****************************************");
}
void G4SimTimer::SteppingAction(const G4Step*)
{

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
