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
#include "xAODPFlow/PFOContainer.h" 
#include "xAODPFlow/PFO.h" 

typedef ElementLink<xAOD::ElectronContainer> ElectronLink_t; 
typedef ElementLink<xAOD::PhotonContainer> PhotonLink_t;
typedef ElementLink<xAOD::PFOContainer> PFOLink_t; 


NewEDM_PFEGamRecoAssoc::NewEDM_PFEGamRecoAssoc(
const std::string& name,
  ISvcLocator* pSvcLocator
  ):
  AthAlgorithm(name,pSvcLocator)
{
// Class initializer 
declareProperty ("ElectronNeutralPFODecorKey", m_electronNeutralPFOWriteDecorKey = "Electrons.neutralpfoLinks");   
declareProperty ("ElectronChargedPFODecorKey", m_electronChargedPFOWriteDecorKey = "Electrons.chargedpfoLinks");   
declareProperty ("NeutralPFOElectronDecorKey", m_neutralpfoElectronWriteDecorKey = "JetETMissNeutralParticleFlowObjects.pfo_ElectronLinks");   
declareProperty ("ChargedPFOElectronDecorKey", m_chargedpfoElectronWriteDecorKey = "JetETMissChargedParticleFlowObjects.pfo_ElectronLinks"); 

declareProperty ("PhotonNeutralPFODecorKey", m_photonNeutralPFOWriteDecorKey = "Photons.neutralpfoLinks");
declareProperty ("PhotonChargedPFODecorKey", m_photonChargedPFOWriteDecorKey = "Photons.chargedpfoLinks");
declareProperty ("NeutralPFOPhotonDecorKey", m_neutralpfoPhotonWriteDecorKey = "JetETMissNeutralParticleFlowObjects.pfo_PhotonLinks");
declareProperty ("ChargedPFOPhotonDecorKey", m_chargedpfoPhotonWriteDecorKey = "JetETMissChargedParticleFlowObjects.pfo_PhotonLinks");
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
StatusCode NewEDM_PFEGamRecoAssoc::execute(){


return StatusCode::SUCCESS;
}
