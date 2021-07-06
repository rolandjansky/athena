/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonFeatureCnvTestAlg.h"

#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigMuonEvent/MuonFeatureDetailsContainer.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"

#include <vector>
#include <iomanip>

MuonFeatureCnvTestAlg::MuonFeatureCnvTestAlg( const std::string& name, ISvcLocator* svcLoc ) :
  AthAlgorithm(name, svcLoc)
{
  declareProperty( "AODMuonFeatureContainerName", m_aodMuonFeatureContainerName = "HLT" );
  declareProperty( "AODMuonFeatureDetailsContainerName", m_aodMuonFeatureDetailsContainerName = "HLT" );
  declareProperty( "xAODL2StandAloneMuonContainerName", m_xaodContainerName = "L2StandAloneMuon" );
}

StatusCode MuonFeatureCnvTestAlg::initialize() {
    
  ATH_MSG_DEBUG( "AODMuonFeatureContainerName  = " << m_aodMuonFeatureContainerName);
  ATH_MSG_DEBUG( "AODMuonFeatureDetailsContainerName  = " << m_aodMuonFeatureDetailsContainerName);
  ATH_MSG_DEBUG( "xAODL2StandAloneMuonContainerName = " << m_xaodContainerName );
    
    // Return gracefully:
    return StatusCode::SUCCESS;
}

StatusCode MuonFeatureCnvTestAlg::execute() {

  // Retrieve MuonFeatureContainer
  const MuonFeatureContainer* mf = evtStore()->tryConstRetrieve<MuonFeatureContainer>(m_aodMuonFeatureContainerName);
  if (!mf) {
    ATH_MSG_ERROR("No MuonFeatureContainer with key " << m_aodMuonFeatureContainerName << " found.");
    return StatusCode::RECOVERABLE;
  }
  ATH_MSG_DEBUG( "Retrieved " << m_aodMuonFeatureContainerName );
  ATH_MSG_DEBUG( "Size of MuonFeatureContainer = " << mf->size() );
  
  // Retrieve MuonFeatureDetailsContainer
  const MuonFeatureDetailsContainer* mfd = evtStore()->tryConstRetrieve<MuonFeatureDetailsContainer>(m_aodMuonFeatureDetailsContainerName);
  if (!mfd) {
    ATH_MSG_ERROR("No MuonFeatureDetailsContainer with key " << m_aodMuonFeatureDetailsContainerName << " found.");
    return StatusCode::RECOVERABLE;
  }
  ATH_MSG_DEBUG("Retrieved " << m_aodMuonFeatureDetailsContainerName);
  ATH_MSG_DEBUG( "Size of MuonFeatureDetailsContainer = " << mfd->size() );
  
  // Retrieve the xAOD::L2StandAloneMuonContainer
  const xAOD::L2StandAloneMuonContainer* xaod = evtStore()->tryConstRetrieve<xAOD::L2StandAloneMuonContainer>(m_xaodContainerName);
  if(!xaod) {
    ATH_MSG_ERROR("No xAOD::L2StandAloneMuonContainer with key " << m_xaodContainerName << " found.");
    return StatusCode::RECOVERABLE;
  }
  ATH_MSG_DEBUG("Retrieved " << m_xaodContainerName);
  ATH_MSG_DEBUG( "Size of xAOD::L2StandAloneMuonContainer = " << xaod->size() );

  xAOD::L2StandAloneMuonContainer::const_iterator muonSA_it = xaod->begin();
  xAOD::L2StandAloneMuonContainer::const_iterator muonSA_end = xaod->end();
  int counter = 0;
  for (; muonSA_it!= muonSA_end; ++muonSA_it) {
    ATH_MSG_DEBUG( "Check xAOD L2StandAloneMuonContainer pt of " << counter << "-th track is " << (*muonSA_it)->pt() );
    counter++;
  }
		  
  return StatusCode::SUCCESS;
}//execute

