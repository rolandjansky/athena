/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4SimMem_H
#define G4SimMem_H

#include "G4AtlasTools/UserActionBase.h"


#include <string>

class G4SimMem final: public UserActionBase {

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
 G4SimMem(const std::string& type, const std::string& name, const IInterface* parent):UserActionBase(type,name,parent),nrOfEntries(0),runMemory_bor(0),runMemory_eor(0),
                           event1Memory_eoe(0),event2Memory_eoe(0),event10Memory_eoe(0),
                           eventpreviousMemory_eoe(0), accumulatedEventMemory(0), accumulatedIncrMemory(0)
   {;}
   virtual void BeginOfEvent(const G4Event*) override;
   virtual void EndOfEvent(const G4Event*) override;
   virtual void BeginOfRun(const G4Run*) override;
   virtual void EndOfRun(const G4Run*) override;

   virtual StatusCode queryInterface(const InterfaceID&, void**) override;


};


#endif
