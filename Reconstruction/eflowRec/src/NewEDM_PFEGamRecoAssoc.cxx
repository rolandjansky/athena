/*  
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "eflowRec/NewEDM_PFEGamRecoAssoc.h" 
#include "StoreGate/WriteDecorHandle.h" 
#include "xAODEgamma/ElectronContainer.h" 
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/Electron.h" 
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/ElectronxAODHelpers.h"
#include "xAODEgamma/EgammaxAODHelpers.h" 
// keep these for the moment, although we expect to NOT need them
#include "xAODPFlow/PFOContainer.h" 
#include "xAODPFlow/PFO.h" 
// this is the new EDM to replace the above
#include "xAODPFlow/FlowElementContainer.h"
#include "xAODPFlow/FlowElement.h"

#include <typeinfo> // temp debug for type checks on objects

typedef ElementLink<xAOD::ElectronContainer> ElectronLink_t; 
typedef ElementLink<xAOD::PhotonContainer> PhotonLink_t;
typedef ElementLink<xAOD::FlowElementContainer> FlowElementLink_t; 

NewEDM_PFEGamRecoAssoc::NewEDM_PFEGamRecoAssoc(
const std::string& name,
  ISvcLocator* pSvcLocator
  ):
  AthAlgorithm(name,pSvcLocator)
{
// Class initializer 
declareProperty ("ElectronNeutralPFODecorKey", m_electronNeutralPFOWriteDecorKey = "Electrons.neutralfeLinks");   
declareProperty ("ElectronChargedPFODecorKey", m_electronChargedPFOWriteDecorKey = "Electrons.chargedfeLinks");   

declareProperty ("NeutralPFOElectronDecorKey", m_neutralpfoElectronWriteDecorKey = "JetETMissNeutralFlowElements.fe_ElectronLinks");   

declareProperty ("ChargedPFOElectronDecorKey", m_chargedpfoElectronWriteDecorKey = "JetETMissChargedFlowElements.fe_ElectronLinks"); 

declareProperty ("PhotonNeutralPFODecorKey", m_photonNeutralPFOWriteDecorKey = "Photons.neutralfeLinks");
declareProperty ("PhotonChargedPFODecorKey", m_photonChargedPFOWriteDecorKey = "Photons.chargedfeLinks");
declareProperty ("NeutralPFOPhotonDecorKey", m_neutralpfoPhotonWriteDecorKey = "JetETMissNeutralFlowElements.fe_PhotonLinks");
declareProperty ("ChargedPFOPhotonDecorKey", m_chargedpfoPhotonWriteDecorKey = "JetETMissChargedFlowElements.fe_PhotonLinks");
}

// Class destructor 
NewEDM_PFEGamRecoAssoc::~NewEDM_PFEGamRecoAssoc(){}

StatusCode NewEDM_PFEGamRecoAssoc::initialize()
{

ATH_MSG_DEBUG("Initializing "<<name() << "...");
ATH_CHECK(m_electronNeutralPFOWriteDecorKey.initialize());   
ATH_CHECK(m_electronChargedPFOWriteDecorKey.initialize());   
ATH_CHECK(m_neutralpfoElectronWriteDecorKey.initialize());   
ATH_CHECK(m_chargedpfoElectronWriteDecorKey.initialize());  

ATH_CHECK(m_photonNeutralPFOWriteDecorKey.initialize());
ATH_CHECK(m_photonChargedPFOWriteDecorKey.initialize());
ATH_CHECK(m_neutralpfoPhotonWriteDecorKey.initialize());
ATH_CHECK(m_chargedpfoPhotonWriteDecorKey.initialize()); 

 ATH_MSG_DEBUG("Initialization completed successfully");   

return StatusCode::SUCCESS;
}

StatusCode NewEDM_PFEGamRecoAssoc::finalize(){
return StatusCode::SUCCESS;
}
StatusCode NewEDM_PFEGamRecoAssoc::execute()
{
  // This algorithm does the following:
  // 1) Read the Input containers for Flow Elements, Electrons and Photons
  // 2) Match the Clusters in the flow elements to the relevant E/Photon clusters (Neutral) or match tracks to relevant Electron/Photon tracks (Charged)
  // 3) Link them
  // 4) output the Electron/Photon containers with the linkers to the Flow element containers

  // write decoration handles for the electron, photon and PFO containers -- these are the OUTPUT handles
  //Electron Write Handle
  SG::WriteDecorHandle<xAOD::ElectronContainer, std::vector<FlowElementLink_t> > electronNeutralPFOWriteDecorHandle (m_electronNeutralPFOWriteDecorKey);   
  SG::WriteDecorHandle<xAOD::ElectronContainer, std::vector<FlowElementLink_t> > electronChargedPFOWriteDecorHandle (m_electronChargedPFOWriteDecorKey);   
  SG::WriteDecorHandle<xAOD::FlowElementContainer, std::vector<ElectronLink_t> > neutralpfoElectronWriteDecorHandle (m_neutralpfoElectronWriteDecorKey);   
  SG::WriteDecorHandle<xAOD::FlowElementContainer, std::vector<ElectronLink_t> > chargedpfoElectronWriteDecorHandle (m_chargedpfoElectronWriteDecorKey); 

  //Photon Write Handle
  SG::WriteDecorHandle<xAOD::PhotonContainer, std::vector<FlowElementLink_t> > photonNeutralPFOWriteDecorHandle (m_photonNeutralPFOWriteDecorKey);
  SG::WriteDecorHandle<xAOD::PhotonContainer, std::vector<FlowElementLink_t> > photonChargedPFOWriteDecorHandle (m_photonChargedPFOWriteDecorKey);
  SG::WriteDecorHandle<xAOD::FlowElementContainer, std::vector<PhotonLink_t> > neutralpfoPhotonWriteDecorHandle (m_neutralpfoPhotonWriteDecorKey);
  SG::WriteDecorHandle<xAOD::FlowElementContainer, std::vector<PhotonLink_t> > chargedpfoPhotonWriteDecorHandle (m_chargedpfoPhotonWriteDecorKey);  
  
  
  // This is the READ handles (so the input containers for electron, photon, PFO)
  
  SG::ReadHandle<xAOD::ElectronContainer>electronReadHandle (m_electronNeutralPFOWriteDecorKey.contHandleKey());
  SG::ReadHandle<xAOD::PhotonContainer> photonReadHandle (m_photonNeutralPFOWriteDecorKey.contHandleKey());
  
  // Charged and Neutral PFlow "Flow elements"
  SG::ReadHandle<xAOD::FlowElementContainer> neutralpfoReadHandle (m_neutralpfoElectronWriteDecorKey.contHandleKey());   
  SG::ReadHandle<xAOD::FlowElementContainer> chargedpfoReadHandle (m_chargedpfoElectronWriteDecorKey.contHandleKey());   
  
  // now initialise some Flow element link containers
  std::vector<std::vector<FlowElementLink_t>> electronNeutralPFOVec(electronReadHandle->size());
  std::vector<std::vector<FlowElementLink_t>> electronChargedPFOVec(electronReadHandle->size());
  
  std::vector<std::vector<FlowElementLink_t>> photonNeutralPFOVec(photonReadHandle->size());
  std::vector<std::vector<FlowElementLink_t>> photonChargedPFOVec(photonReadHandle->size());
  //////////////////////////////////////////////////////////////////////////
  ////                      DO MATCHING/LINKING
  //////////////////////////////////////////////////////////////////////////
  

  ///////////////////////////
  // Loop over neutral flow elements (PFOs)
  ///////////////////////////
  for (const xAOD::FlowElement* pfo: *neutralpfoElectronWriteDecorHandle){
    //Obtain the index of the PFO calo-cluster
    size_t pfoClusterIndex=pfo->otherObjects().at(0)->index();

    // init the linkers
    std::vector<ElectronLink_t> pfoElectronLinks;
    std::vector<PhotonLink_t> pfoPhotonLinks;
    
    //Loop over electrons:
    for (const xAOD::Electron* electron: *electronNeutralPFOWriteDecorHandle){
      // get the calo clusters from the electron

      const std::vector<const xAOD::CaloCluster*> electronTopoClusters = xAOD::EgammaHelpers::getAssociatedTopoClusters(electron->caloCluster());
	
      
      for(const xAOD::CaloCluster* cluster : electronTopoClusters){
	// obtain the index of the electron seed topocluster
	size_t electronClusterIndex=cluster->index();	
	//match the indices: Cluster match between Flow Element (PFO) and electron
	if(electronClusterIndex==pfoClusterIndex){
	  pfoElectronLinks.push_back( ElectronLink_t(*electronReadHandle,electron->index()) );
	  //Add Flow Element (PFO) link to a vector
	  //index() is the unique index of the Flow Element in the container
	  electronNeutralPFOVec.at(electron->index()).push_back(FlowElementLink_t(*neutralpfoReadHandle, pfo->index()) );
	}// end of matching block

      } // end loop over cluster
      
    } // end Electron loop

    // now loop over photons
    for (const xAOD::Photon* photon: *photonNeutralPFOWriteDecorHandle){
      // retrieve clusters from the photon container
      const std::vector<const xAOD::CaloCluster*> photonTopoClusters = xAOD::EgammaHelpers::getAssociatedTopoClusters(photon->caloCluster());
      //loop over clusters, and do the matching
      for (const xAOD::CaloCluster* cluster: photonTopoClusters){
	//retrieve index of the cluster
	size_t photonClusterIndex=cluster->index();
	//do the matching
	if(photonClusterIndex==pfoClusterIndex){
	  // Add flow element (PFO) links to photon
	  pfoPhotonLinks.push_back( PhotonLink_t(*photonReadHandle,photon->index()) );
	  //Add Flow Element (PFO) link to a vector
	  //index() is the unique index of the Flow Element in the container
	  photonNeutralPFOVec.at(photon->index()).push_back(FlowElementLink_t(*neutralpfoReadHandle, pfo->index()) );	  
	}// end of matching block
      } // end of neutral loop
      

    }// end of photon loop
    
    //Add vector of electron element links as decoration to FlowElement container
    neutralpfoElectronWriteDecorHandle (*pfo)=pfoElectronLinks;
    neutralpfoPhotonWriteDecorHandle (*pfo)=pfoPhotonLinks;

  } // end neutral PFO loop

  ////////////////////////////////////////////////////////
  //             Loop over charged Flow Elements (PFOs)
  ////////////////////////////////////////////////////////
  for (const xAOD::FlowElement* pfo: *chargedpfoElectronWriteDecorHandle){
    // retrieve the track from the Flow element
    size_t pfoTrackIndex=pfo->chargedObjects().at(0)->index();
    // Initialise a vector of element links to electrons/Photons
    std::vector<ElectronLink_t> pfoElectronLinks;
    std::vector<PhotonLink_t> pfoPhotonLinks;

    //loop over electrons
    for (const xAOD::Electron* electron: *electronChargedPFOWriteDecorHandle){
      //obtain the clusters
       const std::vector<const xAOD::TrackParticle*> electronTrackParticles = xAOD::EgammaHelpers::getTrackParticlesVec(electron, true, true); // useBremAssoc = true (get original non-GSF track), allParticles = true (include all track particles)
       // loop over tracks
       for (const xAOD::TrackParticle* electronTrack: electronTrackParticles){
	 size_t electronTrackIndex = electronTrack->index();

	 //link to PFO if track indices match
	 if(electronTrackIndex==pfoTrackIndex){
	   // Add electron element link to a vector 
	   // index() is the unique index of the electron in the electron container 
	   pfoElectronLinks.push_back( ElectronLink_t(*electronReadHandle, electron->index()) );
	   // Add pfo element link to a vector 
	   // index() is the unique index of the cPFO in the cPFO container 
	   electronChargedPFOVec.at(electron->index()).push_back( FlowElementLink_t(*chargedpfoReadHandle, pfo->index()) );  

	 }//end of matching block
	 
       }//end of loop on clusters
    } // end of loop on electrons
    
    for(const xAOD::Photon* photon: *photonChargedPFOWriteDecorHandle){
      //obtain indices of the converted photon's original tracks
      const std::set<const xAOD::TrackParticle*> photonTrackParticles = xAOD::EgammaHelpers::getTrackParticles(photon, true);
      // loop over the tracks
      for (const xAOD::TrackParticle* photonTrack: photonTrackParticles){
	size_t photonTrackIndex=photonTrack->index();
	
	// Link the photon to the Flow Element (PFO) if the track indices match
	if (photonTrackIndex==pfoTrackIndex){
	            // Add photon element link to a vector
          // index() is the unique index of the photon in the photon container
          pfoPhotonLinks.push_back( PhotonLink_t(*photonReadHandle, photon->index()) );
          // Add pfo element link to a vector
          // index() is the unique index of the cPFO in the cPFO container
          photonChargedPFOVec.at(photon->index()).push_back( FlowElementLink_t(*chargedpfoReadHandle, pfo->index()) );
	}// end of matching block

      }// end of loop on tracks
      
    }//end of loop on photons


    // Add vector of electron element links as decoration to PFO container     
    chargedpfoElectronWriteDecorHandle (*pfo) = pfoElectronLinks;  
    // Add vector of photon element links as decoration to PFO container
    chargedpfoPhotonWriteDecorHandle (*pfo) = pfoPhotonLinks;

  } // end of charged PFO loop
  


  ////////////////////////////////////////////////////
  //   WRITE OUTPUT: ADD HANDLES TO EL/PHOT CONTAINERS
  ////////////////////////////////////////////////////
  // Add the vectors of the Flow Element (PFO) Links as decorations to the electron container
  for (const xAOD::Electron* electron : *electronNeutralPFOWriteDecorHandle){
    electronNeutralPFOWriteDecorHandle (*electron) = electronNeutralPFOVec.at(electron->index());
    electronChargedPFOWriteDecorHandle (*electron) = electronChargedPFOVec.at(electron->index());
  } //end of photon loop
  // Add the vectors of the Flow Element (PFO) Links as decorations to the photon container
  for (const xAOD::Photon* photon: *photonNeutralPFOWriteDecorHandle){
    photonNeutralPFOWriteDecorHandle (*photon) = photonNeutralPFOVec.at(photon->index());
    photonChargedPFOWriteDecorHandle (*photon) = photonChargedPFOVec.at(photon->index());
  } // end of loop on photons

  ATH_MSG_DEBUG("Execute completed successfully");

  return StatusCode::SUCCESS;
}
