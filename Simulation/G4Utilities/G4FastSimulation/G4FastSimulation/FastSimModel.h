/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FastSimModel_H
#define FastSimModel_H

// The main reason for introducing this class is to disable 
// the constructor:
// G4VFastSimulationModel(const G4String&, G4Envelope*,IsUnique)

#include "G4VFastSimulationModel.hh"
#include "globals.hh"

class FastSimModel: public G4VFastSimulationModel
{
 public:
  FastSimModel(const G4String& aName): G4VFastSimulationModel(aName){}
  virtual ~FastSimModel(){}

  // dummy implementations of all virtual methods
  virtual G4bool IsApplicable(const G4ParticleDefinition&){return false;}
  virtual G4bool ModelTrigger(const G4FastTrack &){return false;}
  virtual void DoIt(const G4FastTrack&, G4FastStep&){}
  virtual G4bool AtRestModelTrigger(const G4FastTrack&){return false;}
  virtual void   AtRestDoIt  (const G4FastTrack&, G4FastStep&){}
};

#endif
