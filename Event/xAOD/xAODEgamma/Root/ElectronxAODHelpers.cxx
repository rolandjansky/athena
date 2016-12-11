/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODEgamma/ElectronxAODHelpers.h"
#include "xAODEgamma/Electron.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

// ==================================================================
const xAOD::TrackParticle* xAOD::EgammaHelpers::getOriginalTrackParticle(const xAOD::Electron* el){
  
  if(el){
    const xAOD::TrackParticle* trkPar= el->trackParticle();
    if(trkPar){
      return getOriginalTrackParticleFromGSF(trkPar);
    }
  }
  return 0;
}
// ==================================================================
const xAOD::TrackParticle* xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(const xAOD::TrackParticle* trkPar){
  
  if(! trkPar) {return 0;}
  static const SG::AuxElement::Accessor<ElementLink<xAOD::TrackParticleContainer> > orig ("originalTrackParticle");
  if (!orig.isAvailable(*trkPar) || !orig(*trkPar).isValid()) {return 0;}

  return (*orig(*trkPar));
}
// ==================================================================
float xAOD::EgammaHelpers::getLastMeasurementQoverP(const xAOD::TrackParticle *tp){
  static const SG::AuxElement::Accessor<float > QoverPLM  ("QoverPLM");
  if(tp && QoverPLM.isAvailable(*tp)){
    return QoverPLM(*tp);
  }
  return -999 ; 
}
// ==================================================================
const std::set<const xAOD::TrackParticle*> xAOD::EgammaHelpers::getTrackParticles(const xAOD::Electron* el, 
										  bool useBremAssoc /* = true */, 
										  bool allParticles /* = true */){
  std::set<const xAOD::TrackParticle*> tps;
  for (unsigned int i = 0; i < el->nTrackParticles(); ++i){
    const xAOD::TrackParticle* tp = el->trackParticle(i);
    if (useBremAssoc) tp = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(tp);
    if (tp) tps.insert( tp );
    if (!allParticles) break; // break after first particle
  }
  return tps;
}
// ==================================================================
const std::vector<const xAOD::TrackParticle*> xAOD::EgammaHelpers::getTrackParticlesVec(const xAOD::Electron* el, 
											bool useBremAssoc /* = true */, 
											bool allParticles /* = true */){

  std::vector<const xAOD::TrackParticle*> tps;
  for (unsigned int i = 0; i < el->nTrackParticles(); ++i){
    const xAOD::TrackParticle* tp = el->trackParticle(i);
    if (useBremAssoc) {tp = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(tp);}
    if (tp) {tps.push_back( tp );}
    if (!allParticles) {break;} // break after first particle
  }
  return tps;
}
// ==================================================================
std::size_t xAOD::EgammaHelpers::numberOfSiHits(const xAOD::TrackParticle *tp){
  if (!tp) return 0;
  uint8_t dummy(0), nSiHits(0);
  if (tp->summaryValue(dummy, xAOD::numberOfPixelHits)){
    nSiHits += dummy;
  }
  if (tp->summaryValue(dummy, xAOD::numberOfSCTHits)){
    nSiHits += dummy;
  }
  return nSiHits;
}  




