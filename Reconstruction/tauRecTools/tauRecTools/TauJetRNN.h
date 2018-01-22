#ifndef TAURECTOOLS_TAUJETRNN_H
#define TAURECTOOLS_TAUJETRNN_H

#include <memory>

#include "xAODTau/TauJet.h"


// Forward declaration
namespace lwt {
    class LightweightGraph;
}

namespace TauJetRNNUtils {
    class VarCalc;
}


class TauJetRNN {
public:
    struct Config {
        std::string input_layer_scalar;
        std::string input_layer_tracks;
        std::string input_layer_clusters;
        std::string output_layer;
        std::string output_node;
    };

public:
    TauJetRNN(const std::string &filename, const Config &config);
    ~TauJetRNN();

    float compute(const xAOD::TauJet &tau,
                  const std::vector<const xAOD::TauTrack *> &tracks,
                  const std::vector<const xAOD::CaloCluster *> &clusters);

    TauJetRNNUtils::VarCalc *variable_calculator() {
        return m_var_calc.get();
    }

    explicit operator bool() const {
        return static_cast<bool>(m_graph);
    }

private:
    // Abbreviations for lwtnn
    using VariableMap = std::map<std::string, double>;
    using VectorMap = std::map<std::string, std::vector<double>>;

    using InputMap = std::map<std::string, VariableMap>;
    using InputSequenceMap = std::map<std::string, VectorMap>;

private:
    const Config m_config;
    std::unique_ptr<const lwt::LightweightGraph> m_graph;

    // Holding names of the inputs
    std::vector<std::string> m_scalar_inputs;
    std::vector<std::string> m_track_inputs;
    std::vector<std::string> m_cluster_inputs;

    // Maps to hold input values for lwtnn
    InputMap m_input_map;
    InputSequenceMap m_input_sequence_map;

    // Pointers to contents of the input (sequence) map
    VariableMap *m_scalar_map;
    VectorMap *m_track_map;
    VectorMap *m_cluster_map;

    // Variable calculator
    std::unique_ptr<TauJetRNNUtils::VarCalc> m_var_calc;
};

#endif // TAURECTOOLS_TAUJETRNN_H
