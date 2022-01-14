/*  
 Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/WriteDecorHandle.h" 
// Flow Element EDM
#include "xAODPFlow/FlowElementContainer.h"
#include "xAODPFlow/FlowElement.h"
#include "xAODEgamma/ElectronContainer.h" 
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/Electron.h" 
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/ElectronxAODHelpers.h"
#include "xAODEgamma/EgammaxAODHelpers.h" 


#include "eflowRec/PFEGamFlowElementAssoc.h" 


using ElectronLink_t = ElementLink<xAOD::ElectronContainer>; 
using PhotonLink_t = ElementLink<xAOD::PhotonContainer>;
using FlowElementLink_t = ElementLink<xAOD::FlowElementContainer>; 

PFEGamFlowElementAssoc::PFEGamFlowElementAssoc(
const std::string& name,
  ISvcLocator* pSvcLocator
  ):
  AthReentrantAlgorithm(name,pSvcLocator)
{
}

// Class destructor 
PFEGamFlowElementAssoc::~PFEGamFlowElementAssoc(){}

StatusCode PFEGamFlowElementAssoc::initialize()
{

  ATH_MSG_VERBOSE("Initializing "<<name() << "...");
 ATH_CHECK(m_electronNeutralFEWriteDecorKey.initialize());   
 ATH_CHECK(m_electronChargedFEWriteDecorKey.initialize());   
 ATH_CHECK(m_neutralFEElectronWriteDecorKey.initialize());   
 ATH_CHECK(m_chargedFEElectronWriteDecorKey.initialize());  

 ATH_CHECK(m_photonNeutralFEWriteDecorKey.initialize());
 ATH_CHECK(m_photonChargedFEWriteDecorKey.initialize());
 ATH_CHECK(m_neutralFEPhotonWriteDecorKey.initialize());
 ATH_CHECK(m_chargedFEPhotonWriteDecorKey.initialize());

 //Init ReadHandleKeys
 ATH_CHECK(m_photonReadHandleKey.initialize());
 ATH_CHECK(m_electronReadHandleKey.initialize());
 ATH_CHECK(m_chargedFEReadHandleKey.initialize());
 ATH_CHECK(m_neutralFEReadHandleKey.initialize());

 ATH_MSG_VERBOSE("Initialization completed successfully");   

return StatusCode::SUCCESS;
}

StatusCode PFEGamFlowElementAssoc::finalize(){
return StatusCode::SUCCESS;
}
/**
   This algorithm does the following:
   1) Read the Input containers for Flow Elements, Electrons and Photons
   2) Match the Clusters in the flow elements to the relevant E/Photon clusters (Neutral) or match tracks to relevant Electron/Photon tracks (Charged)
   3) Link them
   4) output the Electron/Photon containers with the linkers to the Flow element containers 
**/
StatusCode PFEGamFlowElementAssoc::execute(const EventContext &ctx) const
{
  // write decoration handles for the electron, photon and FE containers -- these are the OUTPUT handles
  //Electron Write Handle
    ATH_MSG_VERBOSE("Exec start");  
  SG::WriteDecorHandle<xAOD::ElectronContainer, std::vector<FlowElementLink_t> > electronNeutralFEWriteDecorHandle (m_electronNeutralFEWriteDecorKey,ctx);   
  SG::WriteDecorHandle<xAOD::ElectronContainer, std::vector<FlowElementLink_t> > electronChargedFEWriteDecorHandle (m_electronChargedFEWriteDecorKey,ctx);   
  SG::WriteDecorHandle<xAOD::FlowElementContainer, std::vector<ElectronLink_t> > neutralFEElectronWriteDecorHandle (m_neutralFEElectronWriteDecorKey,ctx);   
  SG::WriteDecorHandle<xAOD::FlowElementContainer, std::vector<ElectronLink_t> > chargedFEElectronWriteDecorHandle (m_chargedFEElectronWriteDecorKey,ctx); 
  ATH_MSG_VERBOSE("Defined Electron writehandles");
  //Photon Write Handle
  SG::WriteDecorHandle<xAOD::PhotonContainer, std::vector<FlowElementLink_t> > photonNeutralFEWriteDecorHandle (m_photonNeutralFEWriteDecorKey,ctx);
  SG::WriteDecorHandle<xAOD::PhotonContainer, std::vector<FlowElementLink_t> > photonChargedFEWriteDecorHandle (m_photonChargedFEWriteDecorKey,ctx);
  SG::WriteDecorHandle<xAOD::FlowElementContainer, std::vector<PhotonLink_t> > neutralFEPhotonWriteDecorHandle (m_neutralFEPhotonWriteDecorKey,ctx);
  SG::WriteDecorHandle<xAOD::FlowElementContainer, std::vector<PhotonLink_t> > chargedFEPhotonWriteDecorHandle (m_chargedFEPhotonWriteDecorKey,ctx);  
  ATH_MSG_VERBOSE("Defined Photon WriteHandles");
  
  // This is the READ handles (so the input containers for electron, photon, FE)
  
  SG::ReadHandle<xAOD::ElectronContainer>electronReadHandle (m_electronReadHandleKey,ctx);
  SG::ReadHandle<xAOD::PhotonContainer> photonReadHandle (m_photonReadHandleKey,ctx);
  
  // Charged and Neutral PFlow "Flow elements"
  SG::ReadHandle<xAOD::FlowElementContainer> neutralFEReadHandle (m_neutralFEReadHandleKey,ctx);   
  SG::ReadHandle<xAOD::FlowElementContainer> chargedFEReadHandle (m_chargedFEReadHandleKey,ctx);   
  ATH_MSG_VERBOSE("Defined ReadHandles");
  // now initialise some Flow element link containers
  std::vector<std::vector<FlowElementLink_t>> electronNeutralFEVec(electronReadHandle->size());
  std::vector<std::vector<FlowElementLink_t>> electronChargedFEVec(electronReadHandle->size());
  
  std::vector<std::vector<FlowElementLink_t>> photonNeutralFEVec(photonReadHandle->size());
  std::vector<std::vector<FlowElementLink_t>> photonChargedFEVec(photonReadHandle->size());
  ATH_MSG_VERBOSE("Finished runtime init");
  //////////////////////////////////////////////////////////////////////////
  ////                      DO MATCHING/LINKING
  //////////////////////////////////////////////////////////////////////////
  

  ///////////////////////////
  // Loop over neutral flow elements (FEs)
  ///////////////////////////
  for (const xAOD::FlowElement* FE: *neutralFEElectronWriteDecorHandle){

    //nullptr catch for removed NFE clusters - only comes up if using AOD where the alg might have some skimmed components
    if(FE->otherObjects().empty()){
	continue;
    }  
    if(FE->otherObjects().at(0)==nullptr){
	continue;
    };
    //Obtain the index of the FE calo-cluster    
    size_t FEClusterIndex=FE->otherObjects().at(0)->index();

    // init the linkers
    std::vector<ElectronLink_t> FEElectronLinks;
    std::vector<PhotonLink_t> FEPhotonLinks;

    double FE_cluster_E=FE->otherObjects().at(0)->p4().E();
    bool neg_E_cluster=(FE_cluster_E<0);
    
    //Loop over electrons:

    for (const xAOD::Electron* electron: *electronNeutralFEWriteDecorHandle){
      // get the calo clusters from the electron
	ATH_MSG_VERBOSE("NFE: start el loop");
      const std::vector<const xAOD::CaloCluster*> electronTopoClusters = xAOD::EgammaHelpers::getAssociatedTopoClusters(electron->caloCluster());
      ATH_MSG_VERBOSE("NFE: got calo clusters");
      
      for(const xAOD::CaloCluster* cluster : electronTopoClusters){
	// obtain the index of the electron seed topocluster
	size_t electronClusterIndex=cluster->index();	
	//match the indices: Cluster match between Flow Element (FE) and electron
	if(electronClusterIndex==FEClusterIndex){
	  FEElectronLinks.emplace_back(*electronReadHandle,electron->index() );
	  //Add Flow Element (FE) link to a vector
	  //index() is the unique index of the Flow Element in the container
	  electronNeutralFEVec.at(electron->index()).push_back(FlowElementLink_t(*neutralFEReadHandle, FE->index()) );
	  if(neg_E_cluster){
	    ATH_MSG_ERROR("Negative energy cluster found and matched to electron");
	    ATH_MSG_ERROR("Cluster Energy: "<<FE_cluster_E<<"");
	  }
	}// end of matching block

      } // end loop over cluster
      
    } // end Electron loop
    ATH_MSG_VERBOSE("NFE: Electron loop finished");
    // now loop over photons
    for (const xAOD::Photon* photon: *photonNeutralFEWriteDecorHandle){
      // retrieve clusters from the photon container
      const std::vector<const xAOD::CaloCluster*> photonTopoClusters = xAOD::EgammaHelpers::getAssociatedTopoClusters(photon->caloCluster());
      //loop over clusters, and do the matching
      for (const xAOD::CaloCluster* cluster: photonTopoClusters){
	//retrieve index of the cluster
	size_t photonClusterIndex=cluster->index();
	//do the matching
	if(photonClusterIndex==FEClusterIndex){
	  // Add flow element (FE) links to photon
	  FEPhotonLinks.emplace_back(*photonReadHandle,photon->index() );
	  //Add Flow Element (FE) link to a vector
	  //index() is the unique index of the Flow Element in the container
	  photonNeutralFEVec.at(photon->index()).push_back(FlowElementLink_t(*neutralFEReadHandle, FE->index()) );	  

	  if(neg_E_cluster){
	    ATH_MSG_ERROR("Negative energy cluster found and matched to photon");
	    ATH_MSG_ERROR("Cluster Energy: "<<FE_cluster_E<<"");
	  }
	}// end of matching block
      } // end of neutral loop


    }// end of photon loop
    ATH_MSG_VERBOSE("NFE: Photon Loop complete");      
    //Add vector of electron element links as decoration to FlowElement container
    neutralFEElectronWriteDecorHandle(*FE)=FEElectronLinks;
    neutralFEPhotonWriteDecorHandle(*FE)=FEPhotonLinks;
    ATH_MSG_VERBOSE("NFE::WriteHandle mapping");
  } // end neutral FE loop
  ATH_MSG_VERBOSE("NFE: Loop finished");
  ////////////////////////////////////////////////////////
  //             Loop over charged Flow Elements (FEs)
  ////////////////////////////////////////////////////////
  for (const xAOD::FlowElement* FE: *chargedFEElectronWriteDecorHandle){
      // Charged Flow Element catch for a case where there are removed tracks - should only apply if running from AOD
      if(FE->chargedObjects().empty()){
	  continue;
      }
      if(FE->chargedObjects().at(0)==nullptr){
	  continue;
      }  
    // retrieve the track from the Flow element
    size_t FETrackIndex=FE->chargedObjects().at(0)->index();
    // Initialise a vector of element links to electrons/Photons
    std::vector<ElectronLink_t> FEElectronLinks;
    std::vector<PhotonLink_t> FEPhotonLinks;

    //loop over electrons
    for (const xAOD::Electron* electron: *electronChargedFEWriteDecorHandle){
      //obtain the clusters
       const std::vector<const xAOD::TrackParticle*> electronTrackParticles = xAOD::EgammaHelpers::getTrackParticlesVec(electron, true, true); // useBremAssoc = true (get original non-GSF track), allParticles = true (include all track particles)
       // loop over tracks
       for (const xAOD::TrackParticle* electronTrack: electronTrackParticles){
	 size_t electronTrackIndex = electronTrack->index();

	 //link to FE if track indices match
	 if(electronTrackIndex==FETrackIndex){
	   // Add electron element link to a vector 
	   // index() is the unique index of the electron in the electron container 
	   FEElectronLinks.emplace_back(*electronReadHandle, electron->index() );
	   // Add FE element link to a vector 
	   // index() is the unique index of the cFE in the cFE container 
	   electronChargedFEVec.at(electron->index()).push_back( FlowElementLink_t(*chargedFEReadHandle, FE->index()) );  

	 }//end of matching block
	 
       }//end of loop on clusters
    } // end of loop on electrons
    
    for(const xAOD::Photon* photon: *photonChargedFEWriteDecorHandle){
      //obtain indices of the converted photon's original tracks
      const std::set<const xAOD::TrackParticle*> photonTrackParticles = xAOD::EgammaHelpers::getTrackParticles(photon, true);
      // loop over the tracks
      for (const xAOD::TrackParticle* photonTrack: photonTrackParticles){
	size_t photonTrackIndex=photonTrack->index();
	
	// Link the photon to the Flow Element (FE) if the track indices match
	if (photonTrackIndex==FETrackIndex){
	            // Add photon element link to a vector
          // index() is the unique index of the photon in the photon container
          FEPhotonLinks.emplace_back(*photonReadHandle, photon->index() );
          // Add FE element link to a vector
          // index() is the unique index of the cFE in the cFE container
          photonChargedFEVec.at(photon->index()).push_back( FlowElementLink_t(*chargedFEReadHandle, FE->index()) );
	}// end of matching block

      }// end of loop on tracks
      
    }//end of loop on photons


    // Add vector of electron element links as decoration to FE container     
    chargedFEElectronWriteDecorHandle (*FE) = FEElectronLinks;  
    // Add vector of photon element links as decoration to FE container
    chargedFEPhotonWriteDecorHandle (*FE) = FEPhotonLinks;

  } // end of charged FE loop
  


  ////////////////////////////////////////////////////
  //   WRITE OUTPUT: ADD HANDLES TO EL/PHOT CONTAINERS
  ////////////////////////////////////////////////////
  // Add the vectors of the Flow Element (FE) Links as decorations to the electron container
  for (const xAOD::Electron* electron : *electronNeutralFEWriteDecorHandle){
    electronNeutralFEWriteDecorHandle (*electron) = electronNeutralFEVec.at(electron->index());
    electronChargedFEWriteDecorHandle (*electron) = electronChargedFEVec.at(electron->index());
  } //end of photon loop
  // Add the vectors of the Flow Element (FE) Links as decorations to the photon container
  for (const xAOD::Photon* photon: *photonNeutralFEWriteDecorHandle){
    photonNeutralFEWriteDecorHandle (*photon) = photonNeutralFEVec.at(photon->index());
    photonChargedFEWriteDecorHandle (*photon) = photonChargedFEVec.at(photon->index());
  } // end of loop on photons

  ATH_MSG_VERBOSE("Execute completed successfully");

  return StatusCode::SUCCESS;
}
