/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4FASTSIMULATION_SIMPLEFASTKILLER_H
#define G4FASTSIMULATION_SIMPLEFASTKILLER_H

//Base class header
#include "G4VFastSimulationModel.hh"

//STL headers
#include <string>

class SimpleFastKiller: public G4VFastSimulationModel
{
 public:
  SimpleFastKiller(const std::string& name);
  ~SimpleFastKiller() {}

  //Fast sim methods
  G4bool IsApplicable(const G4ParticleDefinition&) override final { return true; }
  void DoIt(const G4FastTrack&, G4FastStep&) override final;
  G4bool ModelTrigger(const G4FastTrack &) override final { return true; }
};

#endif //G4FASTSIMULATION_SIMPLEFASTKILLER_H
