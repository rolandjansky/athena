/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDTRACKER_DIPOLEBENDER_H
#define FORWARDTRACKER_DIPOLEBENDER_H

#include "ForwardTrackerConstants.h"
#include "IBender.h"

namespace ForwardTracker {

  class IParticle; 

  class BenderDipole: public IBender {

  public:
    
    BenderDipole(Side side, double length, double strength, double beamEnergy, Bendplane bendplane);
    
    IBender::ConstPtr_t clone() const { IBender::ConstPtr_t ib(new BenderDipole(*this)); return ib; }
    
    void bend(IParticle&) const;
    
  private:
    
    Side        m_side; 
    double      m_length;
    double      m_strength;
    double      m_beamEnergy;
    Bendplane   m_bendplane;
  };
}

#endif
