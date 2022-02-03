/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCSIMHITCOLLECTION_P3_H
#define STGCSIMHITCOLLECTION_P3_H

#include <vector>
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p2.h"

namespace Muon {
  class sTGCSimHitCollection_p3   
  {
  public:
    std::vector<int>      m_sTGCId;
    std::vector<float>    m_globalTime;
    std::vector<float>    m_stX, m_stY, m_stZ;    //!< Represents m_globalPosition in transient class
    std::vector<int>      m_particleEncoding;
    std::vector<float>    m_depositEnergy;
    std::vector<HepMcParticleLink_p2>  m_partLink;
    std::vector<float>    m_kineticEnergy;
    std::vector<float>    m_prestX, m_prestY, m_prestZ;
  };
}

#endif

