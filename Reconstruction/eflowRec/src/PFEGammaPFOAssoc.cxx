/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/PFEGammaPFOAssoc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/WriteDecorHandle.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronxAODHelpers.h"
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
  declareProperty ("ElectronNeutralPFODecorKey", m_electronNeutralPFOWriteDecorKey = "Electrons.neutralpfoLink");
  declareProperty ("ElectronChargedPFODecorKey", m_electronChargedPFOWriteDecorKey = "Electrons.chargedpfoLink");
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
  
  SG::WriteDecorHandle<xAOD::ElectronContainer, PFOLink_t> electronNeutralPFOWriteDecorHandle (m_electronNeutralPFOWriteDecorKey);
  SG::WriteDecorHandle<xAOD::ElectronContainer, PFOLink_t> electronChargedPFOWriteDecorHandle (m_electronChargedPFOWriteDecorKey);
  SG::WriteDecorHandle<xAOD::PFOContainer, std::vector<ElectronLink_t> > neutralpfoWriteDecorHandle (m_neutralpfoWriteDecorKey);
  SG::WriteDecorHandle<xAOD::PFOContainer, std::vector<ElectronLink_t> > chargedpfoWriteDecorHandle (m_chargedpfoWriteDecorKey);

  SG::ReadHandle<xAOD::ElectronContainer> electronReadHandle (m_electronNeutralPFOWriteDecorKey.contHandleKey());
  SG::ReadHandle<xAOD::PFOContainer> neutralpfoReadHandle (m_neutralpfoWriteDecorKey.contHandleKey());
  SG::ReadHandle<xAOD::PFOContainer> chargedpfoReadHandle (m_chargedpfoWriteDecorKey.contHandleKey());

  PFOLink_t dummyPFOLink;

  // Initialise to the default links for electrons
  for (const xAOD::Electron* electron : *electronNeutralPFOWriteDecorHandle) {
    electronNeutralPFOWriteDecorHandle (*electron) = dummyPFOLink;
  }
  for (const xAOD::Electron* electron : *electronChargedPFOWriteDecorHandle) {
    electronChargedPFOWriteDecorHandle (*electron) = dummyPFOLink;
  }

  // Loop over neutral PFOs
  for (const xAOD::PFO* pfo : *neutralpfoWriteDecorHandle) {
    size_t pfoCaloIndex = pfo->cluster(0)->index();

    std::vector<ElectronLink_t> pfoElectronLinks;
   
    // Loop over electrons
    for (const xAOD::Electron* electron : *electronNeutralPFOWriteDecorHandle) {
      size_t electronCaloIndex = electron->caloCluster(0)->index();

      if (electronCaloIndex == pfoCaloIndex) {	
	// Add pfo->electron element links to a vector
	pfoElectronLinks.push_back( ElectronLink_t(*electronReadHandle, electron->index()) );
	// Add electron->pfo link as decoration to electron container
	electronNeutralPFOWriteDecorHandle (*electron) = PFOLink_t(*neutralpfoReadHandle, pfo->index());
      }
    }

    // Add vector of pfo->electron element links as decoration to PFO container
    neutralpfoWriteDecorHandle (*pfo) = pfoElectronLinks;
  }

  // Loop over charged PFOs
  for (const xAOD::PFO* pfo : *chargedpfoWriteDecorHandle) {
    size_t pfoTrackIndex = pfo->track(0)->index();

    std::vector<ElectronLink_t> pfoElectronLinks;

    // Loop over electrons
    for (const xAOD::Electron* electron : *electronChargedPFOWriteDecorHandle) {
      
      const xAOD::TrackParticle* electronOrigTrack = xAOD::EgammaHelpers::getOriginalTrackParticle(electron);
      size_t electronTrackIndex = electronOrigTrack->index();

      if (electronTrackIndex == pfoTrackIndex) {
	// Add pfo->electron element links to a vector
	pfoElectronLinks.push_back( ElectronLink_t(*electronReadHandle, electron->index()) );
	// Add electron->pfo link as decoration to electron container
	electronChargedPFOWriteDecorHandle (*electron) = PFOLink_t(*chargedpfoReadHandle, pfo->index());
      }
    }

    // Add vector of pfo->electron element links as decoration to PFO container
    chargedpfoWriteDecorHandle (*pfo) = pfoElectronLinks;
  }
 
  ATH_MSG_DEBUG("Execute completed successfully");

  return StatusCode::SUCCESS;
}
