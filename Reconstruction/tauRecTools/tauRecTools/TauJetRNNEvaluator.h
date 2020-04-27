/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUJETRNNEVALUATOR_H
#define TAUREC_TAUJETRNNEVALUATOR_H

#include <memory>

#include "tauRecTools/TauRecToolBase.h"

// xAOD includes
#include "xAODTau/TauJet.h"

// Forward declarations
class TauJetRNN;

/**
 * @brief Tool to calculate a tau identification score based on neural networks
 *
 *   The network configuration is supplied in .json format for both 1- and
 *   3-prong taus separately.
 *
 * @author C. Deutsch
 * @author W. Davey
 *
 */
class TauJetRNNEvaluator : public TauRecToolBase {
public:
    ASG_TOOL_CLASS2(TauJetRNNEvaluator, TauRecToolBase, ITauToolBase)

    TauJetRNNEvaluator(const std::string &name = "TauJetRNNEvaluator");
    virtual ~TauJetRNNEvaluator();

    StatusCode initialize() override;
    StatusCode execute(xAOD::TauJet &tau) override;
    // Getter for the underlying RNN implementation
    TauJetRNN *get_rnn_0p();
    TauJetRNN *get_rnn_1p();
    TauJetRNN *get_rnn_3p();

private:
    // Selects tracks to be used as input to the network
    StatusCode get_tracks(const xAOD::TauJet &tau,
                          std::vector<const xAOD::TauTrack *> &out);

    // Selects clusters to be used as input to the network
    StatusCode get_clusters(const xAOD::TauJet &tau,
                            std::vector<const xAOD::CaloCluster *> &out);

private:
    Gaudi::Property<std::string> m_output_varname {this, "OutputVarname", "RNNJetScore"};
    // Network weight files for 0-, 1- and 3-prong taus
    // If the filename is an empty string a default value is decorated
    Gaudi::Property<std::string> m_weightfile_0p {this, "NetworkFile0P", ""};
    Gaudi::Property<std::string> m_weightfile_1p {this, "NetworkFile1P", ""};
    Gaudi::Property<std::string> m_weightfile_3p {this, "NetworkFile3P", ""};
    Gaudi::Property<std::size_t> m_max_tracks {this, "MaxTracks", 10};
    Gaudi::Property<std::size_t> m_max_clusters {this, "MaxClusters", 6};
    Gaudi::Property<float> m_max_cluster_dr {this, "MaxClusterDR", 1.0f};

    // Configuration of the weight file
    Gaudi::Property<std::string> m_input_layer_scalar {this, "InputLayerScalar", "scalar"};
    Gaudi::Property<std::string> m_input_layer_tracks {this, "InputLayerTracks", "tracks"};
    Gaudi::Property<std::string> m_input_layer_clusters {this, "InputLayerClusters", "clusters"};
    Gaudi::Property<std::string> m_output_layer {this, "OutputLayer", "rnnid_output"};
    Gaudi::Property<std::string> m_output_node {this, "OutputNode", "sig_prob"};

    // Wrappers for lwtnn
    std::unique_ptr<TauJetRNN> m_net_0p; //!
    std::unique_ptr<TauJetRNN> m_net_1p; //!
    std::unique_ptr<TauJetRNN> m_net_3p; //!
};


#endif // TAUREC_TAUJETRNNEVALUATOR_H
