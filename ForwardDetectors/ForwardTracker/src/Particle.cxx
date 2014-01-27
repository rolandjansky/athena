/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/Particle.h"
#include "ForwardTracker/ForwardTrackerConstants.h"

#include <sstream>
#include <cmath>
#include <iomanip>
#include <iostream>

namespace ForwardTracker {

  Particle::Particle():
    m_position     (0., 0., 0.), 
    m_momentum     (0), 
    m_charged      (false),
    m_direction    (Point(0., 0., 0.)), 
    m_outOfAperture(false),
    m_debug        (false)
  {}

  Particle::Particle(double x, double y, double z, double px, double py, double pz, bool charged):
    m_position     (x, y, z), 
    m_momentum     (std::sqrt(px*px + py*py + pz*pz)), 
    m_charged      (charged),
    m_direction    (Point(px/m_momentum, py/m_momentum, pz/m_momentum)), 
    m_outOfAperture(m_position.perp2() > ForwardTrackerConstants::beamPipeRadius2 ? true : false),
    m_debug        (false)
  {}
   
  std::string Particle::str() const {
    
    std::ostringstream ost; 
    
    ost << " pos"   << m_position
	<< " dir"   << m_direction
	<< " mom "  << m_momentum
	<< " prb"   << this->positionRelBeam()
	<< " q "    << m_charged
	<< " lost " << m_outOfAperture;

    if (not m_msg.empty()) ost << "\n msg " << m_msg;
    
    return ost.str();
  }
}
