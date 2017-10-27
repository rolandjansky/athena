/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERICMUONSIMHITCOLLECTION_P2_H
#define GENERICMUONSIMHITCOLLECTION_P2_H

#include <vector>
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p2.h"

namespace Muon {
  class GenericMuonSimHitCollection_p2   
  {
  public:
    std::vector<int>      m_GenericId;
    std::vector<float>    m_globalTime;
    std::vector<float>    m_globalpreTime;
    //preStep
    std::vector<float>    m_prestX, m_prestY, m_prestZ;    //!< Represents m_globalPosition in transient class
    std::vector<float>    m_prelocX, m_prelocY, m_prelocZ; //!< Represents m_localPosition in transient class
    //postStep
    std::vector<float>    m_stX, m_stY, m_stZ;    //!< Represents m_globalPosition in transient class
    std::vector<float>    m_locX, m_locY, m_locZ; //!< Represents m_localPosition in transient class
    std::vector<float>    m_kineticEnergy;
    std::vector<int>      m_particleEncoding;
    std::vector<float>    m_ptX, m_ptY, m_ptZ;
    std::vector<float>    m_depositEnergy;
    std::vector<float>    m_StepLength;

    std::vector<HepMcParticleLink_p2>  m_partLink;
  };
}

#endif

