/*  
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/PFMuonFlowElementAssoc.h" 
#include "StoreGate/WriteDecorHandle.h" 
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODPFlow/FlowElementContainer.h" 
#include "xAODPFlow/FlowElement.h" 

typedef ElementLink<xAOD::MuonContainer> MuonLink_t; 
typedef ElementLink<xAOD::FlowElementContainer> FlowElementLink_t; 
//
//      Algorithm created by M.T. Anthony
//
// ============================================================= 
PFMuonFlowElementAssoc::PFMuonFlowElementAssoc(const std::string& name, 
		   ISvcLocator* pSvcLocator): 
  AthReentrantAlgorithm(name, pSvcLocator)
{   
  // Declare the decoration keys   
  declareProperty ("MuonChargedFlowElementDecorKey", m_muonChargedFEWriteDecorKey = "Muons.chargedFELinks");   // updated muon container with the new link 
  declareProperty("ChargedFlowElementMuonDecorKey", m_ChargedFEmuonWriteDecorKey="JetETMissChargedFlowElements.FE_MuonLinks"); // updated Charge
  declareProperty ("MuonNeutralFlowElementDecorKey", m_muonNeutralFEWriteDecorKey = "Muons.neutralFELinks");
  declareProperty ("NeutralFlowElementMuonDecorKey",m_NeutralFEmuonWriteDecorKey = "JetETMissNeutralFlowElements.FE_MuonLinks");
} 
PFMuonFlowElementAssoc::~PFMuonFlowElementAssoc() {} 

// ============================================================= 
StatusCode PFMuonFlowElementAssoc::initialize() {   

  ATH_MSG_DEBUG("Initializing " << name() << "...");   

  // Initialise the decoration keys   
  ATH_CHECK(m_muonChargedFEWriteDecorKey.initialize());
  ATH_CHECK(m_muonNeutralFEWriteDecorKey.initialize());
  
  ATH_CHECK(m_ChargedFEmuonWriteDecorKey.initialize());
  ATH_CHECK(m_NeutralFEmuonWriteDecorKey.initialize());

  //init ReadHandleKeys
  ATH_CHECK(m_muonReadHandleKey.initialize());
  ATH_CHECK(m_chargedFEReadHandleKey.initialize());
  ATH_CHECK(m_neutralFEReadHandleKey.initialize());

  ATH_MSG_DEBUG("Initialization completed successfully");   

  return StatusCode::SUCCESS; 
} 

// ========================================================================= 
StatusCode PFMuonFlowElementAssoc::finalize() {   
  return StatusCode::SUCCESS; 
} 

// ========================================================================= 
StatusCode PFMuonFlowElementAssoc::execute(const EventContext & ctx) const 
{   
  
  // WriteDecorHandles for the charged/neutral Flow Elements and Muons
  // Links a Muon that has a track to a charged flow element if possible
  
  ATH_MSG_DEBUG("Started execute step");

  // Get container for muons
  SG::WriteDecorHandle<xAOD::MuonContainer,std::vector<FlowElementLink_t> > muonChargedFEWriteDecorHandle (m_muonChargedFEWriteDecorKey,ctx);
  SG::WriteDecorHandle<xAOD::MuonContainer,std::vector<FlowElementLink_t> > muonNeutralFEWriteDecorHandle (m_muonNeutralFEWriteDecorKey,ctx);
  // get container for charged flow elements
  SG::WriteDecorHandle<xAOD::FlowElementContainer,std::vector<MuonLink_t> > ChargedFEmuonWriteDecorHandle (m_ChargedFEmuonWriteDecorKey,ctx);
  SG::WriteDecorHandle<xAOD::FlowElementContainer,std::vector<MuonLink_t> > NeutralFEmuonWriteDecorHandle(m_NeutralFEmuonWriteDecorKey,ctx);

  //store readhandles for muon and charged flow elements
  SG::ReadHandle<xAOD::MuonContainer> muonReadHandle (m_muonReadHandleKey,ctx); // readhandle for muon
  SG::ReadHandle<xAOD::FlowElementContainer> ChargedFEReadHandle(m_chargedFEReadHandleKey,ctx);
  SG::ReadHandle<xAOD::FlowElementContainer> NeutralFEReadHandle(m_neutralFEReadHandleKey,ctx);
  
  //now init some Flow element link containers
  std::vector<std::vector<FlowElementLink_t> > muonChargedFEVec(muonReadHandle->size());
  std::vector<std::vector<FlowElementLink_t> > muonNeutralFEVec(muonReadHandle->size());

  //Loop over the Flow Elements 

  //////////////////////////////
  //     CHARGED LOOP
  //////////////////////////////
  for (const xAOD::FlowElement* FE: *ChargedFEmuonWriteDecorHandle){
    //get the track associated to the charged flow element (or at least the index of said track)
    size_t FETrackIndex=FE->chargedObjects().at(0)->index();
    // Init a vector of element links to muons
    std::vector<MuonLink_t> FEMuonLinks;
    
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
	    FEMuonLinks.push_back( MuonLink_t(*muonReadHandle, muon->index()));
	    // Add flow element link to a vector
	    // index() is the unique index of the cFlowElement in the cFlowElementcontaine
	    muonChargedFEVec.at(muon->index()).push_back(FlowElementLink_t(*ChargedFEReadHandle,FE->index()));
	  } // matching block
	} // TrkCont loop
      } // Size check
    }// end of muon loop
    
    // Add vector of muon element links as decoration to FlowElement container
    ChargedFEmuonWriteDecorHandle(*FE) = FEMuonLinks;
  } // end of charged Flow Element loop

  //////////////////////////////////////////////////
  //   Loop over Neutral FlowElements
  //////////////////////////////////////////////////
  if(m_LinkNeutralFEClusters){
    ATH_MSG_DEBUG("Experimental: Cluster Linkers between neutral FEs and Muons are used");
    for (const xAOD::FlowElement* FE: *NeutralFEmuonWriteDecorHandle){
      //get the index of the cluster corresponding to the Neutral FlowElements
      size_t FEclusterindex=FE->otherObjects().at(0)->index();
      
      //design the vector of ElementLinks
      std::vector<MuonLink_t> FEMuonLinks;
      for (const xAOD::Muon* muon: *muonNeutralFEWriteDecorHandle ){
	//Retrieve the ElementLink vector of clusters      
	const ElementLink<xAOD::CaloClusterContainer> ClusterLink=muon->clusterLink();
	
	//access object from element link
	const xAOD::CaloClusterContainer* clustercont = ClusterLink.getDataPtr();
	for (const xAOD::CaloCluster* cluster: *clustercont){
	  size_t cluster_index=cluster->index();
	  if(cluster_index==FEclusterindex){
	    // Add Muon element link to a vector
	    // index() is the unique index of the muon in the muon container   
	    FEMuonLinks.push_back(MuonLink_t(*muonReadHandle,muon->index()));
	    // index() is the unique index of the cFlowElement in the cFlowElementcontaine
	    muonNeutralFEVec.at(muon->index()).push_back(FlowElementLink_t(*NeutralFEReadHandle,FE->index()));
	  } // end of matching cluster index block	    	
	}  // loop over elementlink vector
      } // loop over muons
      NeutralFEmuonWriteDecorHandle(*FE)=FEMuonLinks;
    } // loop over neutral FE
  }// end of the Gaudi check block
  
  //////////////////////////////////////////////////
  //   WRITE OUTPUT: ADD HANDLES TO MUON CONTAINERS
  //////////////////////////////////////////////////
  // Add the vectors of the Flow Element Links as decoations to the muon container
  for(const xAOD::Muon* muon: *muonChargedFEWriteDecorHandle){
    muonChargedFEWriteDecorHandle(*muon)=muonChargedFEVec.at(muon->index());    
  } // end of muon loop
  if(m_LinkNeutralFEClusters){
    for(const xAOD::Muon* muon: *muonNeutralFEWriteDecorHandle){
      if(muonNeutralFEVec.size()>0){
	muonNeutralFEWriteDecorHandle(*muon)=muonNeutralFEVec.at(muon->index());
      }
    }
  }
  ATH_MSG_DEBUG("Execute completed successfully");   
  
  return StatusCode::SUCCESS;
}

