/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InitializeMuonClusters.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Include files

#include "InitializeMuonClusters.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include <math.h>

/////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////

Rec::InitializeMuonClusters::InitializeMuonClusters(const std::string& name, 
						    ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_muonClusterContainerName("MuonClusterContainer")
{
  declareProperty("MuonClusterContainer", m_muonClusterContainerName="MuonClusterCollection");
}

/////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////

Rec::InitializeMuonClusters::~InitializeMuonClusters() {}

/////////////////////////////////////////////////////////////////
// Initialize
/////////////////////////////////////////////////////////////////

StatusCode Rec::InitializeMuonClusters::initialize() {

  ATH_MSG_INFO ("Initialization done in "<<name());
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////
// Finalize
/////////////////////////////////////////////////////////////////

StatusCode Rec::InitializeMuonClusters::finalize() {
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////
// Execute
/////////////////////////////////////////////////////////////////

StatusCode Rec::InitializeMuonClusters::execute() {

  xAOD::CaloClusterContainer * clusterContainer = CaloClusterStoreHelper::makeContainer(&(*evtStore()),
                                                                                        m_muonClusterContainerName,
                                                                                        msg());
  if ( clusterContainer== NULL ) {
    ATH_MSG_WARNING ("Unable to create Muon Cluster Container - bailing out ...");
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}

