/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TrigMinBias/TrigTrackCounterHypo.h>
#include <xAODTrigMinBias/TrigTrackCounts.h>
#include <xAODTrigMinBias/TrigHisto2D.h>


//---------------------------------------------------------------------------------

TrigTrackCounterHypo::TrigTrackCounterHypo(const std::string& name, ISvcLocator* pSvcLocator): HLT::HypoAlgo(name, pSvcLocator)
{
  declareProperty("Max_z0", m_max_z0 = 200.0); // (mm)
  declareProperty("Min_pt", m_min_pt = 0.100); // (GeV)
  declareProperty("Min_eta", m_min_eta = -1.0); // eta
  declareProperty("Max_eta", m_max_eta = -1.0); // eta
  declareProperty("Required_ntrks", m_required_ntrks = 1); // (#)
  declareProperty("MaxRequired_ntrks", m_max_required_ntrks = -1); // (#)
  declareProperty("CutMinAndMax", m_cutMinAndMax = false); 
  declareProperty("AcceptAll_EF", m_acceptAll = false );
  
  // Monitoring
  declareMonitoredVariable("NTrksHypo", m_ntrksHypo);
  declareMonitoredVariable("NTrksSelected", m_ntrksSelected);

  m_trigTrackCounts = 0;
}

//---------------------------------------------------------------------------------

TrigTrackCounterHypo::~TrigTrackCounterHypo(){;}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigTrackCounterHypo::hltInitialize() {
  ATH_MSG_INFO("Initialising TrigTrackCounterHypo, name = " << name());
  ATH_MSG_INFO("max_z0 = " << m_max_z0 << " mm");
  ATH_MSG_INFO("min_pt = " << m_min_pt << " GeV");
  ATH_MSG_INFO("required_ntrks = " << m_required_ntrks << " #");
  ATH_MSG_INFO("Max required_ntrks = " << m_max_required_ntrks << " #");
  ATH_MSG_INFO("Apply min and max cuts = " << (m_cutMinAndMax==true ? "True" : "False"));
  ATH_MSG_INFO("AcceptAll = " << (m_acceptAll==true ? "True" : "False"));
  
  return HLT::OK;
}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigTrackCounterHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  pass=false;
  
  ATH_MSG_DEBUG("Executing this TrigTrackCounterHypo " << name());
  
  if( m_acceptAll ){
    pass = true;
    return HLT::OK;
  }
  
  // Retrieve the TrigTrackCounts object stored by the TrigTrackCounter class.
  m_trigTrackCounts = 0;
  HLT::ErrorCode hltStatus = getFeature(outputTE, m_trigTrackCounts, "trackcounts");
  if(hltStatus != HLT::OK || !m_trigTrackCounts){
    ATH_MSG_WARNING("Failed to retrieve features from TE.");
    return HLT::OK;
  }
  
  // Count the number of tracks within the selected region
  xAOD::TrigHisto2D *th2dZ0_pt = new xAOD::TrigHisto2D;
  th2dZ0_pt->makePrivateStore();

  if(m_min_eta <0 && m_max_eta<0){
    
    th2dZ0_pt->initialize(m_trigTrackCounts->z0Bins(),
			  m_trigTrackCounts->z0Min(),
			  m_trigTrackCounts->z0Max(),
			  m_trigTrackCounts->ptBins(),
			  m_trigTrackCounts->ptMin(),
			  m_trigTrackCounts->ptMax());
    th2dZ0_pt->setContents(m_trigTrackCounts->z0_pt());
    
    m_ntrksSelected =th2dZ0_pt->sumEntries(m_max_z0, m_min_pt, xAOD::TrigHistoCutType::BELOW_X_ABOVE_Y);
  } else {
    th2dZ0_pt->initialize(m_trigTrackCounts->etaBins(),
			  m_trigTrackCounts->etaMin(),
			  m_trigTrackCounts->etaMax(),
			  m_trigTrackCounts->phiBins(),
			  m_trigTrackCounts->phiMin(),
			  m_trigTrackCounts->phiMax());
    th2dZ0_pt->setContents(m_trigTrackCounts->eta_phi());
    
    double ntrksSelected_low = th2dZ0_pt->sumEntries(m_min_eta, 100, xAOD::TrigHistoCutType::ABOVE_X_BELOW_Y);
    double ntrksSelected_up = th2dZ0_pt->sumEntries(m_max_eta, 100, xAOD::TrigHistoCutType::ABOVE_X_BELOW_Y);
    
    m_ntrksSelected = ntrksSelected_low-ntrksSelected_up;
  }
  
  ATH_MSG_DEBUG("There are " << m_ntrksSelected << " tracks within selection window.  ");

  // min and max logic
  if (m_cutMinAndMax == true) {

    // Double sided cut. 
    // Check the trigger condition.
    if((m_ntrksSelected >= m_required_ntrks) && (m_ntrksSelected <= m_max_required_ntrks)) {
      pass=true;
      ATH_MSG_DEBUG("The event passes double-sided cut.");
    } else {
      ATH_MSG_DEBUG("The event fails double-sided cut.");
    }


  } else { // m_cutMinAndMax == false

    // single sided cut. 
    // Check the trigger condition.
    if(((m_ntrksSelected >= m_required_ntrks) && m_max_required_ntrks==-1) || 
       (m_ntrksSelected <= m_max_required_ntrks) ) {
      pass=true;
      ATH_MSG_DEBUG("The event passes single-sided cut.");
    } else {
      ATH_MSG_DEBUG("The event fails single-sided cut.");
    }
    
  }
  
  // for monitoring
  m_ntrksHypo = m_ntrksSelected;
  if( !pass ) m_ntrksSelected = -999;
  th2dZ0_pt->releasePrivateStore();
  delete th2dZ0_pt;
  th2dZ0_pt=0;
  
  return HLT::OK;
}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigTrackCounterHypo::hltFinalize() {
  ATH_MSG_INFO(" Finalizing this TrigTrackCounterHypo" << name() << "."); 
  return HLT::OK;  
}
