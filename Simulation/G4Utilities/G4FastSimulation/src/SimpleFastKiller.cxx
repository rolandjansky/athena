/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4FastSimulation/SimpleFastKiller.h"
#include "G4FastSimulation/FastSimModelProxy.h"
#include "G4FastStep.hh"

static FastSimModelProxy<SimpleFastKiller> plugin("SimpleFastKiller");

void SimpleFastKiller::DoIt(const G4FastTrack&, G4FastStep& fastStep)
{
  fastStep.KillPrimaryTrack();
  fastStep.SetPrimaryTrackPathLength(0.0);
}