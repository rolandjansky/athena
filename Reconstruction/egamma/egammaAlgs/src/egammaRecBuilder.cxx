/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "egammaRecBuilder.h"

#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "egammaInterfaces/IEMTrackMatchBuilder.h"
#include "egammaInterfaces/IEMConversionBuilder.h"
#include "egammaInterfaces/IegammaCheckEnergyDepositTool.h"
#include "egammaUtils/egammaDuplicateRemoval.h"
#include "CaloUtils/CaloClusterStoreHelper.h"

// INCLUDE GAUDI HEADER FILES:
#include <algorithm> 
#include <cmath>

using CLHEP::MeV;
using CLHEP::GeV;

namespace{
  class smallChrono{
  public:
    smallChrono(IChronoStatSvc* timingProfile, const std::string& name): 
      m_time(timingProfile),
      m_name(name){
      if(m_time){
	m_time->chronoStart(m_name);
      }
    }
    ~smallChrono(){
      if(m_time){
	m_time->chronoStop(m_name);
      }
    }
  private:
    IChronoStatSvc* m_time;
    const std::string m_name;
  };
}


egammaRecBuilder::egammaRecBuilder(const std::string& name, 
				   ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator),
  m_timingProfile(0)
{
}

// =================================================================
StatusCode egammaRecBuilder::initialize()
{
  // initialize method

  ATH_MSG_DEBUG("Initializing egammaRecBuilder");

  // First the data handle keys
  ATH_CHECK(m_inputTopoClusterContainerKey.initialize());
  ATH_CHECK(m_egammaRecContainerKey.initialize());

  //////////////////////////////////////////////////
  // retrieve track match builder
  CHECK( RetrieveEMTrackMatchBuilder() );
  // retrieve conversion builder
  CHECK(  RetrieveEMConversionBuilder() );

  // retrieve timing profile
  CHECK( service("ChronoStatSvc",m_timingProfile) );

  ATH_MSG_DEBUG("Initialization completed successfully");
  return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode egammaRecBuilder::RetrieveEMTrackMatchBuilder(){
  // retrieve EMTrackMatchBuilder tool
  if (!m_doTrackMatching) {
    m_trackMatchBuilder.disable();
    return StatusCode::SUCCESS;
  }

  if (m_trackMatchBuilder.empty()) {
    ATH_MSG_ERROR("EMTrackMatchBuilder is empty, but track matching is enabled");
    return StatusCode::FAILURE;
  } 
  
  if(m_trackMatchBuilder.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<<m_trackMatchBuilder);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Retrieved Tool "<<m_trackMatchBuilder); 

  return StatusCode::SUCCESS;
}
// ====================================================================
StatusCode egammaRecBuilder::RetrieveEMConversionBuilder(){
  //
  // retrieve EMConversionBuilder tool
  //  
  if (!m_doConversions) {
    m_conversionBuilder.disable();
    return StatusCode::SUCCESS;
  }
  if (m_conversionBuilder.empty()) {
    ATH_MSG_ERROR("EMConversionBuilder is empty");
    return StatusCode::FAILURE;
  } 
  if(m_conversionBuilder.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<<m_conversionBuilder);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Retrieved Tool "<<m_conversionBuilder); 

  return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode egammaRecBuilder::finalize(){
  // finalize method
  return StatusCode::SUCCESS;
}

// ======================================================================
StatusCode egammaRecBuilder::execute(){
  // athena execute method

  ATH_MSG_DEBUG("Executing egammaRecBuilder");
  
  // Chrono name for each Tool
  std::string chronoName;

  SG::ReadHandle<xAOD::CaloClusterContainer> topoclusters(m_inputTopoClusterContainerKey);

  // validity check is only really needed for serial running. Remove when MT is only way.
  if (!topoclusters.isValid()) {
    ATH_MSG_ERROR("Could not retrieve cluster container:" << m_inputTopoClusterContainerKey.key());
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Retrieved input cluster container");
  }

  //Build the initial egamma Rec objects for all copied Topo Clusters
  SG::WriteHandle<EgammaRecContainer> egammaRecs(m_egammaRecContainerKey);
  ATH_CHECK(egammaRecs.record(std::make_unique<EgammaRecContainer>()));
  
  for (size_t i(0); i < topoclusters->size(); i++) {
    const ElementLink< xAOD::CaloClusterContainer > clusterLink( *topoclusters, i );
    const std::vector< ElementLink<xAOD::CaloClusterContainer> > ClusterLink {clusterLink};    
    auto egRec = std::make_unique<egammaRec>();
    egRec->setCaloClusters( ClusterLink );
    egammaRecs->push_back( std::move(egRec) );
  }
  
  ///Append track Matching information
  if (m_doTrackMatching){
    smallChrono timer(m_timingProfile, this->name()+"_"+m_trackMatchBuilder->name()+"_AllClusters");
    for (auto egRec : *egammaRecs) {
      if (m_trackMatchBuilder->executeRec(egRec).isFailure()){
	ATH_MSG_ERROR("Problem executing TrackMatchBuilder");
	return StatusCode::FAILURE;
      }
    }
  }
  //Do the conversion matching
  if (m_doConversions){
    ATH_MSG_DEBUG("Running ConversionBuilder");  
    smallChrono timer(m_timingProfile, this->name()+"_"+m_conversionBuilder->name()+"_AllClusters");
    for (auto egRec : *egammaRecs) {
      if (m_conversionBuilder->executeRec(egRec).isFailure()){
	ATH_MSG_ERROR("Problem executing " << m_conversionBuilder);
	return StatusCode::FAILURE;  
      }
    }
  }
  return StatusCode::SUCCESS;
}
//-----------------------------------------------------------------
