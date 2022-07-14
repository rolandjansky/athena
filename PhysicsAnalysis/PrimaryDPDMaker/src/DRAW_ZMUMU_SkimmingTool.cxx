/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// DRAW_ZMUMUSkimmingTool.cxx
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// This tool implements the "good muon" event selection required 
// by the DRAW_ZMUMU format, avoiding the use of decorators on the 
// muons. Inherits from derivation framework components.

#include "PrimaryDPDMaker/DRAW_ZMUMUSkimmingTool.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::DRAW_ZMUMUSkimmingTool::DRAW_ZMUMUSkimmingTool( const std::string& t,
                                                 const std::string& n,
                                                 const IInterface* p ) : 
  AthAlgTool(t,n,p) {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
 
  }
  
// Destructor
DerivationFramework::DRAW_ZMUMUSkimmingTool::~DRAW_ZMUMUSkimmingTool() = default;
// Athena initialize and finalize
StatusCode DerivationFramework::DRAW_ZMUMUSkimmingTool::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     ATH_CHECK(m_muonSGKey.initialize());
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
     SG::ReadHandle<xAOD::MuonContainer> muons{m_muonSGKey};
     // Loop over muons, count up and set decision
     unsigned int nGoodMu(0);
     for (const xAOD::Muon* muItr : *muons) {
	    nGoodMu += muItr->pt() > m_muonPtCut && m_muonSelectionTool->accept(*muItr);
     }
     bool acceptEvent{false};
     if (nGoodMu >= m_nMuons) { 
	     acceptEvent = true;
	     ++m_npass;
     }
     return acceptEvent; 
}  
  
