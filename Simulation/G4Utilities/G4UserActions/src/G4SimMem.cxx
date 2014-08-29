/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/G4SimMem.h"
#include <iostream>

static G4SimMem ts1("G4SimMem");
void G4SimMem::BeginOfEventAction(const G4Event*)
{
	nrOfEntries++;
}
void G4SimMem::BeginOfRunAction(const G4Run*)
{
        MemorySnooper Memo_bor("begin of run");
	runMemory_bor=Memo_bor.GetMemorySize();
}
void G4SimMem::EndOfEventAction(const G4Event*)
{
	MemorySnooper Memo("end of event");
        if(nrOfEntries>=1)
	accumulatedEventMemory+=Memo.GetMemorySize();
        if(nrOfEntries==1)
	  {
	    event1Memory_eoe=Memo.GetMemorySize();
	    eventpreviousMemory_eoe=runMemory_bor;
	  }
        if(nrOfEntries==2) 
	    event2Memory_eoe=Memo.GetMemorySize();
        if(nrOfEntries>=3)
	    accumulatedIncrMemory+=Memo.GetMemorySize()-eventpreviousMemory_eoe;
        if(nrOfEntries==10)
	    event10Memory_eoe=Memo.GetMemorySize();
	log()<<MSG::INFO<<" MemorySnooper(event,VmMem,diff-previous) kBytes: "<<
		    nrOfEntries <<" "<<
		    Memo.GetMemorySize()<<" "<<
	       	    //runMemory_bor<<" "<<
		    //Memo.GetMemorySize()-runMemory_bor<<" "<<
		    //eventpreviousMemory_eoe<<" "<<
	            int(Memo.GetMemorySize()-eventpreviousMemory_eoe)<<std::endl;
	 //std::cout<<"*****************************************************"<<std::endl;
	 eventpreviousMemory_eoe=Memo.GetMemorySize();
}
void G4SimMem::EndOfRunAction(const G4Run*)
{
	MemorySnooper Memo_eor("end of run");
	runMemory_eor=Memo_eor.GetMemorySize();
	log() << MSG::INFO <<"*****************************************************"<<std::endl;
	log() << MSG::INFO <<"(VmMem) MEMORY SUMMARY: (kBytes)                     "<<std::endl;
	log() << MSG::INFO <<"Memory begin of run             : "<<runMemory_bor<<std::endl;
    if(nrOfEntries>=1)  
	   log() << MSG::INFO <<"Memory 1st event                 : "<<
		       event1Memory_eoe<<std::endl;
    if(nrOfEntries>=2)
	   log() << MSG::INFO <<"Memory 2st event                 : "<<
		       event2Memory_eoe<<std::endl;
    if(nrOfEntries>=10)
	   log() << MSG::INFO <<"Memory 10th event                : "<<
		       event10Memory_eoe<<std::endl;
	log() << MSG::INFO <<"Memory end of run                : "<<
	 	     runMemory_eor<<std::endl;
    if(nrOfEntries>=1)
	   log() << MSG::INFO <<"Memory per event (average)       : "<<
		       int(averageMemoryPerEvent())<<std::endl;
    if(nrOfEntries>=3)
	   log() << MSG::INFO <<"Memory increase/event (nevent>3) : "<<
		       int(averageMemoryIncreasePerEvent())<<std::endl;
	log() << MSG::INFO <<"*****************************************************"<<std::endl;

}
void G4SimMem::SteppingAction(const G4Step*)
{

}
double G4SimMem::averageMemoryPerEvent()
{
	if (nrOfEntries<1) return -1;
	return accumulatedEventMemory/nrOfEntries;
}
double G4SimMem::averageMemoryIncreasePerEvent()
{
        if (nrOfEntries<1) return -1;
	return accumulatedIncrMemory/nrOfEntries;
}
