/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header include
#include "SimpleFastKiller.h"
// G4 includes
#include "G4FastStep.hh"

SimpleFastKiller::SimpleFastKiller(const std::string& name)
  : G4VFastSimulationModel(name)
{
}

void SimpleFastKiller::DoIt(const G4FastTrack&, G4FastStep& fastStep)
{
  fastStep.KillPrimaryTrack();
  fastStep.SetPrimaryTrackPathLength(0.0);
}
