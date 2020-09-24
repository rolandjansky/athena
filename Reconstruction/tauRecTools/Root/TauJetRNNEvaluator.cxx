/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/TauJetRNNEvaluator.h"
#include "tauRecTools/TauJetRNN.h"
#include "tauRecTools/HelperFunctions.h"

#include "PathResolver/PathResolver.h"

#include <algorithm>


TauJetRNNEvaluator::TauJetRNNEvaluator(const std::string &name): 
    TauRecToolBase(name),
    m_net_0p(nullptr),
    m_net_1p(nullptr),
    m_net_3p(nullptr){
    
    declareProperty("NetworkFile0P", m_weightfile_0p = "");
    declareProperty("NetworkFile1P", m_weightfile_1p = "");
    declareProperty("NetworkFile3P", m_weightfile_3p = "");
    declareProperty("OutputVarname", m_output_varname = "RNNJetScore");
    declareProperty("MaxTracks", m_max_tracks = 10);
    declareProperty("MaxClusters", m_max_clusters = 6);
    declareProperty("MaxClusterDR", m_max_cluster_dr = 1.0f);

    // Naming conventions for the network weight files:
    declareProperty("InputLayerScalar", m_input_layer_scalar = "scalar");
    declareProperty("InputLayerTracks", m_input_layer_tracks = "tracks");
    declareProperty("InputLayerClusters", m_input_layer_clusters = "clusters");
    declareProperty("OutputLayer", m_output_layer = "rnnid_output");
    declareProperty("OutputNode", m_output_node = "sig_prob");
    
    declareProperty("IncShowerSubtr", m_incShowerSubtr = true, "use shower subtracted clusters in calo calculations");
}

TauJetRNNEvaluator::~TauJetRNNEvaluator() {}

StatusCode TauJetRNNEvaluator::initialize() {
    ATH_MSG_INFO("Initializing TauJetRNNEvaluator");
  
    ATH_CHECK(m_tauVertexCorrection.retrieve()); 

    std::string weightfile_0p("");
    std::string weightfile_1p("");
    std::string weightfile_3p("");

    // Use PathResolver to search for the weight files
    if (!m_weightfile_0p.empty()) {
        weightfile_0p = find_file(m_weightfile_0p);
        if (weightfile_0p.empty()) {
            ATH_MSG_ERROR("Could not find network weights: "
                          << m_weightfile_0p);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_INFO("Using network config [0-prong]: " << weightfile_0p);
        }
    }

    if (!m_weightfile_1p.empty()) {
        weightfile_1p = find_file(m_weightfile_1p);
        if (weightfile_1p.empty()) {
            ATH_MSG_ERROR("Could not find network weights: "
                          << m_weightfile_1p);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_INFO("Using network config [1-prong]: " << weightfile_1p);
        }
    }

    if (!m_weightfile_3p.empty()) {
        weightfile_3p = find_file(m_weightfile_3p);
        if (weightfile_3p.empty()) {
            ATH_MSG_ERROR("Could not find network weights: "
                          << m_weightfile_3p);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_INFO("Using network config [3-prong]: " << weightfile_3p);
        }
    }

    // Set the layer and node names in the weight file
    TauJetRNN::Config config;
    config.input_layer_scalar = m_input_layer_scalar;
    config.input_layer_tracks = m_input_layer_tracks;
    config.input_layer_clusters = m_input_layer_clusters;
    config.output_layer = m_output_layer;
    config.output_node = m_output_node;

    // Load the weights and create the network
    // 0p is for trigger only
    if (!weightfile_0p.empty()) {
      m_net_0p = std::make_unique<TauJetRNN>(weightfile_0p, config);
      if (!m_net_0p) {
        ATH_MSG_WARNING("No network configured for 0-prong taus. "
                        "Decorating defaults...");
      }
    }

    m_net_1p = std::make_unique<TauJetRNN>(weightfile_1p, config);

    if (!m_net_1p) {
        ATH_MSG_WARNING("No network configured for 1-prong taus. "
                        "Decorating defaults...");
    }

    m_net_3p = std::make_unique<TauJetRNN>(weightfile_3p, config);
      
    if (!m_net_3p) {
        ATH_MSG_WARNING("No network configured for multi-prong taus. "
                        "Decorating defaults...");
    }

    return StatusCode::SUCCESS;
}

StatusCode TauJetRNNEvaluator::execute(xAOD::TauJet &tau) const {
    // Output variable accessor
    const SG::AuxElement::Accessor<float> output(m_output_varname);

    // Set default score and overwrite later
    output(tau) = -1111.0f;

    const auto nTracksCharged = tau.nTracksCharged();

    // Get input objects
    std::vector<const xAOD::TauTrack *> tracks;
    ATH_CHECK(get_tracks(tau, tracks));
    std::vector<const xAOD::CaloCluster *> clusters;
    ATH_CHECK(get_clusters(tau, clusters));

    // Evaluate networks
    if (nTracksCharged==0 && m_net_0p) {
      output(tau) = m_net_0p->compute(tau, tracks, clusters);
    }

    else if (nTracksCharged == 1 && m_net_1p) {
        output(tau) = m_net_1p->compute(tau, tracks, clusters);
    }

    else if (nTracksCharged > 1 && m_net_3p) {
        output(tau) = m_net_3p->compute(tau, tracks, clusters);
    }

    return StatusCode::SUCCESS;
}

const TauJetRNN* TauJetRNNEvaluator::get_rnn_0p() const {
    return m_net_0p.get();
}

const TauJetRNN* TauJetRNNEvaluator::get_rnn_1p() const {
    return m_net_1p.get();
}

const TauJetRNN* TauJetRNNEvaluator::get_rnn_3p() const {
    return m_net_3p.get();
}

StatusCode TauJetRNNEvaluator::get_tracks(
    const xAOD::TauJet &tau, std::vector<const xAOD::TauTrack *> &out) const {
    auto tracks = tau.allTracks();

    // Sort by descending pt
    auto cmp_pt = [](const xAOD::TauTrack *lhs, const xAOD::TauTrack *rhs) {
        return lhs->pt() > rhs->pt();
    };
    std::sort(tracks.begin(), tracks.end(), cmp_pt);

    // Truncate tracks
    if (tracks.size() > m_max_tracks) {
        tracks.resize(m_max_tracks);
    }
    out = std::move(tracks);

    return StatusCode::SUCCESS;
}

StatusCode TauJetRNNEvaluator::get_clusters(
    const xAOD::TauJet &tau, std::vector<const xAOD::CaloCluster *> &out) const {

    if (! tau.jetLink().isValid()) {
        ATH_MSG_ERROR("Tau jet link is invalid.");
        return StatusCode::FAILURE;
    }
    const xAOD::Jet *jetSeed = tau.jet();
    
    const xAOD::Vertex* jetVertex = m_tauVertexCorrection->getJetVertex(*jetSeed);
    
    const xAOD::Vertex* tauVertex = nullptr;
    if (tau.vertexLink().isValid()) tauVertex = tau.vertex();
    
    TLorentzVector tauAxis = m_tauVertexCorrection->getTauAxis(tau);

    std::vector<const xAOD::CaloCluster*> clusters;
    ATH_CHECK(tauRecTools::GetJetClusterList(jetSeed, clusters, m_incShowerSubtr));

    // remove clusters that do not meet dR requirement
    auto cItr = clusters.begin();
    while( cItr != clusters.end() ){
      const xAOD::CaloCluster* cluster = (*cItr);
      TLorentzVector clusterP4 = m_tauVertexCorrection->getVertexCorrectedP4(*cluster, tauVertex, jetVertex);

      if (tauAxis.DeltaR(clusterP4) > m_max_cluster_dr) {
        clusters.erase(cItr);
      }
      else {
        ++cItr;
      }
    }

    // Sort by descending et
    auto et_cmp = [](const xAOD::CaloCluster *lhs,
                     const xAOD::CaloCluster *rhs) {
        return lhs->et() > rhs->et();
    };
    std::sort(clusters.begin(), clusters.end(), et_cmp);

    // Truncate clusters
    if (clusters.size() > m_max_clusters) {
        clusters.resize(m_max_clusters);
    }
    out = std::move(clusters);

    return StatusCode::SUCCESS;
}
