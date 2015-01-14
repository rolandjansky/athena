/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./ClusterToPseudoJet.h"
#include <string>
#include "JetEDM/IndexedConstituentUserInfo.h"
#include "JetEDM/LabelIndex.h"

using jet::LabelIndex;
using jet::IConstituentUserInfo;
using jet::IndexedConstituentUserInfo;
using fastjet::PseudoJet;
using xAOD::CaloCluster;


ClusterToPseudoJet::ClusterToPseudoJet(const LabelIndex* index_map):
  m_indexMap(index_map){
}


PseudoJet ClusterToPseudoJet::operator() (const CaloCluster* cluster){
  
  PseudoJet psj(cluster->p4());
  IConstituentUserInfo* pcui = new IndexedConstituentUserInfo(*cluster,
                                                              1,
                                                              m_indexMap);
  psj.set_user_info(pcui);
  return psj;
}
