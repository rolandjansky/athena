/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParticleExtrapolationMap.cxx  -  (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "Particle/ParticleExtrapolationMap.h"
#include "GaudiKernel/MsgStream.h"
#include <sstream>

// =====================================================================
Rec::ParticleExtrapolationMap::~ParticleExtrapolationMap() {
 
  if (m_particleMap.size() > 0) {

    ParticleMapType::iterator it    = m_particleMap.begin();
    for (;it != m_particleMap.end(); it++){
      ParticleExtrapolationVector::const_iterator itvec    = it->second->begin();
      ParticleExtrapolationVector::const_iterator itvecend = it->second->end();
      for (;itvec < itvecend; itvec++) delete ((*itvec).first);
      delete it->second;
    }
    // by construction m_trackMap would contain the same pointers, don't delete twice
    
  } else {

    // EXCEPT in HLT mode when there are no particles but only tracks: delete using track map
    TrackMapType::iterator it    = m_trackMap.begin();
    for (;it != m_trackMap.end(); it++){
      ParticleExtrapolationVector::const_iterator itvec    = it->second->begin();
      ParticleExtrapolationVector::const_iterator itvecend = it->second->end();
      for (;itvec < itvecend; itvec++) delete ((*itvec).first);
      delete it->second;
    }
  }
}

// =====================================================================
void Rec::ParticleExtrapolationMap::addExtrapolation(const Rec::TrackParticle& particle,
                                                     const ParticleExtrapolationVector* parameters)
{

  if (m_particleMap.size()==0 && m_trackMap.size() > 0) {
    throw GaudiException ("inconsistent input data: do not mix particles into track map",
			  "ParticleExtrapolationMap",StatusCode::FAILURE);
    return;
  }

  ParticleMapType::const_iterator chosenParticle = m_particleMap.find(&particle);
  if (chosenParticle != m_particleMap.end()) {
    throw GaudiException ("duplicate input data: do not add particles twice, will leak pointer",
			  "ParticleExtrpolationMap",StatusCode::RECOVERABLE);
    return;
  }

  m_particleMap.insert( std::make_pair(&particle, parameters) );
  const Trk::Track* track = particle.originalTrack();
  if (track) m_trackMap.insert ( std::make_pair(track, parameters) );
}

// ======================================================================
void Rec::ParticleExtrapolationMap::addExtrapolation(const Trk::Track& track,
                                                     const ParticleExtrapolationVector* parameters)
  throw (GaudiException) {

  if (m_trackMap.size()==0 && m_particleMap.size() > 0) {
    throw GaudiException ("inconsistent input data: do not mix tracks into particle map",
			  "ParticleExtrapolationMap",StatusCode::FAILURE);
    return;
  }

  TrackMapType::const_iterator chosenParticle = m_trackMap.find(&track);
  if (chosenParticle != m_trackMap.end()) {
    throw GaudiException ("duplicate input data: do not add particles twice, will leak pointer",
			  "ParticleExtrpolationMap",StatusCode::RECOVERABLE);
    return;
  }

  m_trackMap.insert ( std::make_pair(&track, parameters) ); // HLT mode: no particles available
}

// =====================================================================
const Trk::TrackParameters* Rec::ParticleExtrapolationMap::getParameters
(const Rec::TrackParticle& particle, RecIntersect::LayerType there) const {

  ParticleMapType::const_iterator chosenParticle = m_particleMap.find(&particle);
  if (chosenParticle != m_particleMap.end()) {
    ParticleExtrapolationVector::const_iterator itvec    = chosenParticle->second->begin();
    ParticleExtrapolationVector::const_iterator itvecend = chosenParticle->second->end();
    for (;itvec < itvecend; itvec++) {
      if ((*itvec).second == there) return (*itvec).first;
    }
  }
  // std::cout << "ParticleExtrapolationMap: not found" << std::endl;
  return 0;
}

// =====================================================================
const Trk::TrackParameters* Rec::ParticleExtrapolationMap::getParameters
(const Trk::Track& track, RecIntersect::LayerType there) const {

  TrackMapType::const_iterator chosenParticle = m_trackMap.find(&track);
  if (chosenParticle != m_trackMap.end()) {
    ParticleExtrapolationVector::const_iterator itvec    = chosenParticle->second->begin();
    ParticleExtrapolationVector::const_iterator itvecend = chosenParticle->second->end();
    for (;itvec < itvecend; itvec++) {
      if ((*itvec).second == there) return (*itvec).first;
    }
  }
  // std::cout << "ParticleExtrapolationMap: not found" << std::endl;
  return 0;
}
