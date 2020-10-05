/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrigParticleTable.h, (c) ATLAS Detector software
// LVL2 adaptation of the offline code ParticleHypothesis.h 
// by A.Salzburger
///////////////////////////////////////////////////////////////////

#ifndef __TRIG_PARTICLE_TABLE__
#define __TRIG_PARTICLE_TABLE__

#include <array>
#include "GaudiKernel/SystemOfUnits.h"

namespace TrigVtx
{
  enum TrigParticleName 
    { 
      electron=0,			 
      muon=1,
      pion=2,
      kaon=3,
      proton=4,
      gamma=5
    };
  
  struct TrigParticleMasses 
  {
    constexpr TrigParticleMasses() = default;
    std::array<double,6> mass{
      0.5109989*Gaudi::Units::MeV, // electron mass
      105.65837*Gaudi::Units::MeV, // muon mass
      139.57019*Gaudi::Units::MeV, // charged pion mass
      493.67700*Gaudi::Units::MeV, // charged kaon mass
      938.27203*Gaudi::Units::MeV, // proton mass
      0                            // photon mass
    };
  };
}

#endif

