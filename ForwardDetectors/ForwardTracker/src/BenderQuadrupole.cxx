/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/BenderQuadrupole.h"
#include "ForwardTracker/IParticle.h"
#include "ForwardTracker/Point.h"
#include "ForwardTracker/TransversePoint.h"

namespace ForwardTracker {
  
  BenderQuadrupole::BenderQuadrupole(Side side, double length, double strength, double beamEnergy, Bendplane bendplane):
    m_side      (side),
    m_length    (length), 
    m_strength  (strength),
    m_beamEnergy(beamEnergy),
    m_bendplane (bendplane)
  {}
  
  void BenderQuadrupole::bend(IParticle& particle) const {

    // Calculates (x, y) position and direction after deflection (charge dependent).
    // Particle is at magnet front face and is moved to the rear face.
    
    double          aside        = ForwardTrackerConstants::aside[m_side];
    Point&          direction    = particle.direction();
    TransversePoint displacement = particle.displacement();

    if (!particle.charged()) displacement += m_length*aside/direction[2]*direction.transverse(); // Linear drift
    else {

      double exitPos[2];
      double exitDir[2];

      double qk  = std::sqrt(m_beamEnergy*std::fabs(m_strength)/m_length/particle.momentum());
      double qkl = aside*qk*m_length;

      exitPos[ m_bendplane] =     std::cos (qkl)*displacement[ m_bendplane] + std::sin (qkl)*direction[ m_bendplane]/direction[2]/qk;
      exitDir[ m_bendplane] = -qk*std::sin (qkl)*displacement[ m_bendplane] + std::cos (qkl)*direction[ m_bendplane]/direction[2];
	
      exitPos[!m_bendplane] =     std::cosh(qkl)*displacement[!m_bendplane] + std::sinh(qkl)*direction[!m_bendplane]/direction[2]/qk;
      exitDir[!m_bendplane] = +qk*std::sinh(qkl)*displacement[!m_bendplane] + std::cosh(qkl)*direction[!m_bendplane]/direction[2];
      
      displacement[ m_bendplane] = exitPos[ m_bendplane];
      displacement[!m_bendplane] = exitPos[!m_bendplane];

      direction[ m_bendplane] = exitDir[ m_bendplane];
      direction[!m_bendplane] = exitDir[!m_bendplane];

      direction[2] = aside/std::sqrt(1 + direction[0]*direction[0] + direction[1]*direction[1]);
      
      direction[0] *= direction[2];
      direction[1] *= direction[2];
    }

    particle.updatePositionFromDisplacement(displacement);
  }
}
