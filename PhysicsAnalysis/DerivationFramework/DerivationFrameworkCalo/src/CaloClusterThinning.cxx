/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// CaloClusterThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Simone Mazza (simone.mazza@mi.infn.it)

#include "DerivationFrameworkCalo/CaloClusterThinning.h"
#include "ClustersInCone.h"

#include "GaudiKernel/ThreadLocalContext.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ThinningHandle.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"

#include <algorithm>
#include <string>
#include <vector>

// Constructor
DerivationFramework::CaloClusterThinning::CaloClusterThinning(
  const std::string& t,
  const std::string& n,
  const IInterface* p)
  : base_class(t, n, p)
  , m_ntot(0)
  , m_ntotTopo(0)
  , m_npass(0)
  , m_npassTopo(0)
  , m_run_calo(false)
  , m_run_topo(false)
  , m_selectionString("")
  , m_coneSize(-1.0)
{
  declareProperty("SelectionString", m_selectionString);
  declareProperty("ConeSize", m_coneSize);
}

// Destructor
DerivationFramework::CaloClusterThinning::~CaloClusterThinning() {}

// Athena initialize and finalize
StatusCode
DerivationFramework::CaloClusterThinning::initialize()
{
  // Decide which collections need to be checked for ID TrackParticles
  ATH_MSG_VERBOSE("initialize() ...");

  if (!m_CaloClSGKey.empty()) {
    ATH_CHECK(m_CaloClSGKey.initialize(m_streamName));
    m_run_calo = true;
    ATH_MSG_INFO("Using " << m_CaloClSGKey.key()
                          << "as the source collection for calo clusters");
  }
  if (!m_TopoClSGKey.empty()) {
    ATH_CHECK(m_TopoClSGKey.initialize(m_streamName));
    m_run_topo = true;
    ATH_MSG_INFO("Using " << m_TopoClSGKey.key()
                          << "as the source collection for topo calo clusters");
  }
  if (m_CaloClSGKey.empty() && m_TopoClSGKey.empty()) {
    ATH_MSG_FATAL(
      "No CalCaloTopoCluster or CaloCluster collection provided for thinning.");
    return StatusCode::FAILURE;
  }

  if (m_sgKey.empty()) {
    ATH_MSG_FATAL("No particle collection provided for thinning.");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO(
      "Calo clusters associated with objects in "
      << m_sgKey.key()
      << " will be retained in this format with the rest being thinned away");
    ATH_CHECK(m_sgKey.initialize());
  }

  // Set up the text-parsing machinery for selectiong the photon directly
  // according to user cuts
  if (!m_selectionString.empty()) {
    ATH_CHECK(initializeParser(m_selectionString));
  }

  return StatusCode::SUCCESS;
}

StatusCode
DerivationFramework::CaloClusterThinning::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  ATH_MSG_INFO("Processed " << m_ntot << " clusters, of which " << m_npass
                            << " were retained ");
  ATH_MSG_INFO("Processed " << m_ntotTopo << " topo clusters, of which "
                            << m_npassTopo << " were retained ");
  ATH_CHECK(finalizeParser());
  return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode
DerivationFramework::CaloClusterThinning::doThinning() const
{
  const EventContext& ctx = Gaudi::Hive::currentContext();

  bool is_muons = false;
  bool is_egamma = false;
  bool is_tau = false;

  // Retrieve egCluster collection
  SG::ReadHandle<xAOD::CaloClusterContainer> importedCaloCluster;
  if (m_run_calo) {
    importedCaloCluster =
      SG::ReadHandle<xAOD::CaloClusterContainer>(m_CaloClSGKey, ctx);
  }

  // Retrieve CalCaloTopo collection if required
  SG::ReadHandle<xAOD::CaloClusterContainer> importedTopoCaloCluster;
  if (m_run_topo) {
    importedTopoCaloCluster =
      SG::ReadHandle<xAOD::CaloClusterContainer>(m_TopoClSGKey, ctx);
  }
  // Check the event contains tracks
  unsigned int nClusters = 0;
  if (m_run_calo) {
    nClusters = importedCaloCluster->size();
  }
  unsigned int nTopoClusters = 0;
  if (m_run_topo) {
    nTopoClusters = importedTopoCaloCluster->size();
  }
  if (nClusters == 0 && nTopoClusters == 0) {
    return StatusCode::SUCCESS;
  }

  // Set up a mask with the same entries as the full TrackParticle collection(s)
  std::vector<bool> mask(nClusters, false), topomask(nTopoClusters, false);
  m_ntot += nClusters;
  m_ntotTopo += nTopoClusters;

  // Retrieve particle container
  SG::ReadHandle<xAOD::IParticleContainer> importedParticlesHandle{ m_sgKey,
                                                                    ctx };
  const xAOD::IParticleContainer* importedParticles =
    importedParticlesHandle.ptr();

  // No particles in the input
  unsigned int nEgammas(importedParticles->size());
  if (nEgammas == 0) {
    return StatusCode::SUCCESS;
  }

  is_egamma = false;
  is_muons = false;

  // Are we dealing with a compatible collection?
  const xAOD::ElectronContainer* testElectrons =
    dynamic_cast<const xAOD::ElectronContainer*>(importedParticles);
  const xAOD::PhotonContainer* testPhotons =
    dynamic_cast<const xAOD::PhotonContainer*>(importedParticles);
  if (testElectrons || testPhotons) {
    is_egamma = true;
  }
  const xAOD::MuonContainer* testMuons =
    dynamic_cast<const xAOD::MuonContainer*>(importedParticles);
  if (testMuons) {
    is_muons = true;
  }
  const xAOD::TauJetContainer* testTaus =
    dynamic_cast<const xAOD::TauJetContainer*>(importedParticles);
  if (testTaus) {
    is_tau = true;
  }

  if (!(is_egamma || is_muons || is_tau)) {
    ATH_MSG_ERROR("This tool only works with Egamma, Muons and Taus, "
                  << m_sgKey.key() << " is not a compatible collection");
    return StatusCode::FAILURE;
  }

  // Selection
  // Execute the text parsers if requested
  if (!m_selectionString.empty()) {
    std::vector<int> entries = m_parser->evaluateAsVector();
    unsigned int nEntries = entries.size();
    // check the sizes are compatible
    if (nEgammas != nEntries) {
      ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string "
                    "used e-gamma objects??");
      return StatusCode::FAILURE;
    }
    std::vector<const xAOD::IParticle*> particlesToCheck = {};
    // identify which e-gammas to keep for the thinning check
    for (unsigned int i = 0; i < nEgammas; ++i) {
      if (static_cast<bool>(entries[i])) {
        particlesToCheck.push_back((*importedParticles)[i]);
      }
    }
    ////
    for (const auto* particle : particlesToCheck) {
      if (m_run_calo) {
        // Always keep own particle clusters
        if (particleCluster(mask,
                            particle,
                            importedCaloCluster.cptr(),
                            is_muons,
                            is_egamma,
                            is_tau) != StatusCode::SUCCESS) {
          return StatusCode::FAILURE;
        }
        if (setClustersMask(mask,
                            particle,
                            importedCaloCluster.cptr(),
                            is_muons,
                            is_egamma) != StatusCode::SUCCESS) {
          return StatusCode::FAILURE;
        }
      }
      if (m_run_topo) {
        if (particleCluster(topomask,
                            particle,
                            importedTopoCaloCluster.cptr(),
                            is_muons,
                            is_egamma,
                            is_tau) != StatusCode::SUCCESS) {
          return StatusCode::FAILURE;
        }
        if (setClustersMask(topomask,
                            particle,
                            importedTopoCaloCluster.cptr(),
                            is_muons,
                            is_egamma) != StatusCode::SUCCESS) {
          return StatusCode::FAILURE;
        }
      }
    }
  } else { // No selection string provided
    for (const auto* particle : *importedParticles) {
      if (m_run_calo) {
        if (particleCluster(mask,
                            particle,
                            importedCaloCluster.cptr(),
                            is_muons,
                            is_egamma,
                            is_tau) != StatusCode::SUCCESS) {
          return StatusCode::FAILURE;
        }
        if (setClustersMask(mask,
                            particle,
                            importedCaloCluster.cptr(),
                            is_muons,
                            is_egamma) != StatusCode::SUCCESS) {
          return StatusCode::FAILURE;
        }
      }
      if (m_run_topo) {
        if (particleCluster(topomask,
                            particle,
                            importedTopoCaloCluster.cptr(),
                            is_muons,
                            is_egamma,
                            is_tau) != StatusCode::SUCCESS) {
          return StatusCode::FAILURE;
        }
        if (setClustersMask(topomask,
                            particle,
                            importedTopoCaloCluster.cptr(),
                            is_muons,
                            is_egamma) != StatusCode::SUCCESS) {
          return StatusCode::FAILURE;
        }
      }
    }
  }
  //
  // Count up the mask contents
  for (unsigned int i = 0; i < nClusters; ++i) {
    if (mask[i])
      ++m_npass;
  }
  for (unsigned int i = 0; i < nTopoClusters; ++i) {
    if (topomask[i])
      ++m_npassTopo;
  }
  //
  // Execute the thinning service based on the mask. Finish.
  if (m_run_calo) {
    SG::ThinningHandle<xAOD::CaloClusterContainer> thin(m_CaloClSGKey, ctx);
    thin.keep(mask);
  }
  if (m_run_topo) {
    SG::ThinningHandle<xAOD::CaloClusterContainer> thin(m_TopoClSGKey, ctx);
    thin.keep(topomask);
  }
  return StatusCode::SUCCESS;
}

StatusCode
DerivationFramework::CaloClusterThinning::setClustersMask(
  std::vector<bool>& mask,
  const xAOD::IParticle* particle,
  const xAOD::CaloClusterContainer* cps,
  bool is_muons,
  bool is_egamma) const
{
  if (!cps) {
    ATH_MSG_ERROR("Cluster collection not found");
    return StatusCode::FAILURE;
  }

  if (is_egamma) {
    const xAOD::Egamma* egamma = dynamic_cast<const xAOD::Egamma*>(particle);
    if (!egamma) {
      ATH_MSG_ERROR("Egamma cast failed");
      return StatusCode::FAILURE;
    }
    DerivationFramework::ClustersInCone::select(egamma, m_coneSize, cps, mask);
  }
  if (is_muons) {
    const xAOD::Muon* muon = dynamic_cast<const xAOD::Muon*>(particle);
    if (!muon) {
      ATH_MSG_ERROR("Muon cast failed");
      return StatusCode::FAILURE;
    }
    DerivationFramework::ClustersInCone::select(muon, m_coneSize, cps, mask);
  }
  return StatusCode::SUCCESS;
}

StatusCode
DerivationFramework::CaloClusterThinning::particleCluster(
  std::vector<bool>& mask,
  const xAOD::IParticle* particle,
  const xAOD::CaloClusterContainer* cps,
  bool is_muons,
  bool is_egamma,
  bool is_tau) const
{
  if (!cps) {
    ATH_MSG_ERROR("Cluster collection not found");
    return StatusCode::FAILURE;
  }

  if (is_egamma) {
    const xAOD::Egamma* egamma = dynamic_cast<const xAOD::Egamma*>(particle);
    if (!egamma) {
      ATH_MSG_ERROR("Egamma cast failed");
      return StatusCode::FAILURE;
    }
    auto it = std::find(cps->begin(), cps->end(), egamma->caloCluster());
    if (it != cps->end()) {
      int ItsCluster = std::distance(cps->begin(), it);
      mask[ItsCluster] = true;
    }
  }

  if (is_muons) {
    const xAOD::Muon* muon = dynamic_cast<const xAOD::Muon*>(particle);
    if (!muon) {
      ATH_MSG_ERROR("Muon cast failed");
      return StatusCode::FAILURE;
    }
    auto it = std::find(cps->begin(), cps->end(), muon->cluster());
    if (it != cps->end()) {
      int ItsCluster = std::distance(cps->begin(), it);
      mask[ItsCluster] = true;
    }
  }

  if (is_tau) {
    const xAOD::TauJet* tau = dynamic_cast<const xAOD::TauJet*>(particle);
    if (!tau) {
      ATH_MSG_ERROR("TauJet cast failed");
      return StatusCode::FAILURE;
    }
    xAOD::JetConstituentVector vec = tau->jet()->getConstituents();
    xAOD::JetConstituentVector::iterator it = vec.begin();
    xAOD::JetConstituentVector::iterator itE = vec.end();
    TLorentzVector LC_P4 = tau->p4(xAOD::TauJetParameters::DetectorAxis);
    for (; it != itE; it++) {
      TLorentzVector cluster_P4;
      cluster_P4.SetPtEtaPhiM(1, (*it)->Eta(), (*it)->Phi(), 1);
      if (LC_P4.DeltaR(cluster_P4) > 0.2)
        continue;
      const xAOD::CaloCluster* cl =
        dynamic_cast<const xAOD::CaloCluster*>((*it)->rawConstituent());
      auto it_cps = std::find(cps->begin(), cps->end(), cl);
      if (it_cps != cps->end()) {
        int ItsCluster = std::distance(cps->begin(), it_cps);
        mask[ItsCluster] = true;
      }
    }
  }

  return StatusCode::SUCCESS;
}
