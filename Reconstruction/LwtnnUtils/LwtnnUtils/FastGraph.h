// this is -*- C++ -*-
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Any modifications to this file may be copied to lwtnn[1] without
// attribution.
//
// [1]: https::www.github.com/lwtnn/lwtnn

#ifndef LWTNN_UTILS_FAST_GRAPH_H
#define LWTNN_UTILS_FAST_GRAPH_H

#include "lwtnn/lightweight_network_config.hh"
#include "lwtnn/Graph.hh"

#include <Eigen/Dense>

namespace lwt::atlas {

  class FastInputPreprocessor;
  class FastInputVectorPreprocessor;
  struct InputOrder;

  struct SourceIndices
  {
    std::vector<size_t> scalar;
    std::vector<size_t> sequence;
  };

  // Graph class
  class FastGraph
  {
  public:
    // Since a graph has multiple input nodes, we actually call
    typedef std::vector<Eigen::VectorXd> NodeVec;
    typedef std::vector<Eigen::MatrixXd> SeqNodeVec;


    // In cases where the graph has multiple outputs, we have to
    // define a "default" output, so that calling "compute" with no
    // output specified doesn't lead to ambiguity.
    FastGraph(const GraphConfig& config, const InputOrder& order,
              std::string default_output = "");

    ~FastGraph();
    FastGraph(FastGraph&) = delete;
    FastGraph& operator=(FastGraph&) = delete;

    // The simpler "compute" function
    Eigen::VectorXd compute(const NodeVec&, const SeqNodeVec& = {}) const;

    // the other "compute" which allows you to select an arbitrary output
    Eigen::VectorXd compute(const NodeVec&, const SeqNodeVec&, size_t) const;

  private:
    typedef FastInputPreprocessor IP;
    typedef FastInputVectorPreprocessor IVP;
    typedef std::vector<IP*> Preprocs;
    typedef std::vector<IVP*> VecPreprocs;

    Graph* m_graph;
    Preprocs m_preprocs;
    VecPreprocs m_vec_preprocs;
    size_t m_default_output;
    // the mapping from a node in the network to a user input node
    SourceIndices m_input_indices;
  };
}

#endif
