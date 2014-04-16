/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FinalizeMuonClusters.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Include files

#include "FinalizeMuonClusters.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "CaloUtils/CaloClusterStoreHelper.h"

/////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////

Rec::FinalizeMuonClusters::FinalizeMuonClusters(const std::string& name, 
						ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_muonClusterContainerName("MuonClusterCollection")
{
  declareProperty("MuonClusterContainer", m_muonClusterContainerName);
}

/////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////

Rec::FinalizeMuonClusters::~FinalizeMuonClusters() {}

/////////////////////////////////////////////////////////////////
// Initialize
/////////////////////////////////////////////////////////////////

StatusCode Rec::FinalizeMuonClusters::initialize() {

  ATH_MSG_INFO ("Initialize done in "<<name());
  return StatusCode::SUCCESS;

}

/////////////////////////////////////////////////////////////////
// Finalize
/////////////////////////////////////////////////////////////////

StatusCode Rec::FinalizeMuonClusters::finalize() {
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////
// Execute
/////////////////////////////////////////////////////////////////

StatusCode Rec::FinalizeMuonClusters::execute() {

  xAOD::CaloClusterContainer* clusterContainer = 0;
  if (evtStore()->retrieve(clusterContainer, m_muonClusterContainerName).isFailure() || clusterContainer == NULL) {
    ATH_MSG_WARNING ("Cannot retrieve CaloClusterContainer " << m_muonClusterContainerName);
    return StatusCode::SUCCESS;
  }  
  if (CaloClusterStoreHelper::finalizeClusters(&(*evtStore()), 
					        clusterContainer,
					        m_muonClusterContainerName,
					        msg()).isFailure() ) ATH_MSG_WARNING("Muon Cluster Finalization failed inside Calo helper!");
  return StatusCode::SUCCESS;  
}

