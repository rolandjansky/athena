/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCSIMHITCOLLECTION_P1_H
#define RPCSIMHITCOLLECTION_P1_H

#include <vector>
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p1.h"

namespace Muon {
class RPCSimHitCollection_p1   
{
 public:
  std::vector<int>      m_RPCid;
  std::vector<float>    m_globalTime;
  std::vector<float>    m_stX, m_stY, m_stZ; //!< Represents m_localPosition in transient class
  std::vector<HepMcParticleLink_p1>  m_partLink;
};
}

#endif

