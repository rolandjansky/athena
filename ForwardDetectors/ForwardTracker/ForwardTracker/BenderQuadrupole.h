/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDTRACKER_QUADRUPOLEBENDER_H
#define FORWARDTRACKER_QUADRUPOLEBENDER_H

#include "ForwardTrackerConstants.h"
#include "IBender.h"

namespace ForwardTracker {

  class IParticle;

  class BenderQuadrupole: public IBender {
  
  public:
    
    BenderQuadrupole(Side side, double length, double strength, double beamEnergy, Bendplane bendplane);
    
    void bend(IParticle&) const;

  private:
    
    Side      m_side;
    double    m_length;
    double    m_strength;
    double    m_beamEnergy;
    Bendplane m_bendplane;
  };
}

#endif
