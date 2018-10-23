/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#include "egammaTruthAssociationAlg.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/WriteDecorHandle.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"

#include <memory>

typedef ElementLink<xAOD::TruthParticleContainer> TruthLink_t;
typedef ElementLink<xAOD::CaloClusterContainer> ClusterLink_t;  
typedef ElementLink<xAOD::ElectronContainer> ElectronLink_t;  
typedef ElementLink<xAOD::PhotonContainer> PhotonLink_t;  

  
// =============================================================
egammaTruthAssociationAlg::egammaTruthAssociationAlg(const std::string& name, 
						     ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator)
{
}

egammaTruthAssociationAlg::~egammaTruthAssociationAlg() {}

// =============================================================
StatusCode egammaTruthAssociationAlg::initialize() {

  ATH_MSG_DEBUG("Initializing " << name() << "...");

  // initialize the data handles
  ATH_CHECK(m_truthEventContainerKey.initialize(m_doEgammaTruthContainer));
  ATH_CHECK(m_egammaTruthParticleContainerKey.initialize(m_doEgammaTruthContainer));
  ATH_CHECK(m_truthParticleContainerKey.initialize());

  // Now the standard decoration handles
  ATH_CHECK(initializeDecorKeys(m_electronDecKeys));
  ATH_CHECK(initializeDecorKeys(m_photonDecKeys));
  if (m_matchClusters) {
    ATH_CHECK(initializeDecorKeys(m_clusterDecKeys));
  } else {
    m_clusterDecKeys.clear();
  }
  if (m_matchForwardElectrons){
    ATH_CHECK(initializeDecorKeys(m_fwdElectronDecKeys));
  } else {
    m_fwdElectronDecKeys.clear();
  }

  CHECK( m_mcTruthClassifier.retrieve() );
  ATH_MSG_DEBUG("Retrieved tool " << m_mcTruthClassifier);
  ATH_MSG_DEBUG("Initialization successful");
  return StatusCode::SUCCESS;
}

// =========================================================================
StatusCode egammaTruthAssociationAlg::finalize() {
  return StatusCode::SUCCESS;
}

// =========================================================================
StatusCode egammaTruthAssociationAlg::execute() {  

  SG::WriteHandle<xAOD::TruthParticleContainer> egammaTruthContainer;

  if (m_doEgammaTruthContainer){

    egammaTruthContainer = SG::WriteHandle<xAOD::TruthParticleContainer>(m_egammaTruthParticleContainerKey);
    ATH_CHECK(egammaTruthContainer.record(std::make_unique<xAOD::TruthParticleContainer>(),
					  std::make_unique<xAOD::TruthParticleAuxContainer>()));
    

    // Add a copy of electrons and photons to the truth egamma container
    SG::ReadHandle<xAOD::TruthEventContainer> truthEvtContainer(m_truthEventContainerKey);

    //only for serial running. Can remove check later
    if (!truthEvtContainer.isValid() || !truthEvtContainer->size() ){
      ATH_MSG_WARNING("Could not retrieve " << m_truthEventContainerKey.key() << 
		      " or container empty, returning");
      return StatusCode::SUCCESS;
    }

    for (const auto& truthParticleLink : truthEvtContainer->front()->truthParticleLinks() ){
      if (!truthParticleLink.isValid()) continue;
      const xAOD::TruthParticle* truthParticle = *truthParticleLink;
      if (!isPromptEgammaParticle(truthParticle)) continue;
      getNewTruthParticle(*egammaTruthContainer, truthParticle, truthParticleLink.getDataPtr());
    }    
  }
  // Decorate containers with truth info, including links to truth particles
  // Decorate the truth particles with links to the reco ones

  // note that in multithreading this must be valid; can't just fail with success.
  SG::ReadHandle<xAOD::TruthParticleContainer> truthParticles(m_truthParticleContainerKey);

  // accessors
  static const SG::AuxElement::Accessor<ClusterLink_t> accClusLink("recoClusterLink");
  static const SG::AuxElement::Accessor<ElectronLink_t> accElLink("recoElectronLink");
  static const SG::AuxElement::Accessor<PhotonLink_t> accPhLink("recoPhotonLink");

  ATH_MSG_DEBUG("About to match electrons");
  ATH_CHECK( match(*truthParticles, m_electronDecKeys, accElLink, *egammaTruthContainer) );

  ATH_MSG_DEBUG("About to match photons");
  ATH_CHECK( match(*truthParticles, m_photonDecKeys, accPhLink,	*egammaTruthContainer) );

  if (m_matchClusters) {
    ATH_MSG_DEBUG("About to match clusters");
    ATH_CHECK( match(*truthParticles, m_clusterDecKeys,	accClusLink, *egammaTruthContainer) );
  }
  if (m_matchForwardElectrons){
    ATH_MSG_DEBUG("About to match fwd electrons");
    ATH_CHECK( match(*truthParticles, m_fwdElectronDecKeys, accElLink, *egammaTruthContainer) );
  }

  return StatusCode::SUCCESS;
}

// ==========================================================================
bool egammaTruthAssociationAlg::isPromptEgammaParticle(const xAOD::TruthParticle *truth) {

  if ((truth->pdgId() != 22 && abs(truth->pdgId()) != 11) || 
      truth->status() == 2 || truth->status() == 3 ||
      truth->barcode() > m_barcodeOffset ||
      truth->pt() < m_minPt) return false;
  
  auto type = m_mcTruthClassifier->particleTruthClassifier(truth);

  // Isolated electron or photon
  if(type.first==MCTruthPartClassifier::IsoElectron ||
     type.first==MCTruthPartClassifier::IsoPhoton) return true;
  
  // FSR photon
  if (type.first == MCTruthPartClassifier::NonIsoPhoton && 
      type.second == MCTruthPartClassifier::FSRPhot &&
      truth->pt() > m_minPtFSR) return true;
  
  return false;
}
    
// ==========================================================================
void egammaTruthAssociationAlg::getNewTruthParticle(xAOD::TruthParticleContainer& egammaTruthContainer,
						    const xAOD::TruthParticle *truth, 
						    const xAOD::TruthParticleContainer *oldContainer) {
  ATH_MSG_DEBUG("In getNewTruthParticle");
  xAOD::TruthParticle* truthParticle = new xAOD::TruthParticle();
  egammaTruthContainer.push_back( truthParticle ); 
  truthParticle->setPdgId(truth->pdgId());
  truthParticle->setBarcode(truth->barcode());
  truthParticle->setStatus(truth->status());
  truthParticle->setPx(truth->px());
  truthParticle->setPy(truth->py());
  truthParticle->setPz(truth->pz());
  truthParticle->setE(truth->e());
  truthParticle->setM(truth->m());
  truthParticle->setProdVtxLink( truth->prodVtxLink() );
  truthParticle->setDecayVtxLink( truth->decayVtxLink() );
  
  static const SG::AuxElement::Accessor<ClusterLink_t> accClusLink("recoClusterLink");
  static const SG::AuxElement::Accessor<ElectronLink_t> accElLink("recoElectronLink");
  static const SG::AuxElement::Accessor<PhotonLink_t> accPhLink("recoPhotonLink");
  static const SG::AuxElement::Accessor<TruthLink_t> accTruthLink("truthParticleLink");
  static const SG::AuxElement::Accessor<int> accType("truthType");
  static const SG::AuxElement::Accessor<int> accOrigin("truthOrigin");

  if (m_matchClusters) {
    accClusLink(*truthParticle) = ClusterLink_t();
  }
  accElLink(*truthParticle) = ElectronLink_t();
  accPhLink(*truthParticle) = PhotonLink_t();  
  accTruthLink(*truthParticle) = TruthLink_t(truth, *oldContainer);
  accTruthLink(*truthParticle).toPersistent();
  ATH_MSG_DEBUG("Decorating truth particle with link to old truth, index = " 
		<< accTruthLink(*truthParticle).index() );
  // MCTruthClassifier info
  auto info = m_mcTruthClassifier->particleTruthClassifier(truth);
  accType(*truthParticle) = static_cast<int>( info.first );
  accOrigin(*truthParticle) = static_cast<int>( info.second );
  ATH_MSG_DEBUG("Exiting getNewTruthParticle");
}

// ==========================================================================
xAOD::TruthParticle* 
egammaTruthAssociationAlg::getEgammaTruthParticle(const xAOD::TruthParticle *truth,
						  xAOD::TruthParticleContainer& egammaTruthContainer) const{
  if (!truth) return nullptr;

  // Find the original truth particle for electrons from conversions
  for (unsigned int i = 0; i < 100 && truth && truth->barcode() > 200e3; ++i){
    if (truth->prodVtx() && truth->prodVtx()->nIncomingParticles()){
      truth = truth->prodVtx()->incomingParticle(0);
    }
    else{
      break;
    }
  }

  //In case truth became null in the above loop
  if (!truth) return nullptr;

  for (auto egammaTruth : egammaTruthContainer) {    
    if (truth->barcode() == egammaTruth->barcode()) {
      return egammaTruth;
    }
  }
  return 0;
}
