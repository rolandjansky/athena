/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMatchingTool/TypeMap.h"

TypeMap::TypeMap(){
  m_typemap[xAOD::Type::Muon] = std::make_pair(1178459224, "xAOD::MuonContainer" ); 
  m_typemap[xAOD::Type::Electron] = std::make_pair(1087532415, "xAOD::ElectronContainer"); 
  m_typemap[xAOD::Type::CaloCluster] = std::make_pair(1219821989, "xAOD::CaloClusterContainer"); 
  m_typemap[xAOD::Type::Photon] = std::make_pair(1105575213, "xAOD::PhotonContainer");
  m_typemap[xAOD::Type::Tau] = std::make_pair(1177172564, "xAOD::TauJetContainer");
  m_typemap[xAOD::Type::Jet] = std::make_pair(1244316195, "xAOD::JetContainer");
}

bool TypeMap::isKnown(const xAOD::Type::ObjectType& recoType) const {
  auto it = m_typemap.find(recoType);
  return (it!=m_typemap.end());
}

TypeMap::clid_string_t TypeMap::get(const xAOD::Type::ObjectType& recoType) const {
  auto it = m_typemap.find(recoType);
  if(it!=m_typemap.end()){
    return it->second;
  }
  return std::make_pair(0,"");
}
