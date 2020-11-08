#ifndef FAST_GRAPH_H
#define FAST_GRAPH_H

#include "lwtnn/lightweight_network_config.hh"

namespace lwt {
  class Graph;
}

namespace lwt::atlas {

  class FastInputPreprocessor;
  class FastInputVectorPreprocessor;

  // We currently allow several input types
  // The "ValueMap" is for simple rank-1 inputs
  typedef std::map<std::string, double> ValueMap;
  // The "VectorMap" is for sequence inputs
  typedef std::map<std::string, std::vector<double> > VectorMap;

  // Graph class
  class FastGraph
  {
  public:
    // Since a graph has multiple input nodes, we actually call
    typedef std::map<std::string, ValueMap> NodeMap;
    typedef std::map<std::string, VectorMap> SeqNodeMap;

    // In cases where the graph has multiple outputs, we have to
    // define a "default" output, so that calling "compute" with no
    // output specified doesn't lead to ambiguity.
    FastGraph(const GraphConfig& config,
                     std::string default_output = "");

    ~FastGraph();
    FastGraph(FastGraph&) = delete;
    FastGraph& operator=(FastGraph&) = delete;

    // The simpler "compute" function
    ValueMap compute(const NodeMap&, const SeqNodeMap& = {}) const;

    // More complicated version, only needed when you have multiple
    // output nodes and need to specify the non-default ones
    ValueMap compute(const NodeMap&, const SeqNodeMap&,
                     const std::string& output) const;

    // The simpler "scan" function
    VectorMap scan(const NodeMap&, const SeqNodeMap& = {}) const;

    // More complicated version, only needed when you have multiple
    // output nodes and need to specify the non-default ones
    VectorMap scan(const NodeMap&, const SeqNodeMap&,
                   const std::string& output) const;

  private:
    typedef FastInputPreprocessor IP;
    typedef FastInputVectorPreprocessor IVP;
    typedef std::vector<std::pair<std::string, IP*> > Preprocs;
    typedef std::vector<std::pair<std::string, IVP*> > VecPreprocs;

    ValueMap compute(const NodeMap&, const SeqNodeMap&, size_t) const;
    VectorMap scan(const NodeMap&, const SeqNodeMap&, size_t) const;
    Graph* m_graph;
    Preprocs m_preprocs;
    VecPreprocs m_vec_preprocs;
    std::vector<std::pair<size_t, std::vector<std::string> > > m_outputs;
    std::map<std::string, size_t> m_output_indices;
    size_t m_default_output;
  };
}

#endif
