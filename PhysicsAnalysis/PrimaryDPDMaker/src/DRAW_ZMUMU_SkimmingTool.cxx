/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// DRAW_ZMUMUSkimmingTool.cxx
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// This tool implements the "good muon" event selection required 
// by the DRAW_ZMUMU format, avoiding the use of decorators on the 
// muons. Inherits from derivation framework components.

#include "PrimaryDPDMaker/DRAW_ZMUMUSkimmingTool.h"
#include "xAODMuon/MuonContainer.h"
#include "MuonSelectorTools/IMuonSelectionTool.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::DRAW_ZMUMUSkimmingTool::DRAW_ZMUMUSkimmingTool( const std::string& t,
                                                 const std::string& n,
                                                 const IInterface* p ) : 
  AthAlgTool(t,n,p),
  m_ntot(0),
  m_npass(0),
  m_muonSGKey("Muons"),
  m_muonSelectionTool("CP::MuonSelectionTool/MuonSelectionTool"),
  m_nMuons(1),
  m_muonPtCut(20.0)
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
    declareProperty("MuonContainerKey", m_muonSGKey);
    declareProperty("MuonSelectorTool", m_muonSelectionTool); 
    declareProperty("MinimumNumberOfMuons", m_nMuons);
    declareProperty("MuonPtCut", m_muonPtCut);	
  }
  
// Destructor
DerivationFramework::DRAW_ZMUMUSkimmingTool::~DRAW_ZMUMUSkimmingTool() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::DRAW_ZMUMUSkimmingTool::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::DRAW_ZMUMUSkimmingTool::finalize()
{
     ATH_MSG_VERBOSE("finalize() ...");
     ATH_MSG_INFO("Processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter ");
     return StatusCode::SUCCESS;
}

// The filter itself
bool DerivationFramework::DRAW_ZMUMUSkimmingTool::eventPassesFilter() const
{
     ++m_ntot;

     // Retrieve muon container	
     const xAOD::MuonContainer* muons(0);
     StatusCode sc = evtStore()->retrieve(muons,m_muonSGKey);	
     if (sc.isFailure()) {
	ATH_MSG_FATAL("No muon collection with name " << m_muonSGKey << " found in StoreGate!");
	return StatusCode::FAILURE;
     } 
     
     // Loop over muons, count up and set decision
     xAOD::MuonContainer::const_iterator muItr;
     unsigned int nGoodMu(0);
     for (muItr=muons->begin(); muItr!=muons->end(); ++muItr) {  
	if ( m_muonSelectionTool->accept(**muItr) && (*muItr)->pt() > m_muonPtCut ) ++nGoodMu;
     }
     bool acceptEvent(false);
     if (nGoodMu >= m_nMuons) { 
	acceptEvent = true;
	++m_npass;
     }
     return acceptEvent; 

}  
  
