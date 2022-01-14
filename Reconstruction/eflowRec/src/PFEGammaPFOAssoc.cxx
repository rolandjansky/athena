/*  
 Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/PFEGammaPFOAssoc.h" 
#include "StoreGate/WriteDecorHandle.h" 
#include "xAODEgamma/ElectronContainer.h" 
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/Electron.h" 
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/ElectronxAODHelpers.h"
#include "xAODEgamma/EgammaxAODHelpers.h" 
#include "xAODPFlow/PFOContainer.h" 
#include "xAODPFlow/PFO.h" 

using ElectronLink_t = ElementLink<xAOD::ElectronContainer>; 
using PhotonLink_t = ElementLink<xAOD::PhotonContainer>;
using PFOLink_t = ElementLink<xAOD::PFOContainer>; 

// ============================================================= 
PFEGammaPFOAssoc::PFEGammaPFOAssoc(const std::string& name, 
		   ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator)
{   
  // Declare the decoration keys   
  declareProperty ("ElectronNeutralPFODecorKey", m_electronNeutralPFOWriteDecorKey = "Electrons.neutralpfoLinks");   
  declareProperty ("ElectronChargedPFODecorKey", m_electronChargedPFOWriteDecorKey = "Electrons.chargedpfoLinks");   
  declareProperty ("NeutralPFOElectronDecorKey", m_neutralpfoElectronWriteDecorKey = "JetETMissNeutralParticleFlowObjects.pfo_ElectronLinks");   
  declareProperty ("ChargedPFOElectronDecorKey", m_chargedpfoElectronWriteDecorKey = "JetETMissChargedParticleFlowObjects.pfo_ElectronLinks"); 

  declareProperty ("PhotonNeutralPFODecorKey", m_photonNeutralPFOWriteDecorKey = "Photons.neutralpfoLinks");
  declareProperty ("PhotonChargedPFODecorKey", m_photonChargedPFOWriteDecorKey = "Photons.chargedpfoLinks");
  declareProperty ("NeutralPFOPhotonDecorKey", m_neutralpfoPhotonWriteDecorKey = "JetETMissNeutralParticleFlowObjects.pfo_PhotonLinks");
  declareProperty ("ChargedPFOPhotonDecorKey", m_chargedpfoPhotonWriteDecorKey = "JetETMissChargedParticleFlowObjects.pfo_PhotonLinks");
} 

PFEGammaPFOAssoc::~PFEGammaPFOAssoc() {} 

// ============================================================= 
StatusCode PFEGammaPFOAssoc::initialize() {   

  ATH_MSG_DEBUG("Initializing " << name() << "...");   

  // Initialise the decoration keys   
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

// ========================================================================= 
StatusCode PFEGammaPFOAssoc::finalize() {   
  return StatusCode::SUCCESS; 
} 

// ========================================================================= 
StatusCode PFEGammaPFOAssoc::execute() {   
  
  // WriteDecorHandles for the electron, photon, and PFO containers   
  // Each electron and photon receives a decoration that is a vector of element links to PFOs - one such decoration for both cPFOs and nPFOs   
  // Similarly each PFO receives a decoration that is a vector of element links to electrons and a decoration that is a vector of element 
  // links to photons  
  SG::WriteDecorHandle<xAOD::ElectronContainer, std::vector<PFOLink_t> > electronNeutralPFOWriteDecorHandle (m_electronNeutralPFOWriteDecorKey);   
  SG::WriteDecorHandle<xAOD::ElectronContainer, std::vector<PFOLink_t> > electronChargedPFOWriteDecorHandle (m_electronChargedPFOWriteDecorKey);   
  SG::WriteDecorHandle<xAOD::PFOContainer, std::vector<ElectronLink_t> > neutralpfoElectronWriteDecorHandle (m_neutralpfoElectronWriteDecorKey);   
  SG::WriteDecorHandle<xAOD::PFOContainer, std::vector<ElectronLink_t> > chargedpfoElectronWriteDecorHandle (m_chargedpfoElectronWriteDecorKey); 

  SG::WriteDecorHandle<xAOD::PhotonContainer, std::vector<PFOLink_t> > photonNeutralPFOWriteDecorHandle (m_photonNeutralPFOWriteDecorKey);
  SG::WriteDecorHandle<xAOD::PhotonContainer, std::vector<PFOLink_t> > photonChargedPFOWriteDecorHandle (m_photonChargedPFOWriteDecorKey);
  SG::WriteDecorHandle<xAOD::PFOContainer, std::vector<PhotonLink_t> > neutralpfoPhotonWriteDecorHandle (m_neutralpfoPhotonWriteDecorKey);
  SG::WriteDecorHandle<xAOD::PFOContainer, std::vector<PhotonLink_t> > chargedpfoPhotonWriteDecorHandle (m_chargedpfoPhotonWriteDecorKey);  

  // Store the ReadHandles for electron, photon, and PFO containers   
  SG::ReadHandle<xAOD::ElectronContainer> electronReadHandle (m_electronNeutralPFOWriteDecorKey.contHandleKey());
  SG::ReadHandle<xAOD::PhotonContainer> photonReadHandle (m_photonNeutralPFOWriteDecorKey.contHandleKey());   
  SG::ReadHandle<xAOD::PFOContainer> neutralpfoReadHandle (m_neutralpfoElectronWriteDecorKey.contHandleKey());   
  SG::ReadHandle<xAOD::PFOContainer> chargedpfoReadHandle (m_chargedpfoElectronWriteDecorKey.contHandleKey());   
  
  // Containers for the PFO links to electrons and photons   
  std::vector< std::vector<PFOLink_t> > electronNeutralPFOVec (electronReadHandle->size());
  std::vector< std::vector<PFOLink_t> > electronChargedPFOVec (electronReadHandle->size());
  std::vector< std::vector<PFOLink_t> > photonNeutralPFOVec (photonReadHandle->size());
  std::vector< std::vector<PFOLink_t> > photonChargedPFOVec (photonReadHandle->size());

  ///////////////////////////
  // Loop over neutral PFOs  
  ///////////////////////////
  for (const xAOD::PFO* pfo : *neutralpfoElectronWriteDecorHandle) {     
    // Obtain the index of the PFO topo-cluster
    size_t pfoClusterIndex = pfo->cluster(0)->index();

    // Initialise a vector of element links to electrons and photons     
    std::vector<ElectronLink_t> pfoElectronLinks;     
    std::vector<PhotonLink_t> pfoPhotonLinks;

    // Loop over electrons     
    for (const xAOD::Electron* electron : *electronNeutralPFOWriteDecorHandle) {
 
      const std::vector<const xAOD::CaloCluster*> electronTopoClusters = xAOD::EgammaHelpers::getAssociatedTopoClusters(electron->caloCluster());

      for (const xAOD::CaloCluster* cluster : electronTopoClusters) {
	      // Obtain the index of the electron seed topo-cluster       
	      size_t electronClusterIndex = cluster->index();       
	
	      // Link the electron to the PFO if the cluster indices match       
	      if (electronClusterIndex == pfoClusterIndex) { 
	        // Add electron element link to a vector          
	        // index() is the unique index of the electron in the electron container 
	        pfoElectronLinks.emplace_back(*electronReadHandle, electron->index() ); 
	        // Add pfo element link to a vector 
	        // index() is the unique index of the nPFO in the nPFO container 
	        electronNeutralPFOVec.at(electron->index()).push_back( PFOLink_t(*neutralpfoReadHandle, pfo->index()) );       
	      }     
      }     
    }

    // Loop over photons
    for (const xAOD::Photon* photon : *photonNeutralPFOWriteDecorHandle) {

      const std::vector<const xAOD::CaloCluster*> photonTopoClusters = xAOD::EgammaHelpers::getAssociatedTopoClusters(photon->caloCluster());

      for (const xAOD::CaloCluster* cluster : photonTopoClusters) {
        // Obtain the index of the photon seed topo-cluster
        size_t photonClusterIndex = cluster->index();

        // Link the photon to the PFO if the cluster indices match
        if (photonClusterIndex == pfoClusterIndex) {
          // Add photon element link to a vector
          // index() is the unique index of the nPFO in the nPFO container
          pfoPhotonLinks.emplace_back(*photonReadHandle, photon->index() );
          // Add pfo element link to a vector
          // index() is the unique index of the nPFO in the nPFO container
          photonNeutralPFOVec.at(photon->index()).push_back( PFOLink_t(*neutralpfoReadHandle, pfo->index()) );
        }
      }
    }

    // Add vector of electron element links as decoration to PFO container     
    neutralpfoElectronWriteDecorHandle (*pfo) = pfoElectronLinks;  
    // Add vector of photon element links as decoration to PFO container
    neutralpfoPhotonWriteDecorHandle (*pfo) = pfoPhotonLinks;
  }   

  ///////////////////////////
  // Loop over charged PFOs  
  ///////////////////////////
  for (const xAOD::PFO* pfo : *chargedpfoElectronWriteDecorHandle) {     
    size_t pfoTrackIndex = pfo->track(0)->index();     

    // Initialise a vector of element links to electrons     
    std::vector<ElectronLink_t> pfoElectronLinks;  
    // Initialise a vector of element links to photons
    std::vector<PhotonLink_t> pfoPhotonLinks;   

    // Loop over electrons     
    for (const xAOD::Electron* electron : *electronChargedPFOWriteDecorHandle) {  
      // Obtain the index of the electron original track       
      const std::vector<const xAOD::TrackParticle*> electronTrackParticles = xAOD::EgammaHelpers::getTrackParticlesVec(electron, true, true); // useBremAssoc = true (get original non-GSF track), allParticles = true (include all track particles)
      
      for (const xAOD::TrackParticle* electronTrack : electronTrackParticles) {

	      size_t electronTrackIndex = electronTrack->index();             

	      // Link the electron to the PFO if the track indices match       
	      if (electronTrackIndex == pfoTrackIndex) { 
	        // Add electron element link to a vector 
	        // index() is the unique index of the electron in the electron container 
	        pfoElectronLinks.emplace_back(*electronReadHandle, electron->index() ); 
	        // Add pfo element link to a vector 
	        // index() is the unique index of the cPFO in the cPFO container 
	        electronChargedPFOVec.at(electron->index()).push_back( PFOLink_t(*chargedpfoReadHandle, pfo->index()) );      
	      }    
      }     
    }

    // Loop over photons
    for (const xAOD::Photon* photon : *photonChargedPFOWriteDecorHandle) {
      // Obtain the indices of the converted photon's original tracks
      const std::set<const xAOD::TrackParticle*> photonTrackParticles = xAOD::EgammaHelpers::getTrackParticles(photon, true);

      for (const xAOD::TrackParticle* photonTrack : photonTrackParticles) {

        size_t photonTrackIndex = photonTrack->index();

        // Link the photon to the PFO if the track indices match
        if (photonTrackIndex == pfoTrackIndex) {
          // Add photon element link to a vector
          // index() is the unique index of the photon in the photon container
          pfoPhotonLinks.emplace_back(*photonReadHandle, photon->index() );
          // Add pfo element link to a vector
          // index() is the unique index of the cPFO in the cPFO container
          photonChargedPFOVec.at(photon->index()).push_back( PFOLink_t(*chargedpfoReadHandle, pfo->index()) );
        }
      }
    }

    // Add vector of electron element links as decoration to PFO container     
    chargedpfoElectronWriteDecorHandle (*pfo) = pfoElectronLinks;  
    // Add vector of photon element links as decoration to PFO container
    chargedpfoPhotonWriteDecorHandle (*pfo) = pfoPhotonLinks;
  }   

  // Add the vectors of pfo element links as decorations to the electron container   
  for (const xAOD::Electron* electron : *electronNeutralPFOWriteDecorHandle) {     
    electronNeutralPFOWriteDecorHandle (*electron) = electronNeutralPFOVec.at(electron->index()); 
    electronChargedPFOWriteDecorHandle (*electron) = electronChargedPFOVec.at(electron->index()); 
  }   

  // Add the vectors of pfo element links as decorations to the photon container
  for (const xAOD::Photon* photon : *photonNeutralPFOWriteDecorHandle) {
    photonNeutralPFOWriteDecorHandle (*photon) = photonNeutralPFOVec.at(photon->index());
    photonChargedPFOWriteDecorHandle (*photon) = photonChargedPFOVec.at(photon->index());
  }

  ATH_MSG_DEBUG("Execute completed successfully");   
  
  return StatusCode::SUCCESS;
}

