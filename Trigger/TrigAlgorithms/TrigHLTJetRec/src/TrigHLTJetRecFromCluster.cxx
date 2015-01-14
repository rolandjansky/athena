/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigHLTJetRec_Cluster: Provide the xAOD::CaloCluster specific method
// needed by TruggerHLTJetRec_Base to for m jets from clusters.

#include "TrigHLTJetRec/TrigHLTJetRecFromCluster.h"

TrigHLTJetRecFromCluster::TrigHLTJetRecFromCluster(const std::string& name, 
                                             ISvcLocator* pSvcLocator):
  TrigHLTJetRecBase<xAOD::CaloClusterContainer>(name, pSvcLocator) {}

TrigHLTJetRecFromCluster::~TrigHLTJetRecFromCluster(){}
