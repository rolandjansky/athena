/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SimpleFastKiller_h
#define SimpleFastKiller_h

#include "G4String.hh"
#include "G4FastSimulation/FastSimModel.h"

class SimpleFastKiller: public FastSimModel
{
 public:
  SimpleFastKiller(G4String name) : FastSimModel(name) {}
  G4bool IsApplicable(const G4ParticleDefinition&) { return true; }
  void DoIt(const G4FastTrack&, G4FastStep&);
  G4bool ModelTrigger(const G4FastTrack &) { return true; }
};

#endif