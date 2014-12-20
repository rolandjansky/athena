/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEFCaloIsolation.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

#include "CaloEvent/CaloCellContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "RecoToolInterfaces/ICaloCellIsolationTool.h"
#include "RecoToolInterfaces/IsolationCommon.h"

#include "TLorentzVector.h"
#include <iostream>
#include <deque>

// ------------------------------------------------------------------------------------------------------
/**
 * Standard constructor for the calo isolation algo.
 */
TrigMuonEFCaloIsolation::TrigMuonEFCaloIsolation(const std::string &name, ISvcLocator *pSvcLocator) :
    FexAlgo(name, pSvcLocator),
    m_requireCombined(false),
    m_debug(false),
    m_caloIsolationTool("xAOD__CaloIsolationTool"),
    m_etiso_cone1(),
    m_etiso_cone2(),
    m_etiso_cone3(),
    m_etiso_cone4()
{
  declareProperty("RequireCombinedMuon",      m_requireCombined);
  declareProperty("CaloIsolationTool",        m_caloIsolationTool);
  declareProperty("HistoPathBase",            m_histo_path_base = "/EXPERT/");
    
  ///////// Monitoring Variables
  declareMonitoredStdContainer("EF_etiso_cone1",      m_etiso_cone1, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_etiso_cone2",      m_etiso_cone2, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_etiso_cone3",      m_etiso_cone3, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_etiso_cone4",      m_etiso_cone4, IMonitoredAlgo::AutoClear);
}

// ------------------------------------------------------------------------------------------------------
/**
 * Destructor.
 */
TrigMuonEFCaloIsolation::~TrigMuonEFCaloIsolation() {

}

// ------------------------------------------------------------------------------------------------------
/**
 * Initialize the algorithm.
 */
HLT::ErrorCode TrigMuonEFCaloIsolation::hltInitialize() {

    m_debug   = msgLvl() <= MSG::DEBUG;

    ATH_MSG_DEBUG("Initializing TrigMuonEFCaloIsolation[" << name() << "]");
    ATH_MSG_DEBUG("package version = " << PACKAGE_VERSION);
    ATH_MSG_DEBUG("Properties set as follows: ");
    ATH_MSG_DEBUG("RequireCombinedMuon:       " << m_requireCombined);



    if (m_caloIsolationTool.retrieve().isSuccess()) {
      ATH_MSG_DEBUG("Retrieved " << m_caloIsolationTool);
    } else {
      ATH_MSG_FATAL("Could not retrieve " << m_caloIsolationTool);
      return HLT::BAD_JOB_SETUP;
    }

    ATH_MSG_DEBUG("End of init TrigMuonEFCaloIsolation");

    return HLT::OK;
}



// ------------------------------------------------------------------------------------------------------
/**
 * Fill the et-cone calorimeter isolation values for a container of xAOD muons.
 */
void TrigMuonEFCaloIsolation::fillCaloIsolation(const xAOD::MuonContainer* muons) {

  if (m_caloIsolationTool.empty()){
    ATH_MSG_WARNING("No calorimeter isolation tool available." );
    return;
  }

  std::vector<xAOD::Iso::IsolationType> etCones = { xAOD::Iso::etcone40,
						    xAOD::Iso::etcone30,
						    xAOD::Iso::etcone20};

  for(auto muon : *muons) {
    
    if(m_debug) {
      ATH_MSG_DEBUG("Processing next EF muon " << muon);
    }
    
    const xAOD::Muon::MuonType muontype = muon->muonType();
    if( muontype == xAOD::Muon::MuonType::Combined || muontype == xAOD::Muon::MuonType::SegmentTagged ) {
      if(m_debug) {
	ATH_MSG_DEBUG("EF muon has combined or segment tagged muon");
      }
    } else {
      if(m_requireCombined) {
	if(m_debug) {
	  ATH_MSG_DEBUG("Not a combined or segment tagged muon & requireCombined=true, so ignore this muon");
	}
	continue;
      }//requireCombined
    }//no combined muon

    const xAOD::TrackParticle* tp = 0;
    if(muon->inDetTrackParticleLink().isValid()) tp = *(muon->inDetTrackParticleLink());
    if( !tp ) tp = muon->primaryTrackParticle();
    if( !tp ) {
      ATH_MSG_WARNING("No TrackParticle found for muon." );
      continue;
    }

    xAOD::CaloIsolation  caloIsolation;
    xAOD::CaloCorrection corrlist;
    corrlist.calobitset.set(static_cast<unsigned int>(xAOD::Iso::IsolationCaloCorrection::coreMuon));

    if ( !m_caloIsolationTool->caloCellIsolation( caloIsolation, *tp, etCones, corrlist  ) ){
      ATH_MSG_WARNING("Calculation of calorimeter isolation failed");
      continue;
    }

    for( unsigned int i=0;i<etCones.size();++i ) {
      ((xAOD::Muon*)muon)->setIsolation(caloIsolation.etcones[i], etCones[i]);
    }

    //Monitor the values
    m_etiso_cone2.push_back( caloIsolation.etcones[2]/1000.0 );
    m_etiso_cone3.push_back( caloIsolation.etcones[1]/1000.0 );
    m_etiso_cone4.push_back( caloIsolation.etcones[0]/1000.0 );

    if(m_debug) {
      ATH_MSG_DEBUG("Filled muon isolation information with:");
      ATH_MSG_DEBUG("\tCone Et 0.2 sum = " << m_etiso_cone2.back() << " GeV");
      ATH_MSG_DEBUG("\tCone Et 0.3 sum = " << m_etiso_cone3.back() << " GeV");
      ATH_MSG_DEBUG("\tCone Et 0.4 sum = " << m_etiso_cone4.back() << " GeV");
    }

  }
}

// ------------------------------------------------------------------------------------------------------
/**
 * Execute function - called for each roi.
 */
HLT::ErrorCode TrigMuonEFCaloIsolation::hltExecute(const HLT::TriggerElement *inputTE, HLT::TriggerElement *TEout) {
  
  ATH_MSG_DEBUG(": Executing TrigMuonEFCaloIsolation::execHLTAlgorithm()");
  
  //Access the last created trigger cell container
  std::vector<const CaloCellContainer* > vectorOfCellContainers;
    
  if( getFeatures(TEout, vectorOfCellContainers, "") != HLT::OK) {
    ATH_MSG_ERROR("Failed to get TrigCells");   
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::NAV_ERROR);
  }
	 
  ATH_MSG_DEBUG("Got vector with " << vectorOfCellContainers.size() << " CellContainers");

  //Get the name of the cell container so that the offline tool can be told which container to use
  if ( vectorOfCellContainers.size() > 0 ){

    const CaloCellContainer* theCellCont = vectorOfCellContainers.back();

    std::string cellCollKey;
    if ( getStoreGateKey( theCellCont, cellCollKey) != HLT::OK) {
      ATH_MSG_ERROR("Failed to get key for TrigCells");   
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::NAV_ERROR);
    }

    ATH_MSG_DEBUG(" Retrieved the cell container in the RoI called " << cellCollKey);


  } else {
    ATH_MSG_ERROR("Failed to get TrigCells");   
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::NAV_ERROR);
  }

  /// extract EF muons
  const xAOD::MuonContainer* muonContainer(0);
  if(HLT::OK != getFeature(inputTE, muonContainer)) {
    ATH_MSG_WARNING("Could not get xAOD::MuonContainer from the trigger element");
    return HLT::MISSING_FEATURE;
  } else {
    if(!muonContainer) {
      ATH_MSG_WARNING("muonContainer is 0 pointer");
      return HLT::MISSING_FEATURE;
    }
    ATH_MSG_DEBUG("MuonContainer extracted with size = " << muonContainer->size());
  }
    
  //For each of the muons fill the calorimeter isolation values using the offline tool
  fillCaloIsolation(muonContainer);
  
  //validate sequence
  TEout->setActiveState(true);
  
  return HLT::OK;
}//hltExecute


// ------------------------------------------------------------------------------------------------------
/**
 * Finalize the algorithm.
 */
HLT::ErrorCode TrigMuonEFCaloIsolation::hltFinalize() {
  ATH_MSG_DEBUG("Finalizing " + name());  
  return HLT::OK;
}

/**
 * Here we can handle incidents like end of event, but for now
 * nothing to do.
 */
void TrigMuonEFCaloIsolation::handle(const Incident & /*inc*/) {
}




