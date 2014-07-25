/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrigParticleTable.h, (c) ATLAS Detector software
// LVL2 adaptation of the offline code ParticleHypothesis.h 
// by A.Salzburger
///////////////////////////////////////////////////////////////////

#ifndef __TRIG_PARTICLE_TABLE__
#define __TRIG_PARTICLE_TABLE__

#include <vector>
#include "CLHEP/Units/SystemOfUnits.h"

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
    std::vector<double> mass;   
    TrigParticleMasses()
    {
      mass.push_back(0.5109989*CLHEP::MeV); // electron mass
      mass.push_back(105.65837*CLHEP::MeV); // muon mass
      mass.push_back(139.57019*CLHEP::MeV); // charged pion mass
      mass.push_back(493.67700*CLHEP::MeV); // charged kaon mass
      mass.push_back(938.27203*CLHEP::MeV); // proton mass
      mass.push_back(0.*CLHEP::MeV); // photon mass
    }    
  };
}

#endif

