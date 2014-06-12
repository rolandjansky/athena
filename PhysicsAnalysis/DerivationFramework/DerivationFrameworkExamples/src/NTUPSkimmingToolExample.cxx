/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// NTUPSkimmingToolExample.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// Based on the Integrated Simulation Framework
// This is a trivial example of an implementation of a skimming tool 
// which only passes events with N combined muons passing a pt cut of M
// Implemented for NTUP-to-NTUP

#include "DerivationFrameworkExamples/NTUPSkimmingToolExample.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::NTUPSkimmingToolExample::NTUPSkimmingToolExample( const std::string& t,
                                                 const std::string& n,
                                                 const IInterface* p ) : 
  AthAlgTool(t,n,p),
  m_ntot(0),
  m_npass(0),
  m_mu_isComb_name("mu_muid_isCombinedMuon"),
  m_mu_pt_name("mu_muid_pt"), 
  m_nMuons(2),
  m_muonPtCut(10.0)
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
    declareProperty("IsCombinedMuonBranchName",m_mu_isComb_name);
    declareProperty("MuonPtBranchName",m_mu_pt_name);
    declareProperty("NumberOfMuons", m_nMuons);
    declareProperty("MuonPtCut", m_muonPtCut);	
  }
  
// Destructor
DerivationFramework::NTUPSkimmingToolExample::~NTUPSkimmingToolExample() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::NTUPSkimmingToolExample::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::NTUPSkimmingToolExample::finalize()
{
     ATH_MSG_VERBOSE("finalize() ...");
     ATH_MSG_INFO("Processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter ");
     return StatusCode::SUCCESS;
}

// The filter itself
bool DerivationFramework::NTUPSkimmingToolExample::eventPassesFilter() const
{
     ++m_ntot;

     // Retrieve data
     // isComb branch
     StatusCode sc = evtStore()->retrieve((const std::vector<int>*&)mu_isComb,m_mu_isComb_name);
     if (sc.isFailure()) {
        ATH_MSG_FATAL("No branch with name " << m_mu_isComb_name << " found in StoreGate!");
        return StatusCode::FAILURE;
     }
     // pt branch 	
     sc = evtStore()->retrieve((const std::vector<float>*&)mu_pt,m_mu_pt_name);
     if (sc.isFailure()) {
     	ATH_MSG_FATAL("No branch with name " << m_mu_pt_name << " found in StoreGate!");
        return StatusCode::FAILURE;
     } 

     // Loop over muons, count up and set decision
     unsigned int nGoodMu(0);
     unsigned int muItr(0);
     unsigned int nMu = mu_pt->size();
     for (muItr=0; muItr < nMu; ++muItr) {  
	if ( ((*mu_isComb)[muItr]==1) && (*mu_pt)[muItr] > m_muonPtCut ) ++nGoodMu;
     }
     bool acceptEvent(false);
     if (nGoodMu >= m_nMuons) { 
	acceptEvent = true;
	++m_npass;
     }
     return acceptEvent; 

}  
  
