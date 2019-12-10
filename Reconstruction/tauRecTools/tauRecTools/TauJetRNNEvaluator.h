/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

    StatusCode initialize();
    StatusCode execute(xAOD::TauJet &tau);
    StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
    StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&, const xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
    StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
    StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }
    StatusCode executePi0nPFO(xAOD::TauJet&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }

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
    std::string m_output_varname;
    std::string m_weightfile_0p;
    std::string m_weightfile_1p;
    std::string m_weightfile_3p;
    std::size_t m_max_tracks;
    std::size_t m_max_clusters;
    float m_max_cluster_dr;

    // Configuration of the weight file
    std::string m_input_layer_scalar;
    std::string m_input_layer_tracks;
    std::string m_input_layer_clusters;
    std::string m_output_layer;
    std::string m_output_node;

    // Wrappers for lwtnn
    std::unique_ptr<TauJetRNN> m_net_0p; //!
    std::unique_ptr<TauJetRNN> m_net_1p; //!
    std::unique_ptr<TauJetRNN> m_net_3p; //!
};


#endif // TAUREC_TAUJETRNNEVALUATOR_H
