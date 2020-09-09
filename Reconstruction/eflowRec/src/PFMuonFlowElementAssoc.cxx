/*  
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/PFMuonFlowElementAssoc.h" 
#include "StoreGate/WriteDecorHandle.h" 
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODPFlow/FlowElementContainer.h" 
#include "xAODPFlow/FlowElement.h" 
#include <typeinfo> // temp debug to check object

typedef ElementLink<xAOD::MuonContainer> MuonLink_t; 
typedef ElementLink<xAOD::FlowElementContainer> FlowElementLink_t; 
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
  SG::WriteDecorHandle<xAOD::FlowElementContainer,std::vector<MuonLink_t> > ChargedFEmuonWriteDecorHandle (m_ChargedFEmuonWriteDecorKey);
  

  //store readhandles for muon and charged flow elements
  SG::ReadHandle<xAOD::MuonContainer> muonReadHandle (m_muonChargedFEWriteDecorKey.contHandleKey()); // readhandle for muon
  SG::ReadHandle<xAOD::FlowElementContainer> ChargedFEReadHandle(m_ChargedFEmuonWriteDecorKey.contHandleKey());
  
  //now init some Flow element link containers
  std::vector<std::vector<FlowElementLink_t> > muonChargedFEVec(muonReadHandle->size());

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
      const xAOD::TrackParticleContainer* TrkCont=muonTrackContLink.getDataPtr();
      if(TrkCont->size()>0){
	for(const xAOD::TrackParticle* MuonTrkParticle: *TrkCont){
	  size_t MuonTrkIndex=MuonTrkParticle->index();
	  if(MuonTrkIndex==FETrackIndex){
	    // Add Muon element link to a vector
	    // index() is the unique index of the muon in the muon container
	    feMuonLinks.push_back( MuonLink_t(*muonReadHandle, muon->index()));
	    // Add flow element link to a vector
	    // index() is the unique index of the cFlowElement in the cFlowElementcontaine
	    muonChargedFEVec.at(muon->index()).push_back(FlowElementLink_t(*ChargedFEReadHandle,FE->index()));
	  } // matching block
	} // TrkCont loop
      } // Size check
    }// end of muon loop
    
    // Add vector of muon element links as decoration to FlowElement container
    ChargedFEmuonWriteDecorHandle(*FE) = feMuonLinks;
  } // end of charged Flow Element loop


  //////////////////////////////////////////////////
  //   WRITE OUTPUT: ADD HANDLES TO MUON CONTAINERS
  //////////////////////////////////////////////////
  // Add the vectors of the Flow Element Links as decoations to the muon container
  for(const xAOD::Muon* muon: *muonChargedFEWriteDecorHandle){
    muonChargedFEWriteDecorHandle(*muon)=muonChargedFEVec.at(muon->index());
  } // end of muon loop

  ATH_MSG_DEBUG("Execute completed successfully");   
  
  return StatusCode::SUCCESS;
}

