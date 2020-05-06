/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCSIMHITCOLLECTION_P3_H
#define STGCSIMHITCOLLECTION_P3_H

#include <vector>
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p3.h"

namespace Muon {
  class sTGCSimHitCollection_p3   
  {
  public:
    std::vector<int>      m_sTGCId;
    std::vector<float>    m_globalTime;
    //postStep
    std::vector<float>    m_stX, m_stY, m_stZ;    //!< Represents m_globalPosition in transient class
    std::vector<int>      m_particleEncoding;
    std::vector<float>    m_ptX, m_ptY, m_ptZ;
    std::vector<float>    m_depositEnergy;

    std::vector<HepMcParticleLink_p3>  m_partLink;
  };
}

#endif

