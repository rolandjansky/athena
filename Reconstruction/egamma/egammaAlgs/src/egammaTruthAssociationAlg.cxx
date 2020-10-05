/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaTruthAssociationAlg.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteDecorHandle.h"
#include "StoreGate/WriteHandle.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"

#include <memory>

using TruthLink_t = ElementLink<xAOD::TruthParticleContainer>;
using ClusterLink_t = ElementLink<xAOD::CaloClusterContainer>;
using ElectronLink_t = ElementLink<xAOD::ElectronContainer>;
using PhotonLink_t = ElementLink<xAOD::PhotonContainer>;

egammaTruthAssociationAlg::egammaTruthAssociationAlg(const std::string& name,
                                                     ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{}

egammaTruthAssociationAlg::~egammaTruthAssociationAlg() {}

StatusCode
egammaTruthAssociationAlg::initialize()
{

  ATH_MSG_DEBUG("Initializing " << name() << "...");

  // initialize the data handles
  ATH_CHECK(m_truthEventContainerKey.initialize(m_doEgammaTruthContainer));
  ATH_CHECK(m_egammaTruthParticleContainerKey.initialize(m_doEgammaTruthContainer));
  ATH_CHECK(m_truthParticleContainerKey.initialize());

  // Now the standard decoration handles
  if (m_matchElectrons) {
    ATH_CHECK(initializeDecorKeys(m_electronDecKeys, m_electronDecName));
  } else {
    m_electronDecKeys.clear();
  }

  if (m_matchPhotons) {
    ATH_CHECK(initializeDecorKeys(m_photonDecKeys, m_photonDecName));
  } else {
    m_photonDecKeys.clear();
  }

  if (m_matchClusters) {
    ATH_CHECK(initializeDecorKeys(m_clusterDecKeys, m_clusterDecName));
  } else {
    m_clusterDecKeys.clear();
  }

  if (m_matchForwardElectrons) {
    ATH_CHECK(initializeDecorKeys(m_fwdElectronDecKeys, m_fwdElectronDecName));
  } else {
    m_fwdElectronDecKeys.clear();
  }

  CHECK(m_mcTruthClassifier.retrieve());
  ATH_MSG_DEBUG("Retrieved tool " << m_mcTruthClassifier);
  ATH_MSG_DEBUG("Initialization successful");
  return StatusCode::SUCCESS;
}

StatusCode
egammaTruthAssociationAlg::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode
egammaTruthAssociationAlg::execute_r(const EventContext& ctx) const
{

  SG::WriteHandle<xAOD::TruthParticleContainer> egammaTruthContainer;

  if (m_doEgammaTruthContainer) {

    egammaTruthContainer =
      SG::WriteHandle<xAOD::TruthParticleContainer>(m_egammaTruthParticleContainerKey, ctx);
    ATH_CHECK(egammaTruthContainer.record(std::make_unique<xAOD::TruthParticleContainer>(),
                                          std::make_unique<xAOD::TruthParticleAuxContainer>()));

    // Add a copy of electrons and photons to the truth egamma container
    SG::ReadHandle<xAOD::TruthEventContainer> truthEvtContainer(m_truthEventContainerKey, ctx);

    // only for serial running. Can remove check later
    if (!truthEvtContainer.isValid() || truthEvtContainer->empty()) {
      ATH_MSG_WARNING("Could not retrieve " << m_truthEventContainerKey.key()
                                            << " or container empty, returning");
      return StatusCode::SUCCESS;
    }

    for (const auto& truthParticleLink : truthEvtContainer->front()->truthParticleLinks()) {
      if (!truthParticleLink.isValid())
        continue;
      const xAOD::TruthParticle* truthParticle = *truthParticleLink;
      if (!isPromptEgammaParticle(truthParticle))
        continue;
      getNewTruthParticle(*egammaTruthContainer, truthParticle, truthParticleLink.getDataPtr());
    }
  }
  // Decorate containers with truth info, including links to truth particles
  // Decorate the truth particles with links to the reco ones

  // note that in multithreading this must be valid; can't just fail with success.
  SG::ReadHandle<xAOD::TruthParticleContainer> truthParticles(m_truthParticleContainerKey, ctx);

  // accessors
  static const SG::AuxElement::Accessor<ClusterLink_t> accClusLink("recoClusterLink");
  static const SG::AuxElement::Accessor<ElectronLink_t> accElLink("recoElectronLink");
  static const SG::AuxElement::Accessor<PhotonLink_t> accPhLink("recoPhotonLink");

  if (m_matchElectrons) {
    ATH_MSG_DEBUG("About to match electrons");
    ATH_CHECK(match(*truthParticles, m_electronDecKeys, accElLink, egammaTruthContainer.ptr()));
  }

  if (m_matchPhotons) {
    ATH_MSG_DEBUG("About to match photons");
    ATH_CHECK(match(*truthParticles, m_photonDecKeys, accPhLink, egammaTruthContainer.ptr()));
  }

  if (m_matchClusters) {
    ATH_MSG_DEBUG("About to match clusters");
    ATH_CHECK(match(*truthParticles, m_clusterDecKeys, accClusLink, egammaTruthContainer.ptr()));
  }

  if (m_matchForwardElectrons) {
    ATH_MSG_DEBUG("About to match fwd electrons");
    ATH_CHECK(match(*truthParticles, m_fwdElectronDecKeys, accElLink, egammaTruthContainer.ptr()));
  }

  return StatusCode::SUCCESS;
}

bool
egammaTruthAssociationAlg::isPromptEgammaParticle(const xAOD::TruthParticle* truth) const
{

  if ((truth->pdgId() != 22 && abs(truth->pdgId()) != 11) || truth->status() == 2 ||
      truth->status() == 3 || truth->barcode() > m_barcodeOffset || truth->pt() < m_minPt){
    return false;
  }

  auto type = m_mcTruthClassifier->particleTruthClassifier(truth);

  // Isolated electron or photon
  if (type.first == MCTruthPartClassifier::IsoElectron ||
      type.first == MCTruthPartClassifier::IsoPhoton) {
    return true;
  }

  // FSR photon
  if (type.first == MCTruthPartClassifier::NonIsoPhoton &&
      type.second == MCTruthPartClassifier::FSRPhot &&
      truth->pt() > m_minPtFSR) {
    return true;
  }

  return false;
}

void
egammaTruthAssociationAlg::getNewTruthParticle(
  xAOD::TruthParticleContainer& egammaTruthContainer,
  const xAOD::TruthParticle* truth,
  const xAOD::TruthParticleContainer* oldContainer) const
{
  ATH_MSG_DEBUG("In getNewTruthParticle");
  xAOD::TruthParticle* truthParticle = new xAOD::TruthParticle();
  egammaTruthContainer.push_back(truthParticle);
  truthParticle->setPdgId(truth->pdgId());
  truthParticle->setBarcode(truth->barcode());
  truthParticle->setStatus(truth->status());
  truthParticle->setPx(truth->px());
  truthParticle->setPy(truth->py());
  truthParticle->setPz(truth->pz());
  truthParticle->setE(truth->e());
  truthParticle->setM(truth->m());
  truthParticle->setProdVtxLink(truth->prodVtxLink());
  truthParticle->setDecayVtxLink(truth->decayVtxLink());

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
                << accTruthLink(*truthParticle).index());
  // MCTruthClassifier info
  auto info = m_mcTruthClassifier->particleTruthClassifier(truth);
  accType(*truthParticle) = static_cast<int>(info.first);
  accOrigin(*truthParticle) = static_cast<int>(info.second);
  ATH_MSG_DEBUG("Exiting getNewTruthParticle");
}

xAOD::TruthParticle*
egammaTruthAssociationAlg::getEgammaTruthParticle(
  const xAOD::TruthParticle* truth,
  xAOD::TruthParticleContainer& egammaTruthContainer) const
{
  if (!truth){
    return nullptr;
  }
  // Find the original truth particle for electrons from conversions
  for (unsigned int i = 0; i < 100 && truth && truth->barcode() > 200e3; ++i) {
    if (truth->prodVtx() && truth->prodVtx()->nIncomingParticles()) {
      truth = truth->prodVtx()->incomingParticle(0);
    } else {
      break;
    }
  }

  // In case truth became null in the above loop
  if (!truth){
    return nullptr;
  }
  for (auto egammaTruth : egammaTruthContainer) {
    if (truth->barcode() == egammaTruth->barcode()) {
      return egammaTruth;
    }
  }
  return nullptr;
}

//// The templated functions
template<class T>
StatusCode
egammaTruthAssociationAlg::initializeDecorKeys(SG::WriteDecorHandleKeyArray<T>& keys,
                                               const std::string& name)
{
  if (!keys.empty()) {
    ATH_MSG_FATAL("The WriteDecorHandle should not be configured directly.");
    return StatusCode::FAILURE;
  }

  keys.emplace_back(name + ".truthParticleLink");
  keys.emplace_back(name + ".truthType");
  keys.emplace_back(name + ".truthOrigin");
  ATH_CHECK(keys.initialize());
  return StatusCode::SUCCESS;
}

// constructor
template<class T>
egammaTruthAssociationAlg::writeDecorHandles<T>::writeDecorHandles(
  const SG::WriteDecorHandleKeyArray<T>& hkeys)
  : el(hkeys.at(0))
  , type(hkeys.at(1))
  , origin(hkeys.at(2))
{}

template<class T>
egammaTruthAssociationAlg::MCTruthInfo_t
egammaTruthAssociationAlg::particleTruthClassifier(const T* particle,
                                                   Cache* extrapolationCache) const
{
  MCTruthInfo_t info;
  IMCTruthClassifier::Info mcinfo;
  mcinfo.extrapolationCache = extrapolationCache;
  auto ret = m_mcTruthClassifier->particleTruthClassifier(particle, &mcinfo);
  info.genPart = mcinfo.genPart;
  info.first = ret.first;
  info.second = ret.second;
  return info;
}

/** Template specialisation for electrons:
 * second pass based on the cluster to find true photons **/
template<>
egammaTruthAssociationAlg::MCTruthInfo_t
egammaTruthAssociationAlg::particleTruthClassifier<xAOD::Electron>(const xAOD::Electron* electron,
                                                                   Cache* extrapolationCache) const
{
  MCTruthInfo_t info;
  IMCTruthClassifier::Info mcinfo;
  mcinfo.extrapolationCache = extrapolationCache;
  auto ret = m_mcTruthClassifier->particleTruthClassifier(electron, &mcinfo);
  if (ret.first == MCTruthPartClassifier::Unknown &&
      !xAOD::EgammaHelpers::isFwdElectron(electron) && electron->caloCluster()) {
    ATH_MSG_DEBUG("Trying cluster-based truth classification for electron");
    ret = m_mcTruthClassifier->particleTruthClassifier(electron->caloCluster(), &mcinfo);
  }
  info.genPart = mcinfo.genPart;
  info.first = ret.first;
  info.second = ret.second;
  return info;
}

template<class T, class L>
StatusCode
egammaTruthAssociationAlg::match(const xAOD::TruthParticleContainer& truthParticles,
                                 const SG::WriteDecorHandleKeyArray<T>& hkeys,
                                 const SG::AuxElement::Accessor<L>& linkAccess,
                                 xAOD::TruthParticleContainer* egammaTruthContainer) const
{

  writeDecorHandles<T> decoHandles(hkeys);

  // Extrapolation Cache
  Cache extrapolationCache{};

  for (auto particle : *decoHandles.readHandle()) {

    MCTruthInfo_t info = particleTruthClassifier(particle, &extrapolationCache);

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
    decoHandles.type(*particle) = static_cast<int>(info.first);
    decoHandles.origin(*particle) = static_cast<int>(info.second);

    // Decorate the corresponding truth particle with the link to the reco
    if (m_doEgammaTruthContainer) {
      if (!egammaTruthContainer) {
        ATH_MSG_ERROR("The egammaTruthContainer needs to be valid");
        return StatusCode::FAILURE;
      }
      const xAOD::TruthParticle* truth = xAOD::TruthHelpers::getTruthParticle(*particle);
      if (truth) {
        xAOD::TruthParticle* truthEgamma = getEgammaTruthParticle(truth, *egammaTruthContainer);
        if (truthEgamma) {
          // we found a truthEgamma object we should annotate if this is the best link
          bool annotateLink = true;                   // by default we annotate
          const auto link = linkAccess(*truthEgamma); // what already exists
          if (link.isValid()) {
            auto oldPart = *link;
            if (oldPart && truthEgamma->e() > 0 &&
                std::abs(oldPart->e() / truthEgamma->e() - 1) <
                  std::abs(particle->e() / truthEgamma->e() - 1)) {
              ATH_MSG_DEBUG(truthEgamma
                            << ": "
                            << " already set to a better matched particle: " << particle);
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
