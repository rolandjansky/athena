/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMSIMHITCOLLECTION_P2_H
#define MMSIMHITCOLLECTION_P2_H

#include <vector>
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p2.h"

namespace Muon {
  class MMSimHitCollection_p2   
  {
  public:
    std::vector<int>      m_MMId;
    std::vector<float>    m_globalTime;
    //postStep
    std::vector<float>    m_stX, m_stY, m_stZ;    //!< Represents m_globalPosition in transient class
    std::vector<float>    m_kineticEnergy;
    std::vector<int>      m_particleEncoding;
    std::vector<float>    m_ptX, m_ptY, m_ptZ;
    std::vector<float>    m_depositEnergy;

    std::vector<HepMcParticleLink_p2>  m_partLink;
  };
}

#endif

