/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// EmergingJetsFilterTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "LongLivedParticleDPDMaker/EmergingJetsFilterTool.h"
#include <vector>
#include <string>
#include "xAODTracking/TrackParticleContainer.h"

#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"

// Constructor
DerivationFramework::EmergingJetsFilterTool::EmergingJetsFilterTool( const std::string& t,
								     const std::string& n,
								     const IInterface* p ) : 
  AthAlgTool(t,n,p),
  m_tdt("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_ntot(0),
  m_npass(0),
  m_nptpass(0),
  m_ptCut(100000.0),
  m_etaCut(2.5),
  m_nJetsRequired(4)
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
    declareProperty("TrigDecisionTool", m_tdt, "Tool to access the trigger decision");
    declareProperty("Triggers", m_triggers = std::vector< std::string >());
    declareProperty("JetPtCut", m_ptCut);
    declareProperty("JetEtaCut", m_etaCut);
    declareProperty("nJetsRequired", m_nJetsRequired);
  }
  
// Destructor
DerivationFramework::EmergingJetsFilterTool::~EmergingJetsFilterTool() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::EmergingJetsFilterTool::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");

     if(m_tdt.retrieve().isFailure()) {
       ATH_MSG_FATAL("Failed to retrieve tool: " << m_tdt);
       return StatusCode::FAILURE;
     }
     ATH_MSG_INFO("Retrieved tool: " << m_tdt);
     ATH_CHECK(m_jetSGKey.initialize());
     
     return StatusCode::SUCCESS;
     
}
StatusCode DerivationFramework::EmergingJetsFilterTool::finalize()
{
     ATH_MSG_VERBOSE("finalize() ...");
     ATH_MSG_INFO("Processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter ");
     ATH_MSG_INFO(m_nptpass << " jets passed " << m_ptCut << " *0.001 GeV pt cut");
     return StatusCode::SUCCESS;
}

// The filter itself
bool DerivationFramework::EmergingJetsFilterTool::eventPassesFilter() const
{

  ++m_ntot;
  bool thisEventPasses = false;
  bool passedTrigger = false;

  // check for events passing triggers
  if( m_triggers.size() > 0 ){
    passedTrigger = false;

    for( unsigned int i = 0; i < m_triggers.size(); i++ ){
      int decision = m_tdt->isPassed( m_triggers.at(i) );
      if( decision == 1 ){
	passedTrigger = true;
	break;
      }
    }
  }

  // if no triggers passed, event fails filter
  if( !passedTrigger ) return thisEventPasses;


  // access jet container
  SG::ReadHandle<xAOD::JetContainer> jets(m_jetSGKey);
  if( !jets.isValid() ) {
    msg(MSG::WARNING) << "No Jet container found, will skip this event" << endmsg;
    return false;
  } 
  msg(MSG::DEBUG) << "size of Jet container is " << jets->size() << endmsg;

  // count number of jets passing pt + eta cuts
  int nJetsPassed = 0;
  for( unsigned int i = 0; i < jets->size(); ++i ){ 
    const xAOD::Jet* jet = jets->at(i);
    if( (jet->pt() < m_ptCut) || (std::abs(jet->eta()) > m_etaCut) ) continue;
    nJetsPassed += 1;
    ATH_MSG_INFO( "pt cut passing jet pt: " << jet->pt() * 0.001);
  }

  m_nptpass = nJetsPassed;

  // require certain number of jets 
  if( nJetsPassed >= m_nJetsRequired ){
    ++m_npass;
    thisEventPasses = true;
  }

  return thisEventPasses;
  
}  
  
