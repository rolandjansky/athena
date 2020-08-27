/*  
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/PFMuonFlowElementAssoc.h" 
#include "StoreGate/WriteDecorHandle.h" 
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODPFlow/FlowElementContainer.h" 
#include "xAODPFlow/FlowElement.h" 

typedef ElementLink<xAOD::MuonContainer> MuonLink_t; 
typedef ElementLink<xAOD::FlowElement> FlowElementLink_t; 
//
//      Algorithm created by M.T. Anthony
//
// ============================================================= 
PFMuonFlowElementAssoc::PFMuonFlowElementAssoc(const std::string& name, 
		   ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator)
{   
  // Declare the decoration keys   
  declareProperty ("MuonChargedFlowElementDecorKey", m_muonChargedFEWriteDecorKey = "Muons.neutralfeLinks");   // updated muon container with the new link 
  declareProperty("ChargedFlowElementMuonDecorKey", m_ChargedFEmuonWriteDecorKey="JetETMissChargedFlowElements.fe_MuonLinks"); // updated Charge
} 
PFMuonFlowElementAssoc::~PFMuonFlowElementAssoc() {} 

// ============================================================= 
StatusCode PFMuonFlowElementAssoc::initialize() {   

  ATH_MSG_DEBUG("Initializing " << name() << "...");   

  // Initialise the decoration keys   
  ATH_CHECK(m_muonChargedFEWriteDecorKey.initialize());
  ATH_CHECK(m_ChargedFEmuonWriteDecorKey.initialize());

  ATH_MSG_DEBUG("Initialization completed successfully");   

  return StatusCode::SUCCESS; 
} 

// ========================================================================= 
StatusCode PFMuonFlowElementAssoc::finalize() {   
  return StatusCode::SUCCESS; 
} 

// ========================================================================= 
StatusCode PFMuonFlowElementAssoc::execute() {   
  
  // WriteDecorHandles for the charged Flow Elements and Muons
  // Links a Muon that has a track to a charged flow element if possible
  
  ATH_MSG_DEBUG("Started execute step");

  // Get container for muons
  SG::WriteDecorHandle<xAOD::MuonContainer,std::vector<FlowElementLink_t> > muonChargedFEWriteDecorHandle (m_muonChargedFEWriteDecorKey);
  // get container for charged flow elements
  SG::WriteDecorHandle<xAOD::FlowElementContainer,std::vector<MuonLink_t> > ChargedFEmuonWriteDecorHandle (m_ChargedFEmuonWriteDecorHandle);
  

  //store readhandles for muon and charged flow elements
  SG::ReadHandle<xAOD::MuonContainer> muonReadHandle (m_muonChargedFEWriteDecorKey.contHandleKey()); // readhandle for muon
  SG::ReadHandle<xAOD::FlowElementContainer> ChargedFEReadHandle(m_ChargedFEmuonWriteDecorKey.contHandleKey());
  
  //Loop over the Flow Elements 

  //////////////////////////////
  //     CHARGED LOOP
  //////////////////////////////
  for (const xAOD::FlowElement* FE: *ChargedFEmuonWriteDecorHandle){
    //get the track associated to the charged flow element (or at least the index of said track
    size_t FETrackIndex=FE->chargedObjects().at(0)->index();
    // Init a vector of element links to muons
    std::vector<MuonLink_t> feMuonLinks;
    
    //loop over muons in container
    for(const xAOD::Muon* muon: *muonChargedFEWriteDecorHandle){
      // retrieve a link to an ID track where possible
      const ElementLink<xAOD::TrackParticleContainer> muonTrackContLink=muon->inDetTrackParticleLink();
      //catch for case where muon does not contain an ID track
      if(muonTrackContLink->size()>0){
	// loop over tracks: should be one element at most
	for (xAOD::TrackParticle* muontrack: *muonTrackContLink){
	  size_t muontrackindex=muontrack->index();
	  if(muontrackindex==FETrackIndex){
	    
	  }//end of matching block
	}// end of loop over tracks
      }// end of size check on muonTrackContLink
    }// end of muon loop
    
    
  } // end of charged Flow Element loop

  ATH_MSG_DEBUG("Execute completed successfully");   
  
  return StatusCode::SUCCESS;
}

