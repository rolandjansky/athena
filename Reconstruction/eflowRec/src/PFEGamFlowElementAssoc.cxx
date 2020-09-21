/*  
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include <typeinfo> // temp debug for type checks on objects

typedef ElementLink<xAOD::ElectronContainer> ElectronLink_t; 
typedef ElementLink<xAOD::PhotonContainer> PhotonLink_t;
typedef ElementLink<xAOD::FlowElementContainer> FlowElementLink_t; 

PFEGamFlowElementAssoc::PFEGamFlowElementAssoc(
const std::string& name,
  ISvcLocator* pSvcLocator
  ):
  AthReentrantAlgorithm(name,pSvcLocator)
{
// Class initializer 
declareProperty ("ElectronNeutralFEDecorKey", m_electronNeutralFEWriteDecorKey = "Electrons.neutralfeLinks");   
declareProperty ("ElectronChargedFEDecorKey", m_electronChargedFEWriteDecorKey = "Electrons.chargedfeLinks");   

declareProperty ("NeutralFEElectronDecorKey", m_neutralfeElectronWriteDecorKey = "JetETMissNeutralFlowElements.fe_ElectronLinks");   

declareProperty ("ChargedFEElectronDecorKey", m_chargedfeElectronWriteDecorKey = "JetETMissChargedFlowElements.fe_ElectronLinks"); 

declareProperty ("PhotonNeutralFEDecorKey", m_photonNeutralFEWriteDecorKey = "Photons.neutralfeLinks");
declareProperty ("PhotonChargedFEDecorKey", m_photonChargedFEWriteDecorKey = "Photons.chargedfeLinks");
declareProperty ("NeutralFEPhotonDecorKey", m_neutralfePhotonWriteDecorKey = "JetETMissNeutralFlowElements.fe_PhotonLinks");
declareProperty ("ChargedFEPhotonDecorKey", m_chargedfePhotonWriteDecorKey = "JetETMissChargedFlowElements.fe_PhotonLinks");
}

// Class destructor 
PFEGamFlowElementAssoc::~PFEGamFlowElementAssoc(){}

StatusCode PFEGamFlowElementAssoc::initialize()
{

  ATH_MSG_DEBUG("Initializing "<<name() << "...");
 ATH_CHECK(m_electronNeutralFEWriteDecorKey.initialize());   
 ATH_CHECK(m_electronChargedFEWriteDecorKey.initialize());   
 ATH_CHECK(m_neutralfeElectronWriteDecorKey.initialize());   
 ATH_CHECK(m_chargedfeElectronWriteDecorKey.initialize());  

 ATH_CHECK(m_photonNeutralFEWriteDecorKey.initialize());
 ATH_CHECK(m_photonChargedFEWriteDecorKey.initialize());
 ATH_CHECK(m_neutralfePhotonWriteDecorKey.initialize());
 ATH_CHECK(m_chargedfePhotonWriteDecorKey.initialize());

 //Init ReadHandleKeys
 ATH_CHECK(m_photonReadHandleKey.initialize());
 ATH_CHECK(m_electronReadHandleKey.initialize());
 ATH_CHECK(m_chargedfeReadHandleKey.initialize());
 ATH_CHECK(m_neutralfeReadHandleKey.initialize());

 ATH_MSG_DEBUG("Initialization completed successfully");   

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
  SG::WriteDecorHandle<xAOD::ElectronContainer, std::vector<FlowElementLink_t> > electronNeutralFEWriteDecorHandle (m_electronNeutralFEWriteDecorKey,ctx);   
  SG::WriteDecorHandle<xAOD::ElectronContainer, std::vector<FlowElementLink_t> > electronChargedFEWriteDecorHandle (m_electronChargedFEWriteDecorKey,ctx);   
  SG::WriteDecorHandle<xAOD::FlowElementContainer, std::vector<ElectronLink_t> > neutralfeElectronWriteDecorHandle (m_neutralfeElectronWriteDecorKey,ctx);   
  SG::WriteDecorHandle<xAOD::FlowElementContainer, std::vector<ElectronLink_t> > chargedfeElectronWriteDecorHandle (m_chargedfeElectronWriteDecorKey,ctx); 

  //Photon Write Handle
  SG::WriteDecorHandle<xAOD::PhotonContainer, std::vector<FlowElementLink_t> > photonNeutralFEWriteDecorHandle (m_photonNeutralFEWriteDecorKey,ctx);
  SG::WriteDecorHandle<xAOD::PhotonContainer, std::vector<FlowElementLink_t> > photonChargedFEWriteDecorHandle (m_photonChargedFEWriteDecorKey,ctx);
  SG::WriteDecorHandle<xAOD::FlowElementContainer, std::vector<PhotonLink_t> > neutralfePhotonWriteDecorHandle (m_neutralfePhotonWriteDecorKey,ctx);
  SG::WriteDecorHandle<xAOD::FlowElementContainer, std::vector<PhotonLink_t> > chargedfePhotonWriteDecorHandle (m_chargedfePhotonWriteDecorKey,ctx);  
  
  
  // This is the READ handles (so the input containers for electron, photon, FE)
  
  SG::ReadHandle<xAOD::ElectronContainer>electronReadHandle (m_electronReadHandleKey,ctx);
  SG::ReadHandle<xAOD::PhotonContainer> photonReadHandle (m_photonReadHandleKey,ctx);
  
  // Charged and Neutral PFlow "Flow elements"
  SG::ReadHandle<xAOD::FlowElementContainer> neutralfeReadHandle (m_neutralfeReadHandleKey,ctx);   
  SG::ReadHandle<xAOD::FlowElementContainer> chargedfeReadHandle (m_chargedfeReadHandleKey,ctx);   
  
  // now initialise some Flow element link containers
  std::vector<std::vector<FlowElementLink_t>> electronNeutralFEVec(electronReadHandle->size());
  std::vector<std::vector<FlowElementLink_t>> electronChargedFEVec(electronReadHandle->size());
  
  std::vector<std::vector<FlowElementLink_t>> photonNeutralFEVec(photonReadHandle->size());
  std::vector<std::vector<FlowElementLink_t>> photonChargedFEVec(photonReadHandle->size());
  //////////////////////////////////////////////////////////////////////////
  ////                      DO MATCHING/LINKING
  //////////////////////////////////////////////////////////////////////////
  

  ///////////////////////////
  // Loop over neutral flow elements (FEs)
  ///////////////////////////
  for (const xAOD::FlowElement* fe: *neutralfeElectronWriteDecorHandle){
    //Obtain the index of the FE calo-cluster
    size_t feClusterIndex=fe->otherObjects().at(0)->index();

    // init the linkers
    std::vector<ElectronLink_t> feElectronLinks;
    std::vector<PhotonLink_t> fePhotonLinks;
    
    //Loop over electrons:
    for (const xAOD::Electron* electron: *electronNeutralFEWriteDecorHandle){
      // get the calo clusters from the electron

      const std::vector<const xAOD::CaloCluster*> electronTopoClusters = xAOD::EgammaHelpers::getAssociatedTopoClusters(electron->caloCluster());
	
      
      for(const xAOD::CaloCluster* cluster : electronTopoClusters){
	// obtain the index of the electron seed topocluster
	size_t electronClusterIndex=cluster->index();	
	//match the indices: Cluster match between Flow Element (FE) and electron
	if(electronClusterIndex==feClusterIndex){
	  feElectronLinks.push_back( ElectronLink_t(*electronReadHandle,electron->index()) );
	  //Add Flow Element (FE) link to a vector
	  //index() is the unique index of the Flow Element in the container
	  electronNeutralFEVec.at(electron->index()).push_back(FlowElementLink_t(*neutralfeReadHandle, fe->index()) );
	}// end of matching block

      } // end loop over cluster
      
    } // end Electron loop

    // now loop over photons
    for (const xAOD::Photon* photon: *photonNeutralFEWriteDecorHandle){
      // retrieve clusters from the photon container
      const std::vector<const xAOD::CaloCluster*> photonTopoClusters = xAOD::EgammaHelpers::getAssociatedTopoClusters(photon->caloCluster());
      //loop over clusters, and do the matching
      for (const xAOD::CaloCluster* cluster: photonTopoClusters){
	//retrieve index of the cluster
	size_t photonClusterIndex=cluster->index();
	//do the matching
	if(photonClusterIndex==feClusterIndex){
	  // Add flow element (FE) links to photon
	  fePhotonLinks.push_back( PhotonLink_t(*photonReadHandle,photon->index()) );
	  //Add Flow Element (FE) link to a vector
	  //index() is the unique index of the Flow Element in the container
	  photonNeutralFEVec.at(photon->index()).push_back(FlowElementLink_t(*neutralfeReadHandle, fe->index()) );	  
	}// end of matching block
      } // end of neutral loop
      

    }// end of photon loop
    
    //Add vector of electron element links as decoration to FlowElement container
    neutralfeElectronWriteDecorHandle (*fe)=feElectronLinks;
    neutralfePhotonWriteDecorHandle (*fe)=fePhotonLinks;

  } // end neutral FE loop

  ////////////////////////////////////////////////////////
  //             Loop over charged Flow Elements (FEs)
  ////////////////////////////////////////////////////////
  for (const xAOD::FlowElement* fe: *chargedfeElectronWriteDecorHandle){
    // retrieve the track from the Flow element
    size_t feTrackIndex=fe->chargedObjects().at(0)->index();
    // Initialise a vector of element links to electrons/Photons
    std::vector<ElectronLink_t> feElectronLinks;
    std::vector<PhotonLink_t> fePhotonLinks;

    //loop over electrons
    for (const xAOD::Electron* electron: *electronChargedFEWriteDecorHandle){
      //obtain the clusters
       const std::vector<const xAOD::TrackParticle*> electronTrackParticles = xAOD::EgammaHelpers::getTrackParticlesVec(electron, true, true); // useBremAssoc = true (get original non-GSF track), allParticles = true (include all track particles)
       // loop over tracks
       for (const xAOD::TrackParticle* electronTrack: electronTrackParticles){
	 size_t electronTrackIndex = electronTrack->index();

	 //link to FE if track indices match
	 if(electronTrackIndex==feTrackIndex){
	   // Add electron element link to a vector 
	   // index() is the unique index of the electron in the electron container 
	   feElectronLinks.push_back( ElectronLink_t(*electronReadHandle, electron->index()) );
	   // Add fe element link to a vector 
	   // index() is the unique index of the cFE in the cFE container 
	   electronChargedFEVec.at(electron->index()).push_back( FlowElementLink_t(*chargedfeReadHandle, fe->index()) );  

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
	if (photonTrackIndex==feTrackIndex){
	            // Add photon element link to a vector
          // index() is the unique index of the photon in the photon container
          fePhotonLinks.push_back( PhotonLink_t(*photonReadHandle, photon->index()) );
          // Add fe element link to a vector
          // index() is the unique index of the cFE in the cFE container
          photonChargedFEVec.at(photon->index()).push_back( FlowElementLink_t(*chargedfeReadHandle, fe->index()) );
	}// end of matching block

      }// end of loop on tracks
      
    }//end of loop on photons


    // Add vector of electron element links as decoration to FE container     
    chargedfeElectronWriteDecorHandle (*fe) = feElectronLinks;  
    // Add vector of photon element links as decoration to FE container
    chargedfePhotonWriteDecorHandle (*fe) = fePhotonLinks;

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

  ATH_MSG_DEBUG("Execute completed successfully");

  return StatusCode::SUCCESS;
}
