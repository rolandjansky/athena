/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTSIMHITCOLLECTION_P1_H
#define MDTSIMHITCOLLECTION_P1_H

#include <vector>
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p1.h"

namespace Muon {
class MDTSimHitCollection_p1   
{
 public:
  std::vector<int>      m_MDTid;
  std::vector<float>    m_globalTime;
  std::vector<float>    m_driftRadius;
  std::vector<float>    m_stX, m_stY, m_stZ; //!< Represents m_localPosition in transient class
  //std::vector<float>    m_meanTime;
  std::vector<HepMcParticleLink_p1>  m_partLink;
};
}

#endif

