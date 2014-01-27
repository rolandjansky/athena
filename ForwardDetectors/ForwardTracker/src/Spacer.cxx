/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/Spacer.h"
#include "ForwardTracker/Particle.h"

#include <sstream>

namespace ForwardTracker {

  Spacer::Spacer(double front, double rear, Side side):
    m_label    ("Spacer"), 
    m_side     (side),
    m_frontFace(front), 
    m_rearFace (rear),
    m_position (0., 0., m_rearFace)
  {} 

  Spacer::Spacer(const IBeamElement::ConstPtr_t& previous, const IBeamElement::ConstPtr_t& current):
    m_label    ("Spacer"), 
    m_side     (current->side()),
    m_frontFace(previous->rearFace()),
    m_rearFace (current->frontFace()), 
    m_position (previous->position().x(), previous->position().y(), m_rearFace)
  {}
  
  void Spacer::track(IParticle& particle) const {

    particle.setBeamCoordinateShift(this);
 
    if (particle.isOutOfAperture()) return;

    double          aside        = ForwardTrackerConstants::aside[m_side];
    Point           direction    = particle.direction();
    TransversePoint displacement = particle.displacement();

    displacement += std::fabs(m_rearFace - particle.z())*aside/direction[2]*direction.transverse(); // Linear drift

    particle.updatePositionFromDisplacement(displacement);
    particle.z() = m_rearFace;

    if (particle.displacement().mag2() > ForwardTrackerConstants::beamPipeRadius2) particle.setOutOfAperture(true);
  }
  
  std::string Spacer::str() const {

    std::ostringstream ost;

    ost << "m_label     " << m_label     << "\n"
	<< "m_side      " << m_side      << "\n"
	<< "m_frontFace " << m_frontFace << "\n"
	<< "m_rearFace  " << m_rearFace  << "\n"
	<< "m_position  " << m_position  << "\n";
    
    return ost.str();
  }
}

