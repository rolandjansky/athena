/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCSIMHITCOLLECTION_P2_H
#define STGCSIMHITCOLLECTION_P2_H

#include <vector>
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p2.h"

namespace Muon {
  class sTGCSimHitCollection_p2   
  {
  public:
    std::vector<int>      m_sTGCId;
    std::vector<float>    m_globalTime;
    //postStep
    std::vector<float>    m_stX, m_stY, m_stZ;    //!< Represents m_globalPosition in transient class
    std::vector<int>      m_particleEncoding;
    std::vector<float>    m_ptX, m_ptY, m_ptZ;
    std::vector<float>    m_depositEnergy;

    std::vector<HepMcParticleLink_p2>  m_partLink;
  };
}

#endif

