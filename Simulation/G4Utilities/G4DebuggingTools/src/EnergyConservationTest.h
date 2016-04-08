/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_EnergyConservationTest_H
#define G4DEBUGGINGTOOLS_EnergyConservationTest_H

#include "G4AtlasTools/UserActionBase.h"

#include <string>

class EnergyConservationTest final: public UserActionBase{

  private:
   double e_in , e_out , e_dep;

  public:
   EnergyConservationTest(const std::string& type, const std::string& name, const IInterface* parent):
     UserActionBase(type,name,parent),e_in(0),e_out(0),e_dep(0) {};

   virtual void EndOfEvent(const G4Event*) override;
   virtual void Step(const G4Step*) override;
   virtual void PreTracking(const G4Track*) override;
   virtual void PostTracking(const G4Track*) override;

   virtual StatusCode queryInterface(const InterfaceID&, void**) override;


};

#endif // G4DEBUGGINGTOOLS_EnergyConservationTest_H
