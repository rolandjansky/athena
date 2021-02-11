// this is -*- C++ -*-
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Any modifications to this file may be copied to lwtnn[1] without
// attribution.
//
// [1]: https::www.github.com/lwtnn/lwtnn

#include "LwtnnUtils/FastGraph.h"
#include "LwtnnUtils/FastInputPreprocessor.h"
#include "LwtnnUtils/InputOrder.h"

#include <Eigen/Dense>

namespace {
  using namespace Eigen;
  using namespace lwt;
  using namespace lwt::atlas;

  typedef atlas::FastGraph::NodeVec NodeVec;
  typedef atlas::FastInputPreprocessor IP;
  typedef std::vector<IP*> Preprocs;
  typedef atlas::FastGraph::SeqNodeVec SeqNodeVec;
  typedef atlas::FastInputVectorPreprocessor IVP;
  typedef std::vector<IVP*> VecPreprocs;


  // this is used internally to ensure that we only look up map inputs
  // when the network asks for them.
  class LazySource: public ISource
  {
  public:
    LazySource(const NodeVec&, const SeqNodeVec&,
               const Preprocs&, const VecPreprocs&,
               const SourceIndices& input_indices);
    virtual VectorXd at(size_t index) const override;
    virtual MatrixXd matrix_at(size_t index) const override;
  private:
    const NodeVec& m_nodes;
    const SeqNodeVec& m_seqs;
    const Preprocs& m_preprocs;
    const VecPreprocs& m_vec_preprocs;
    const SourceIndices& m_input_indices;
  };

  LazySource::LazySource(const NodeVec& n, const SeqNodeVec& s,
                         const Preprocs& p, const VecPreprocs& v,
                         const SourceIndices& i):
    m_nodes(n), m_seqs(s), m_preprocs(p), m_vec_preprocs(v),
    m_input_indices(i)
  {
  }
  VectorXd LazySource::at(size_t index) const
  {
    const auto& preproc = *m_preprocs.at(index);
    size_t source_index = m_input_indices.scalar.at(index);
    if (source_index >= m_nodes.size()) {
      throw NNEvaluationException(
        "The NN needs an input VectorXd at position "
        + std::to_string(source_index) + " but only "
        + std::to_string(m_nodes.size()) + " inputs were given");
    }
    return preproc(m_nodes.at(source_index));
  }
  MatrixXd LazySource::matrix_at(size_t index) const
  {
    const auto& preproc = *m_vec_preprocs.at(index);
    size_t source_index = m_input_indices.sequence.at(index);
    if (source_index >= m_nodes.size()) {
      throw NNEvaluationException(
        "The NN needs an input MatrixXd at position "
        + std::to_string(source_index) + " but only "
        + std::to_string(m_nodes.size()) + " inputs were given");
    }
    return preproc(m_seqs.at(source_index));
  }

  // utility functions
  //
  // Build a mapping from the inputs in the saved network to the
  // inputs that the user is going to hand us.
  std::vector<size_t> get_node_indices(
    const order_t& order,
    const std::vector<lwt::InputNodeConfig>& inputs)
  {
    std::map<std::string, size_t> order_indices;
    for (size_t i = 0; i < order.size(); i++) {
      order_indices[order.at(i).first] = i;
    }
    std::vector<size_t> node_indices;
    for (const lwt::InputNodeConfig& input: inputs) {
      if (!order_indices.count(input.name)) {
        throw NNConfigurationException("Missing input " + input.name);
      }
      node_indices.push_back(order_indices.at(input.name));
    }
    return node_indices;
  }


}
namespace lwt::atlas {
  // ______________________________________________________________________
  // Fast Graph

  typedef FastGraph::NodeVec NodeVec;
  FastGraph::FastGraph(const GraphConfig& config, const InputOrder& order,
                       std::string default_output):
    m_graph(new Graph(config.nodes, config.layers))
  {

    m_input_indices.scalar = get_node_indices(
      order.scalar, config.inputs);

    m_input_indices.sequence = get_node_indices(
      order.sequence, config.input_sequences);

    for (size_t i = 0; i < config.inputs.size(); i++) {
      const lwt::InputNodeConfig& node = config.inputs.at(i);
      size_t input_node = m_input_indices.scalar.at(i);
      std::vector<std::string> varorder = order.scalar.at(input_node).second;
      m_preprocs.emplace_back(
        new FastInputPreprocessor(node.variables, varorder));
    }
    for (size_t i = 0; i < config.input_sequences.size(); i++) {
      const lwt::InputNodeConfig& node = config.input_sequences.at(i);
      size_t input_node = m_input_indices.sequence.at(i);
      std::vector<std::string> varorder = order.sequence.at(input_node).second;
      m_vec_preprocs.emplace_back(
        new FastInputVectorPreprocessor(node.variables, varorder));
    }
    if (default_output.size() > 0) {
      if (!config.outputs.count(default_output)) {
        throw NNConfigurationException("no output node" + default_output);
      }
      m_default_output = config.outputs.at(default_output).node_index;
    } else if (config.outputs.size() == 1) {
      m_default_output = config.outputs.begin()->second.node_index;
    } else {
      throw NNConfigurationException("you must specify a default output");
    }
  }

  FastGraph::~FastGraph() {
    delete m_graph;
    for (auto& preproc: m_preprocs) {
      delete preproc;
      preproc = 0;
    }
    for (auto& preproc: m_vec_preprocs) {
      delete preproc;
      preproc = 0;
    }
  }

  VectorXd FastGraph::compute(const NodeVec& nodes,
                              const SeqNodeVec& seq) const {
    return compute(nodes, seq, m_default_output);
  }
  VectorXd FastGraph::compute(const NodeVec& nodes,
                              const SeqNodeVec& seq,
                              size_t idx) const {
    LazySource source(nodes, seq, m_preprocs, m_vec_preprocs,
                      m_input_indices);
    return m_graph->compute(source, idx);
  }

}
