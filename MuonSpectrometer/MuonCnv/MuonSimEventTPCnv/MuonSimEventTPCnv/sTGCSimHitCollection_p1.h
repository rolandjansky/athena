/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCSIMHITCOLLECTION_P1_H
#define STGCSIMHITCOLLECTION_P1_H

#include <vector>
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p1.h"

namespace Muon {
  class sTGCSimHitCollection_p1   
  {
  public:
    std::vector<int>      m_sTGCId;
    std::vector<float>    m_globalTime;
    //postStep
    std::vector<float>    m_stX, m_stY, m_stZ;    //!< Represents m_globalPosition in transient class
    std::vector<int>      m_particleEncoding;
    std::vector<float>    m_ptX, m_ptY, m_ptZ;
    std::vector<float>    m_depositEnergy;

    std::vector<HepMcParticleLink_p1>  m_partLink;
  };
}

#endif

