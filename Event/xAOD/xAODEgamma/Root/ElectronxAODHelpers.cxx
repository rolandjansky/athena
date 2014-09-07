/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODEgamma/ElectronxAODHelpers.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

const xAOD::TrackParticle* xAOD::EgammaHelpers::getOriginalTrackParticle(const xAOD::Electron* el){

 if(el){
    const xAOD::TrackParticle* trkPar= el->trackParticle();
    if(trkPar){
      return getOriginalTrackParticleFromGSF(trkPar);
    }
  }
  return 0;
}

const xAOD::TrackParticle* xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(const xAOD::TrackParticle* trkPar){

  if(! trkPar) return 0;
  static SG::AuxElement::Accessor<ElementLink<xAOD::TrackParticleContainer> > orig ("originalTrackParticle");

  if (!orig.isAvailable(*trkPar) || !orig(*trkPar).isValid()) return 0;

  return (*orig(*trkPar));
}

