/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <vector> 
#include <cmath>
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthContainers/DataVector.h"
#include "EFTauTopoHypo.h"
#include "TrigNavStructure/TrigNavStructure.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJet.h"
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"

// Constructor
EFTauTopoHypo::EFTauTopoHypo(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator)
{
  declareProperty("DRMin", m_dRmin = 0.);
  declareProperty("DRMax", m_dRmax = 10.);

  declareMonitoredVariable("DROfProcessed", m_monDRall=-1.);
  declareMonitoredVariable("DROfAccepted", m_monDR=-1.);
  declareMonitoredVariable("CutCounter", m_cutCounter=0);

  m_dR=0.;
}

//Destructor
EFTauTopoHypo::~EFTauTopoHypo()
{  
}

//Athena Hooks
HLT::ErrorCode EFTauTopoHypo::hltInitialize()
// ----------------------------------------------------------------------
{
  ATH_MSG_INFO( "In initialize()" );
  ATH_MSG_INFO( "REGTEST: EFTauTopoHypo will cut on" );
  ATH_MSG_INFO( "REGTEST: param DRMin " << m_dRmin );
  ATH_MSG_INFO( "REGTEST: param DRMax " << m_dRmax );

  if(m_dRmax < m_dRmin) {
    ATH_MSG_ERROR( "EFTauTopoHypo is uninitialized!" );
    return HLT::BAD_JOB_SETUP;
  }
	
  return HLT::OK;
}

HLT::ErrorCode EFTauTopoHypo::hltFinalize()
{
  ATH_MSG_DEBUG("Finalising EFTauTopoHypo"); 
  return HLT::OK;
}

HLT::ErrorCode EFTauTopoHypo::hltExecute(const HLT::TriggerElement* inputTE, bool& pass)
{
  pass = false;
  m_dR = 0.;
  m_monDRall = -1.;
  m_monDR = -1.;
  m_cutCounter = 0;

  std::vector<const xAOD::TrigCompositeContainer*> vms;
  if (getFeatures(inputTE, vms) != HLT::OK) {
    ATH_MSG_WARNING("Failed to get TrigCompositeContainer");
    return HLT::MISSING_FEATURE;
  } 
  else {
    ATH_MSG_DEBUG("Number of TrigCompositeContainers " << vms.size());
  }

  const xAOD::TrigCompositeContainer *cont=vms.back();
  if(!cont) {
    ATH_MSG_ERROR("REGTEST: Retrieval of TrigCompositeContainer from vector failed");
    return HLT::OK;
  }
  else ATH_MSG_DEBUG("REGTEST: Number of TrigComposites " << cont->size());

  for(const xAOD::TrigComposite* comp:*cont) {
    if(comp->name()!="EFTAU_dR") {
      ATH_MSG_DEBUG("REGTEST: Not EFTAUInfo TrigComposite");
      continue;
    }
    if(!comp->getDetail("dR",m_dR)) {
      ATH_MSG_ERROR("REGTEST: Cannot retrieve dR");
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
    }

    m_cutCounter++;
    m_monDRall = m_dR;
    if(m_dR<0.1) {
      ATH_MSG_DEBUG( "Close-by pair with dR " << m_dR );
    }
    
    if( m_dR>=m_dRmin && m_dR<m_dRmax ) {
      m_monDR = m_dR;
      ATH_MSG_DEBUG("Good pair with dR: " << m_dR);
      pass = true;
      m_cutCounter++;
      return HLT::OK;
    }
    
    ATH_MSG_DEBUG("Combination with dR: " << m_dR);
    
  } // End of loop over TrigComposites in EFTAUInfo
  
  return HLT::OK;  
}
  
