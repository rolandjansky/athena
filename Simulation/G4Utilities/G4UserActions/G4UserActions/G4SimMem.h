/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4SimMem_H
#define G4SimMem_H

#include "FadsActions/ActionsBase.h"
#include "FadsActions/UserAction.h"
#include <string>

class G4SimMem: public FADS::ActionsBase , public FADS::UserAction {

  private:
   unsigned int nrOfEntries;
   unsigned int runMemory_bor;
   unsigned int runMemory_eor;
   unsigned int event1Memory_eoe;
   unsigned int event2Memory_eoe;
   unsigned int event10Memory_eoe;
   unsigned int eventpreviousMemory_eoe;
   double accumulatedEventMemory;
   double accumulatedIncrMemory;
   double averageMemoryPerEvent();
   double averageMemoryIncreasePerEvent();

  public:
   G4SimMem(std::string s):FADS::ActionsBase(s),FADS::UserAction(s),nrOfEntries(0),runMemory_bor(0),runMemory_eor(0),
                           event1Memory_eoe(0),event2Memory_eoe(0),event10Memory_eoe(0),
                           eventpreviousMemory_eoe(0), accumulatedEventMemory(0), accumulatedIncrMemory(0)
   {;}
   void BeginOfEventAction(const G4Event*);
   void EndOfEventAction(const G4Event*);
   void BeginOfRunAction(const G4Run*);
   void EndOfRunAction(const G4Run*);
   void SteppingAction(const G4Step*);

};


#endif
