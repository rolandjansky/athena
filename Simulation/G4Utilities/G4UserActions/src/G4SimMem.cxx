/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/G4SimMem.h"
#include "SimHelpers/MemorySnooper.h"
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
	ATH_MSG_INFO(" MemorySnooper(event,VmMem,diff-previous) kBytes: "<<
		    nrOfEntries <<" "<<
		    Memo.GetMemorySize()<<" "<<
	       	    //runMemory_bor<<" "<<
		    //Memo.GetMemorySize()-runMemory_bor<<" "<<
		    //eventpreviousMemory_eoe<<" "<<
	            int(Memo.GetMemorySize()-eventpreviousMemory_eoe));
	 //std::cout<<"*****************************************************"<<std::endl;
	 eventpreviousMemory_eoe=Memo.GetMemorySize();
}
void G4SimMem::EndOfRunAction(const G4Run*)
{
	MemorySnooper Memo_eor("end of run");
	runMemory_eor=Memo_eor.GetMemorySize();
	ATH_MSG_INFO("*****************************************************");
	ATH_MSG_INFO("(VmMem) MEMORY SUMMARY: (kBytes)                     ");
	ATH_MSG_INFO("Memory begin of run             : "<<runMemory_bor);
    if(nrOfEntries>=1)  
	   ATH_MSG_INFO("Memory 1st event                 : "<<
		       event1Memory_eoe);
    if(nrOfEntries>=2)
	   ATH_MSG_INFO("Memory 2st event                 : "<<
		       event2Memory_eoe);
    if(nrOfEntries>=10)
	   ATH_MSG_INFO("Memory 10th event                : "<<
		       event10Memory_eoe);
	ATH_MSG_INFO("Memory end of run                : "<<
	 	     runMemory_eor);
    if(nrOfEntries>=1)
	   ATH_MSG_INFO("Memory per event (average)       : "<<
		       int(averageMemoryPerEvent()));
    if(nrOfEntries>=3)
	   ATH_MSG_INFO("Memory increase/event (nevent>3) : "<<
		       int(averageMemoryIncreasePerEvent()));
	ATH_MSG_INFO("*****************************************************");

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
