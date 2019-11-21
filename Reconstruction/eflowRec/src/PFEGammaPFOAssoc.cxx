/*  
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/PFEGammaPFOAssoc.h" 
#include "StoreGate/WriteDecorHandle.h" 
#include "xAODEgamma/ElectronContainer.h" 
#include "xAODEgamma/Electron.h" 
#include "xAODEgamma/ElectronxAODHelpers.h"
#include "xAODEgamma/EgammaxAODHelpers.h" 
#include "xAODPFlow/PFOContainer.h" 
#include "xAODPFlow/PFO.h" 

typedef ElementLink<xAOD::ElectronContainer> ElectronLink_t; 
typedef ElementLink<xAOD::PFOContainer> PFOLink_t; 

// ============================================================= 
PFEGammaPFOAssoc::PFEGammaPFOAssoc(const std::string& name, 
		   ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator)
{   
  // Declare the decoration keys   
  declareProperty ("ElectronNeutralPFODecorKey", m_electronNeutralPFOWriteDecorKey = "Electrons.neutralpfoLinks");   
  declareProperty ("ElectronChargedPFODecorKey", m_electronChargedPFOWriteDecorKey = "Electrons.chargedpfoLinks");   
  declareProperty ("NeutralPFODecorKey", m_neutralpfoWriteDecorKey = "JetETMissNeutralParticleFlowObjects.pfo_ElectronLinks");   
  declareProperty ("ChargedPFODecorKey", m_chargedpfoWriteDecorKey = "JetETMissChargedParticleFlowObjects.pfo_ElectronLinks"); 
} 

PFEGammaPFOAssoc::~PFEGammaPFOAssoc() {} 

// ============================================================= 
StatusCode PFEGammaPFOAssoc::initialize() {   

  ATH_MSG_DEBUG("Initializing " << name() << "...");   

  // Initialise the decoration keys   
  ATH_CHECK(m_electronNeutralPFOWriteDecorKey.initialize());   
  ATH_CHECK(m_electronChargedPFOWriteDecorKey.initialize());   
  ATH_CHECK(m_neutralpfoWriteDecorKey.initialize());   
  ATH_CHECK(m_chargedpfoWriteDecorKey.initialize());   

  ATH_MSG_DEBUG("Initialization completed successfully");   

  return StatusCode::SUCCESS; 
} 

// ========================================================================= 
StatusCode PFEGammaPFOAssoc::finalize() {   
  return StatusCode::SUCCESS; 
} 

// ========================================================================= 
StatusCode PFEGammaPFOAssoc::execute() {   
  
  // WriteDecorHandles for the electron and PFO containers   
  // Each electron receives a decoration that is a vector of element links to PFOs - one such decoration for both cPFOs and nPFOs   
  // Similarly each PFO receives a decoration that is a vector of element links to electrons   
  SG::WriteDecorHandle<xAOD::ElectronContainer, std::vector<PFOLink_t> > electronNeutralPFOWriteDecorHandle (m_electronNeutralPFOWriteDecorKey);   
  SG::WriteDecorHandle<xAOD::ElectronContainer, std::vector<PFOLink_t> > electronChargedPFOWriteDecorHandle (m_electronChargedPFOWriteDecorKey);   
  SG::WriteDecorHandle<xAOD::PFOContainer, std::vector<ElectronLink_t> > neutralpfoWriteDecorHandle (m_neutralpfoWriteDecorKey);   
  SG::WriteDecorHandle<xAOD::PFOContainer, std::vector<ElectronLink_t> > chargedpfoWriteDecorHandle (m_chargedpfoWriteDecorKey);   

  // Store the ReadHandles for electron and PFO containers   
  SG::ReadHandle<xAOD::ElectronContainer> electronReadHandle (m_electronNeutralPFOWriteDecorKey.contHandleKey());   
  SG::ReadHandle<xAOD::PFOContainer> neutralpfoReadHandle (m_neutralpfoWriteDecorKey.contHandleKey());   
  SG::ReadHandle<xAOD::PFOContainer> chargedpfoReadHandle (m_chargedpfoWriteDecorKey.contHandleKey());   
  
  // Containers for the PFO links to electrons   
  std::vector< std::vector<PFOLink_t> > electronNeutralPFOVec (electronReadHandle->size());
  std::vector< std::vector<PFOLink_t> > electronChargedPFOVec (electronReadHandle->size());

  ///////////////////////////
  // Loop over neutral PFOs  
  ///////////////////////////
  for (const xAOD::PFO* pfo : *neutralpfoWriteDecorHandle) {     
    // Obtain the index of the PFO topo-cluster
    size_t pfoClusterIndex = pfo->cluster(0)->index();

    // Initialise a vector of element links to electrons     
    std::vector<ElectronLink_t> pfoElectronLinks;     

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
	  pfoElectronLinks.push_back( ElectronLink_t(*electronReadHandle, electron->index()) ); 
	  // Add pfo element link to a vector 
	  // index() is the unique index of the nPFO in the nPFO container 
	  electronNeutralPFOVec.at(electron->index()).push_back( PFOLink_t(*neutralpfoReadHandle, pfo->index()) );       
	}     
      }     
    }

    // Add vector of electron element links as decoration to PFO container     
    neutralpfoWriteDecorHandle (*pfo) = pfoElectronLinks;  
  }   

  ///////////////////////////
  // Loop over charged PFOs  
  ///////////////////////////
  for (const xAOD::PFO* pfo : *chargedpfoWriteDecorHandle) {     
    size_t pfoTrackIndex = pfo->track(0)->index();     

    // Initialise a vector of element links to electrons     
    std::vector<ElectronLink_t> pfoElectronLinks;     

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
	  pfoElectronLinks.push_back( ElectronLink_t(*electronReadHandle, electron->index()) ); 
	  // Add pfo element link to a vector 
	  // index() is the unique index of the cPFO in the cPFO container 
	  electronChargedPFOVec.at(electron->index()).push_back( PFOLink_t(*chargedpfoReadHandle, pfo->index()) );      
	}    
      }     
    }

    // Add vector of electron element links as decoration to PFO container     
    chargedpfoWriteDecorHandle (*pfo) = pfoElectronLinks;  
  }   

  // Add the vectors of pfo element links as decorations to the electron container   
  for (const xAOD::Electron* electron : *electronNeutralPFOWriteDecorHandle) {     
    electronNeutralPFOWriteDecorHandle (*electron) = electronNeutralPFOVec.at(electron->index()); 
    electronChargedPFOWriteDecorHandle (*electron) = electronChargedPFOVec.at(electron->index()); 
  }   

  ATH_MSG_DEBUG("Execute completed successfully");   
  
  return StatusCode::SUCCESS;
}

