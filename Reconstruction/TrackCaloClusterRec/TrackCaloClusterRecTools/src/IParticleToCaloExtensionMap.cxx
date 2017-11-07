/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackCaloClusterRecTools/IParticleToCaloExtensionMap.h"

IParticleToCaloExtensionMap::IParticleToCaloExtensionMap() {
  m_caloExtensionMap.clear();
}

IParticleToCaloExtensionMap::~IParticleToCaloExtensionMap() {
  m_caloExtensionMap.clear();
}

void IParticleToCaloExtensionMap::addEntry(const xAOD::IParticle& particle, const Trk::CurvilinearParameters& caloEntry) {
  m_caloExtensionMap.insert( std::pair<const xAOD::IParticle*, const Trk::CurvilinearParameters>(&particle, caloEntry) );  
}

const Trk::CurvilinearParameters*  IParticleToCaloExtensionMap::readCaloEntry(const xAOD::IParticle* particle) const
{
  auto pos = m_caloExtensionMap.find(particle);
  if (pos == m_caloExtensionMap.end())
    return 0;
  else
    return &pos->second;
}

unsigned int IParticleToCaloExtensionMap::size() const {
  return m_caloExtensionMap.size();
}