/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetRec/ClusterToPseudoJetConverter.h"

ClusterToPseudoJetConverter::ClusterToPseudoJetConverter(const ClusterToPseudoJet& apj,
                                                         bool uncalibrated):
m_apj(apj), m_uncalibrated(uncalibrated){
}


PseudoJet ClusterToPseudoJetConverter::operator()(ValueType c) const{
  if (m_uncalibrated){
    CaloClusterChangeSignalState stateHelper (c, 
                                              xAOD::CaloCluster::UNCALIBRATED);
    return m_apj(c);
  }
    
  return m_apj(c);
}
