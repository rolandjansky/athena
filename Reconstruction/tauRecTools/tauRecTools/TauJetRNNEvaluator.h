#ifndef TAURECTOOLS_TAUJETRNNEVALUATOR_H
#define TAURECTOOLS_TAUJETRNNEVALUATOR_H

#include <memory>

#include "tauRecTools/TauRecToolBase.h"

// xAOD includes
#include "xAODTau/TauJet.h"

// Forward declarations
class TauJetRNN;


class TauJetRNNEvaluator : public TauRecToolBase {
public:
    ASG_TOOL_CLASS2(TauJetRNNEvaluator, TauRecToolBase, ITauToolBase)

    TauJetRNNEvaluator(const std::string &name = "TauJetRNNEvaluator");
    virtual ~TauJetRNNEvaluator();

    StatusCode initialize();
    StatusCode execute(xAOD::TauJet &tau);

    TauJetRNN *get_rnn_1p();
    TauJetRNN *get_rnn_3p();

private:
    StatusCode get_tracks(const xAOD::TauJet &tau,
                          std::vector<const xAOD::TauTrack *> &out);

    StatusCode get_clusters(const xAOD::TauJet &tau,
                            std::vector<const xAOD::CaloCluster *> &out);

private:
    std::string m_output_varname;
    std::string m_weightfile_1p;
    std::string m_weightfile_3p;
    std::size_t m_max_tracks;
    std::size_t m_max_clusters;

    std::string m_input_layer_scalar;
    std::string m_input_layer_tracks;
    std::string m_input_layer_clusters;
    std::string m_output_layer;
    std::string m_output_node;

    std::unique_ptr<TauJetRNN> m_net_1p; //!
    std::unique_ptr<TauJetRNN> m_net_3p; //!
};


#endif // TAURECTOOLS_TAUJETRNNEVALUATOR_H
