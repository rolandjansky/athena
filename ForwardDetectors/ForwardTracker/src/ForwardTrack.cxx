/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/BeamlineSetup.h"
#include "ForwardTracker/ForwardTrack.h"

#include <iostream>

ForwardTrack::ForwardTrack() {

  m_fBeamline1 = ForwardTracker::Beamline();
  m_fBeamline2 = ForwardTracker::Beamline();
  m_fPosition  = ForwardTracker::Point();
  m_fMomentum  = ForwardTracker::Point();
}

void ForwardTrack::initialize(ForwardTracker::ConfigData cData) {
  
  std::cout << " ForwardTrack::initialize " << std::endl;
  std::cout << " ConfigData "               << cData;
  
  m_fBeamline1 = ForwardTracker::BeamlineSetup(cData, ForwardTracker::beam1); std::cout << m_fBeamline1;
  m_fBeamline2 = ForwardTracker::BeamlineSetup(cData, ForwardTracker::beam2); std::cout << m_fBeamline2;
}

bool ForwardTrack::TrackParticle(ForwardTracker::Particle particle) {
    
  (particle.direction().z()>0.) ? m_fBeamline1.track(particle) : m_fBeamline2.track(particle);

  m_fPosition = particle.positionRelBeam();
  m_fMomentum = particle.momentum()*particle.direction();
  m_fParticle = particle;
  
  if (particle.isOutOfAperture()) return false;
  
  return true;
}
