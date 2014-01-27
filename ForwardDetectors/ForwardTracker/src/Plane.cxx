/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/Plane.h"
#include "ForwardTracker/Particle.h"
#include "ForwardTracker/ForwardTrackerConstants.h"
#include "ForwardTracker/TransverseShift.h"

namespace ForwardTracker {

  Plane::Plane(double z, Side side):
    m_label    ("Plane"),
    m_side     (side),
    m_frontFace(z*ForwardTrackerConstants::aside[side]),
    m_rearFace (z*ForwardTrackerConstants::aside[side]),
    m_position (TransverseShift(z).x(), 
		TransverseShift(z).y(), 
		z*ForwardTrackerConstants::aside[side]) 
  {}
  
  void Plane::track(IParticle& particle) const {

    particle.setBeamCoordinateShift(this);
    
    if (particle.isOutOfAperture()) return;
 
    if (particle.displacement().mag2() > ForwardTrackerConstants::beamPipeRadius2) particle.setOutOfAperture(true);
  } 
  
  std::string Plane::str() const {
    
    std::ostringstream ost;

    ost << "m_label     " << m_label     << "\n"
	<< "m_side      " << m_side      << "\n"
	<< "m_frontFace " << m_frontFace << "\n"
	<< "m_rearFace  " << m_rearFace  << "\n"
	<< "m_position  " << m_position  << "\n";
    
    return ost.str();
  }
}
