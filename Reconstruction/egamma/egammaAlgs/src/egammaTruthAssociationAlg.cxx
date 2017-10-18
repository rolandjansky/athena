/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "egammaTruthAssociationAlg.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/WriteDecorHandle.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODTruth/xAODTruthHelpers.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "CxxUtils/make_unique.h"

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
  ATH_CHECK(m_electronDecKeys.initializeDecorKeys(m_electronContainerName));
  ATH_CHECK(m_photonDecKeys.initializeDecorKeys(m_photonContainerName));
  if (m_matchClusters) {
    ATH_CHECK(m_clusterDecKeys.initializeDecorKeys(m_clusterContainerName));
  }
  if (m_matchForwardElectrons){
    ATH_CHECK(m_fwdElectronDecKeys.initializeDecorKeys(m_fwdElectronContainerName));
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
// ==========================================================================   
template<class T, class L> 
StatusCode egammaTruthAssociationAlg::match(const xAOD::TruthParticleContainer& truthParticles,
					    const egammaTruthAssociationAlg::writeDecorHandleKeys<T>& hkeys,
					    const SG::AuxElement::Accessor<L>& linkAccess,
					    xAOD::TruthParticleContainer& egammaTruthContainer) {

  writeDecorHandles<T> decoHandles(hkeys);

  for (auto particle : *decoHandles.readHandle()){
    MCTruthInfo_t info = particleTruthClassifier(particle);
    const xAOD::TruthParticle* truthParticle = info.genPart;
    if (truthParticle) {
      ElementLink<xAOD::TruthParticleContainer> link(truthParticle, truthParticles);
      ATH_MSG_DEBUG("Decorating object with link to truth, index = " << link.index());
      decoHandles.el(*particle) = link;
    } else {
      decoHandles.el(*particle) = ElementLink<xAOD::TruthParticleContainer>();
    }
    decoHandles.el(*particle).toPersistent();
    ATH_MSG_DEBUG("truthType = " << info.first << " truthOrigin = " << info.second);
    decoHandles.type(*particle) = static_cast<int>( info.first );
    decoHandles.origin(*particle) = static_cast<int>( info.second );

    // Decorate the corresponding truth particle with the link to the reco
    if (m_doEgammaTruthContainer) {
      const xAOD::TruthParticle *truth = xAOD::TruthHelpers::getTruthParticle(*particle);
      if (truth) {
	xAOD::TruthParticle *truthEgamma = getEgammaTruthParticle(truth, egammaTruthContainer);
	if (truthEgamma) {
	  // we found a truthEgamma object we should annotate if this is the best link
	  bool annotateLink = true; // by default we annotate
	  const auto link = linkAccess(*truthEgamma); // what already exists
	  if (link.isValid()) {
	    auto oldPart = *link;
	    if (oldPart && truthEgamma->e() > 0 &&
		std::abs( oldPart->e()/truthEgamma->e()-1 ) < std::abs( particle->e()/truthEgamma->e()-1 ) ){
	      ATH_MSG_DEBUG(truthEgamma << ": " << " already set to a better matched particle: " << particle);
	      annotateLink = false;
	    }
	  }

	  if (annotateLink) {
	    L link(particle, *decoHandles.readHandle());
	    linkAccess(*truthEgamma) = link;
	    linkAccess(*truthEgamma).toPersistent(); 
	  }
	}
      }
    }
  }
  return StatusCode::SUCCESS;
}

// ==========================================================================
// not a constructor
template<class T> 
StatusCode 
egammaTruthAssociationAlg::writeDecorHandleKeys<T>::initializeDecorKeys(const std::string &name)
{
  keys[0] = name + ".truthParticleLink";
  keys[1] = name + ".truthType";
  keys[2] = name + ".truthOrigin";

  for (auto& key : keys) {
    ATH_CHECK(key.initialize());
  }
  return StatusCode::SUCCESS;
}

// ==========================================================================   
// constructor
template<class T> 
egammaTruthAssociationAlg::writeDecorHandles<T>::writeDecorHandles(const egammaTruthAssociationAlg::writeDecorHandleKeys<T>& hkeys) :
  el(hkeys.keys[0]),
  type(hkeys.keys[1]),
  origin(hkeys.keys[2])
{
}


// ==========================================================================   
template<class T> egammaTruthAssociationAlg::MCTruthInfo_t 
egammaTruthAssociationAlg::particleTruthClassifier(const T* particle) {
  MCTruthInfo_t info;
#ifdef MCTRUTHCLASSIFIER_CONST
  IMCTruthClassifier::Info mcinfo;
  auto ret = m_mcTruthClassifier->particleTruthClassifier(particle, &mcinfo);
  info.genPart = mcinfo.genPart;
#else
  auto ret = m_mcTruthClassifier->particleTruthClassifier(particle);
  info.genPart = m_mcTruthClassifier->getGenPart();
#endif
  info.first = ret.first;
  info.second = ret.second;
  return info;
}
/** Template specialisation for electrons: 
 * second pass based on the cluster to find true photons **/
template<> egammaTruthAssociationAlg::MCTruthInfo_t 
egammaTruthAssociationAlg::particleTruthClassifier<xAOD::Electron>(const xAOD::Electron* electron) {
  MCTruthInfo_t info;
#ifdef MCTRUTHCLASSIFIER_CONST
  IMCTruthClassifier::Info mcinfo;
  auto ret = m_mcTruthClassifier->particleTruthClassifier(electron, &mcinfo);
#else
  auto ret = m_mcTruthClassifier->particleTruthClassifier(electron);
#endif
  if (ret.first == MCTruthPartClassifier::Unknown &&
      !xAOD::EgammaHelpers::isFwdElectron(electron) && electron->caloCluster()){
    ATH_MSG_DEBUG("Trying cluster-based truth classification for electron");
#ifdef MCTRUTHCLASSIFIER_CONST
    ret = m_mcTruthClassifier->particleTruthClassifier( electron->caloCluster(), &mcinfo);
#else
    ret = m_mcTruthClassifier->particleTruthClassifier( electron->caloCluster() );
#endif
  }
#ifdef MCTRUTHCLASSIFIER_CONST
  info.genPart = mcinfo.genPart;
#else
  info.genPart = m_mcTruthClassifier->getGenPart();
#endif
  info.first = ret.first;
  info.second = ret.second;
  return info;
}

