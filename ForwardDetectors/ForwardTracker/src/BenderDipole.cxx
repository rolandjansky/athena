/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/BenderDipole.h"
#include "ForwardTracker/IParticle.h"
#include "ForwardTracker/Point.h"

#include <iostream>

namespace ForwardTracker {

  BenderDipole::BenderDipole(Side side, double length, double strength, double beamEnergy, Bendplane bendplane):
    m_side      (side),
    m_length    (length), 
    m_strength  (strength), 
    m_beamEnergy(beamEnergy),
    m_bendplane (bendplane)
  {}
  
  void BenderDipole::bend(IParticle& particle) const {

    // Calculate (x, y) position and direction after deflection (charge dependent).
    // Particle is at magnet front face and is moved to the rear face.

    // -----------------------------------------------------------------------------------------
    // m_strength = k0 = L/R = L*B*speedoflight/momentum = deflection angle[rad]
    //
    // Separation dipoles bring beams to collision from nominal separation (194mm = 2*0.097m)
    // Main bending dipoles guide beams along the LHC arc (first @ ~270 m)
    // MBXW separation dipoles (D1): 6 normal conducting (3.40m long, from  59.622m to  84.352m)
    // MBRC separation dipoles (D2): 1 super  conducting (9.45m long, from 153.175m to 162.625m)
    // MB   bending dipoles:      1232 super  conducting (14.30m long, first @ ~270m)
    // 
    // Special case: MB dipoles
    // k0 = L/radiusLHC/fLHCfilled    = (14.3 m)/(4300 m)/(0.8*22km/27km) = 0.0033/0.65 = 0.0051
    //    = L*B*speedoflight/momentum = (14.3 m)*(8.33 Tesla)*(3e8 m/s)/(7e12eV) = 0.0051 
    //    = 2*pi/(number of dipoles)  = 6.28/1232 = 0.0051
    // 
    // Position and direction are in the LHC arc frame (relative to on-momentum particles)
    // -> for on-momentum particles: direction=(0,0,0) and position=(0.097m,0,0) for z>155m
    // -----------------------------------------------------------------------------------------

    double          aside        = ForwardTrackerConstants::aside[m_side];
    Point&          direction    = particle.direction();
    TransversePoint displacement = particle.displacement();

    displacement += m_length*aside/direction[2]*direction.transverse(); // Linear drift
    
    if (particle.charged()) { // Add effect of bending
    
      double angleB_on  = -m_strength;                                // Deflection of  on-momentum particles (strength<0 => bend in)
      double angleB_off = angleB_on*m_beamEnergy/particle.momentum(); // Deflection of off-momentum particles (momentum rescaling)
      bool   isMBdipole = (m_length > 10.);                           // MB dipoles are identified by their length (14.3 m)
      
      displacement[m_bendplane] -= m_length*tan(aside*angleB_off/2);
      displacement[m_bendplane] += m_length*tan(aside*angleB_on/2)*isMBdipole;
            
      direction.rotat(angleB_off - angleB_on*isMBdipole, m_bendplane);
    }
    
    particle.updatePositionFromDisplacement(displacement);
  }
}
