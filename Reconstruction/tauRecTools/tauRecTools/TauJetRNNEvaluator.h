/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_TAUJETRNNEVALUATOR_H
#define TAURECTOOLS_TAUJETRNNEVALUATOR_H

#include "tauRecTools/TauRecToolBase.h"

#include "xAODTau/TauJet.h"
#include "xAODCaloEvent/CaloVertexedTopoCluster.h"

#include <memory>

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

    virtual StatusCode initialize() override;
    virtual StatusCode execute(xAOD::TauJet &tau) const override;
    // Getter for the underlying RNN implementation
    const TauJetRNN* get_rnn_0p() const;
    const TauJetRNN* get_rnn_1p() const;
    const TauJetRNN* get_rnn_3p() const;

    // Selects tracks to be used as input to the network
    StatusCode get_tracks(const xAOD::TauJet &tau,
                          std::vector<const xAOD::TauTrack *> &out) const;

    // Selects clusters to be used as input to the network
    StatusCode get_clusters(const xAOD::TauJet &tau,
                            std::vector<xAOD::CaloVertexedTopoCluster> &out) const;

private:
    std::string m_output_varname;
    std::string m_weightfile_0p;
    std::string m_weightfile_1p;
    std::string m_weightfile_3p;
    std::size_t m_max_tracks;
    std::size_t m_max_clusters;
    float m_max_cluster_dr;
    bool m_doVertexCorrection;

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

#endif // TAURECTOOLS_TAUJETRNNEVALUATOR_H
