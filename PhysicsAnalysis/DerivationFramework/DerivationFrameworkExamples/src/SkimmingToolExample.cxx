/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// SkimmingToolExample.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// Based on the Integrated Simulation Framework
// This is a trivial example of an implementation of a skimming tool 
// which only passes events with N combined muons passing a pt cut of M

#include "DerivationFrameworkExamples/SkimmingToolExample.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::SkimmingToolExample::SkimmingToolExample( const std::string& t,
                                                 const std::string& n,
                                                 const IInterface* p ) : 
  AthAlgTool(t,n,p),
  m_ntot(0),
  m_npass(0),
  m_muonSGKey("Muons"),
  m_nMuons(2),
  m_muonPtCut(10.0)
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
    declareProperty("MuonContainerKey", m_muonSGKey);
    declareProperty("NumberOfMuons", m_nMuons);
    declareProperty("MuonPtCut", m_muonPtCut);	
  }
  
// Destructor
DerivationFramework::SkimmingToolExample::~SkimmingToolExample() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::SkimmingToolExample::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::SkimmingToolExample::finalize()
{
     ATH_MSG_VERBOSE("finalize() ...");
     ATH_MSG_INFO("Processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter ");
     return StatusCode::SUCCESS;
}

// The filter itself
bool DerivationFramework::SkimmingToolExample::eventPassesFilter() const
{
     ++m_ntot;

     // Retrieve muon container	
     const xAOD::MuonContainer* muons(0);
     StatusCode sc = evtStore()->retrieve(muons,m_muonSGKey);	
     if (sc.isFailure()) {
	ATH_MSG_FATAL("No muon collection with name " << m_muonSGKey << " found in StoreGate!");
	return false;
     } 
     
     // Loop over muons, count up and set decision
     xAOD::MuonContainer::const_iterator muItr;
     unsigned int nGoodMu(0);
     for (muItr=muons->begin(); muItr!=muons->end(); ++muItr) {  
	if ( (*muItr)->muonType() == xAOD::Muon::Combined && (*muItr)->pt() > m_muonPtCut ) ++nGoodMu;
     }
     bool acceptEvent(false);
     if (nGoodMu >= m_nMuons) { 
	acceptEvent = true;
	++m_npass;
     }
     return acceptEvent; 

}  
  
