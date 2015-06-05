/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSIMHITCOLLECTION_P1_H
#define TGCSIMHITCOLLECTION_P1_H

#include <vector>
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p1.h"

namespace Muon {
class TGCSimHitCollection_p1   
{
 public:
  std::vector<int>      m_TGCid;
  std::vector<float>    m_globalTime;
  std::vector<float>    m_stX, m_stY, m_stZ; //!< Represents m_localPosition in transient class
  std::vector<float>    m_enX, m_enY, m_enZ; //!< Represents m_localDireCos in transient class
  std::vector<HepMcParticleLink_p1>  m_partLink;
};
}

#endif

