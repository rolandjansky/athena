/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCSIMHITCOLLECTION_P3_H
#define RPCSIMHITCOLLECTION_P3_H

#include <vector>
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p2.h"

namespace Muon {
class RPCSimHitCollection_p3   
{
 public:
  std::vector<int>      m_RPCid;
  std::vector<float>    m_globalTime;
  std::vector<float>    m_stX, m_stY, m_stZ; //!< Represents m_localPosition in transient class
  std::vector<HepMcParticleLink_p2>  m_partLink;
  std::vector<float>    m_stopX, m_stopY, m_stopZ; //!< Represents end-point of the g4 step in the transient class
  std::vector<float>    m_energyDeposit;
  std::vector<float>    m_stepLength;
  std::vector<int>      m_particleEncoding;
  std::vector<float>    m_kineticEnergy;
};
}

#endif

