/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "egammaRec/egammaTruthAssociationAlg.h"

#include "StoreGate/StoreGateSvc.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "xAODTruth/TruthParticle.h"
// #include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

#include "MCTruthClassifier/IMCTruthClassifier.h"

typedef ElementLink<xAOD::ElectronContainer> ElectronLink_t;  
typedef ElementLink<xAOD::PhotonContainer> PhotonLink_t;  
  
// =============================================================
egammaTruthAssociationAlg::egammaTruthAssociationAlg(const std::string& name, 
				   ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator),
  m_storeGate(0), m_egammaTruthContainer(0)
{
  declareProperty("ElectronContainerName", m_electronContainerName, 
    "Name of the input electron container");
  declareProperty("FwdElectronContainerName", m_fwdElectronContainerName,
    "Name of the input fwd electron container");    
  declareProperty("PhotonContainerName", m_photonContainerName, 
    "Name of the input photon container");
  declareProperty("TruthParticleContainerName", m_truthParticleContainerName, 
    "Name of the truth particle container");
  declareProperty("EgammaTruthContainerName", m_egammaTruthParticleContainerName,
    "Name of the output egamma truth particle container");
  declareProperty("CreateEgammaTruthContainer", m_doEgammaTruthContainer = true,
    "Create egammaTruthContainer ?");
  declareProperty("UseForwardElectrons", m_useForwardElectrons = true,
    "Use forward electrons?");
  declareProperty("MinPtEgammaTruth", m_minPt = 1e3, 
    "Minimum Pt to enter egamma truth particle container");
      
  declareProperty("MCTruthClassifier", m_mcTruthClassifier, 
    "Handle of MCTruthClassifier");
}

egammaTruthAssociationAlg::~egammaTruthAssociationAlg() {}

// =============================================================
StatusCode egammaTruthAssociationAlg::initialize() {

  ATH_MSG_DEBUG("Initializing " << name() << "...");

  if (service("StoreGateSvc", m_storeGate).isFailure()) 
  {
    ATH_MSG_ERROR("Unable to retrieve pointer to StoreGateSvc");
    return StatusCode::FAILURE;
  }
  
  // Retrive MCTruthClassifier
  if(m_mcTruthClassifier.retrieve().isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve " << m_mcTruthClassifier);
    return StatusCode::SUCCESS;
  } 
  else {
    ATH_MSG_DEBUG("Retrieved tool " << m_mcTruthClassifier);   
  }

  ATH_MSG_DEBUG("Initialization successful");

  return StatusCode::SUCCESS;
}

// =========================================================================
StatusCode egammaTruthAssociationAlg::finalize() {
  return StatusCode::SUCCESS;
}

// =========================================================================
StatusCode egammaTruthAssociationAlg::execute() 
{  
  
  if (m_doEgammaTruthContainer)
  {
    // Create truth egamma container
    m_egammaTruthContainer = new xAOD::TruthParticleContainer();
    CHECK( evtStore()->record( m_egammaTruthContainer, m_egammaTruthParticleContainerName ) );
    xAOD::TruthParticleAuxContainer* egammaTruthAuxContainer = new xAOD::TruthParticleAuxContainer();
    CHECK( evtStore()->record( egammaTruthAuxContainer, m_egammaTruthParticleContainerName + "Aux." ) );
    m_egammaTruthContainer->setStore( egammaTruthAuxContainer );
    ATH_MSG_DEBUG( "Recorded TruthParticleContainer with key: " << m_egammaTruthParticleContainerName );
  
    // Add a copy of electrons and photons to the truth egamma container
    const xAOD::TruthParticleContainer* truthContainer{0};
    ATH_CHECK(evtStore()->retrieve(truthContainer,m_truthParticleContainerName ));
  
    for( const auto& truth : *truthContainer )
    {
      if (!isPromptEgammaParticle(truth)) continue;
      getNewTruthParticle(truth, truthContainer, m_egammaTruthContainer);
    }
  }

  // Loop over electrons and decorate them with truth info
  // Decorate the truth particles with links to the reco ones
  ATH_MSG_DEBUG("--- Electrons ---");
  xAOD::ElectronContainer *electronContainer;
  CHECK( m_storeGate->retrieve(electronContainer, m_electronContainerName) );  
  for (auto electron : *electronContainer)
  {
    MCTruthInfo_t info = m_mcTruthClassifier->particleTruthClassifier(electron);
    if (!decorateTruth(electron, info) || 
        (m_doEgammaTruthContainer && !decorateReco(electron, electronContainer)) )
      return StatusCode::FAILURE;
  }
  
  // Loop over forward electrons and decorate them with truth info
  // Decorate the truth particles with links to the reco ones
  if (m_useForwardElectrons)
  {
    ATH_MSG_DEBUG("--- FwdElectrons ---");
    xAOD::ElectronContainer *fwdElectronContainer;
    CHECK( m_storeGate->retrieve(fwdElectronContainer, m_fwdElectronContainerName) );
    for (auto fwdElectron : *fwdElectronContainer)
    {
      MCTruthInfo_t info = m_mcTruthClassifier->particleTruthClassifier(fwdElectron);
      if (!decorateTruth(fwdElectron, info) || 
          (m_doEgammaTruthContainer && !decorateReco(fwdElectron, fwdElectronContainer)) )
        return StatusCode::FAILURE;
    }
  }

  // Loop over photons and decorate them with truth info
  ATH_MSG_DEBUG("--- Photons ---");
  xAOD::PhotonContainer *photonContainer;  
  CHECK( m_storeGate->retrieve(photonContainer, m_photonContainerName) );  
  for (auto photon : *photonContainer)
  {
    MCTruthInfo_t info = m_mcTruthClassifier->particleTruthClassifier(photon);
    if (!decorateTruth(photon, info) ||
        (m_doEgammaTruthContainer && !decorateReco(photon, photonContainer)) )
      return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

// ==========================================================================
bool egammaTruthAssociationAlg::decorateTruth(xAOD::Egamma *egamma, MCTruthInfo_t &info)
{
  const xAOD::TruthParticle *truthParticle = m_mcTruthClassifier->getGenPart();
  
  xAOD::TruthParticleContainer *truthContainer;
  if ( m_storeGate->retrieve(truthContainer, m_truthParticleContainerName).isFailure() )
  {
    ATH_MSG_WARNING("Cannot retrieve truth container " << m_truthParticleContainerName);
    return false;
  }
  
  if (truthParticle) {
    ElementLink<xAOD::TruthParticleContainer> link(truthParticle, *truthContainer);
    ATH_MSG_DEBUG("Decorating egamma object with link to truth, index = " << link.index());
    egamma->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" ) = link;
  } else {
    egamma->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" ) = ElementLink<xAOD::TruthParticleContainer>();
  }
  egamma->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" ).toPersistent();
  ATH_MSG_DEBUG("truthType = " << info.first << " truthOrigin = " << info.second);
  egamma->auxdata<int>("truthType") = static_cast<int>( info.first );
  egamma->auxdata<int>("truthOrigin") = static_cast<int>( info.second );
  
  return true;
}

// ==========================================================================
bool egammaTruthAssociationAlg::isPromptEgammaParticle(const xAOD::TruthParticle *truth)
{
  if ((truth->pdgId() != 22 && abs(truth->pdgId()) != 11) || 
      truth->status() == 2 || truth->status() == 3 ||
      truth->barcode() > 200e3 ||
      truth->pt() < m_minPt) return false;
  
  MCTruthInfo_t type = m_mcTruthClassifier->particleTruthClassifier(truth);

  // Isolated electron or photon
  if(type.first==MCTruthPartClassifier::IsoElectron ||
     type.first==MCTruthPartClassifier::IsoPhoton) return true;
  
  // FSR photon
  if (type.first == MCTruthPartClassifier::NonIsoPhoton && 
      type.second == MCTruthPartClassifier::FSRPhot) return true;
  
  // Electron from photon conversion
//   if(type.first==BkgElectron&&type.second==PhotonConv)
//   {
//     type=m_mcTruthClassifier->checkOrigOfBkgElec(m_mcTruthClassifier->getGenPart());
//     if((type.first==IsoElectron)||(type.first==NonIsoPhoton&&type.second==FSRPhot))
//       return true;
//   }
//   
//   std::vector <const xAOD::TruthParticle*> theParts = m_mcTruthClassifier->getCnvPhotTrkToTruthPart();
//   std::vector<MCTruthPartClassifier::ParticleType>   cnvType = m_mcTruthClassifier->getCnvPhotPartType();
//   std::vector<MCTruthPartClassifier::ParticleOrigin> cnvOrig = m_mcTruthClassifier->getCnvPhotPartOrig();
//   for(unsigned int i=0; i<theParts.size(); i++){
//     if(cnvType[i]==BkgElectron&&cnvOrig[i]==PhotonConv){
//       std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> bkgType=m_mcTruthClassifier->checkOrigOfBkgElec(theParts[i]);
//       if(bkgType.first==IsoPhoton){ return true; }
//   
  return false;
}
    
// ==========================================================================
void egammaTruthAssociationAlg::getNewTruthParticle(const xAOD::TruthParticle *truth, const xAOD::TruthParticleContainer *oldContainer, xAOD::TruthParticleContainer *newContainer)
{
  xAOD::TruthParticle* truthParticle = new xAOD::TruthParticle();
  newContainer->push_back( truthParticle );

  truthParticle->setPdgId(truth->pdgId());
  truthParticle->setBarcode(truth->barcode());
  truthParticle->setStatus(truth->status());
  truthParticle->setPx(truth->px());
  truthParticle->setPy(truth->py());
  truthParticle->setPz(truth->pz());
  truthParticle->setE(truth->e());
  truthParticle->setM(truth->m());
  
  xAOD::TruthParticle::Polarization pol = truth->polarization();
  truthParticle->setPolarizationParameter(pol.theta,xAOD::TruthParticle::THETA);
  truthParticle->setPolarizationParameter(pol.phi,  xAOD::TruthParticle::PHI);

  truthParticle->auxdata< ElectronLink_t >("recoElectronLink") = ElectronLink_t();
  truthParticle->auxdata< PhotonLink_t >("recoPhotonLink") = PhotonLink_t();  
  
  typedef ElementLink<xAOD::TruthParticleContainer> TruthLink_t;
  truthParticle->auxdata< TruthLink_t >("truthParticleLink") = TruthLink_t(truth, *oldContainer);
  truthParticle->auxdata< TruthLink_t >("truthParticleLink").toPersistent();
  ATH_MSG_DEBUG("Decorating truth particle with link to old truth, index = " << truthParticle->auxdata< TruthLink_t >("truthParticleLink").index() );
  
}

// ==========================================================================
bool egammaTruthAssociationAlg::decorateReco(xAOD::Electron* egamma, const xAOD::ElectronContainer* egammaContainer)
{
  const xAOD::TruthParticle *truth = xAOD::EgammaHelpers::getTruthParticle(egamma);
  if (!truth)
  {
    ATH_MSG_DEBUG("No truth particle associated to electron");
    return true;
  }
  xAOD::TruthParticle *truthEgamma = getEgammaTruthParticle(truth);
  
  if (!truthEgamma)
  {
    ATH_MSG_DEBUG("Truth particle associated to electron not in egamma truth collection");
    return true;
  }
  
  ElectronLink_t link(egamma, *egammaContainer);
  truthEgamma->auxdata<ElectronLink_t>("recoElectronLink") = link;
  truthEgamma->auxdata<ElectronLink_t>("recoElectronLink").toPersistent(); 
  ATH_MSG_DEBUG("Decorating truth egamma particle with link to electron, index = " << link.index() );
  
  return true;
}

// ==========================================================================
bool egammaTruthAssociationAlg::decorateReco(xAOD::Photon* egamma, const xAOD::PhotonContainer* egammaContainer)
{
  const xAOD::TruthParticle *truth = xAOD::EgammaHelpers::getTruthParticle(egamma);
  if (!truth)
  {  
    ATH_MSG_DEBUG("No truth particle associated to photon");
    return true;
  }
  xAOD::TruthParticle *truthEgamma = getEgammaTruthParticle(truth);
  
  if (!truthEgamma)
  {
    ATH_MSG_DEBUG("Truth particle associated to electron not in egamma truth collection");
    return true;
  }

  PhotonLink_t link(egamma, *egammaContainer);
  truthEgamma->auxdata<PhotonLink_t>("recoPhotonLink") = link;
  truthEgamma->auxdata<PhotonLink_t>("recoPhotonLink").toPersistent();
  ATH_MSG_DEBUG("Decorating truth egamma particle with link to photon, index = " << link.index() );
    
  return true;
}

// ==========================================================================
xAOD::TruthParticle* egammaTruthAssociationAlg::getEgammaTruthParticle(const xAOD::TruthParticle *truth)
{
  if (!truth) return 0;
  
  // Find the original truth particle for electrons from conversions
  for (unsigned int i = 0; i < 100 && truth && truth->barcode() > 200e3; ++i)
  {
    if (truth->prodVtx() && truth->prodVtx()->nIncomingParticles())
      truth = truth->prodVtx()->incomingParticle(0);
    else
      break;
  }
    
  for (const auto& egammaTruth : *m_egammaTruthContainer)
    if (truth->barcode() == egammaTruth->barcode()) 
      return egammaTruth;
  
  return 0;
} 
   
