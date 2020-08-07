/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/NavGraph.h"

namespace TrigCompositeUtils {

  NavGraphNode::NavGraphNode(const Decision* me) : m_decisionObject(me){
  }


  bool NavGraphNode::linksTo(const NavGraphNode* to) {
    return (m_filteredSeeds.insert(to)).second; // Return TRUE if a new edge is added
  }


  const Decision* NavGraphNode::node() const {
    return m_decisionObject;
  }


  const std::set<const NavGraphNode*>& NavGraphNode::seeds() const {
    return m_filteredSeeds;
  }


  // #################################################


  NavGraph::NavGraph() : m_nodes(), m_finalNodes(), m_edges(0) {
  }


  void NavGraph::addNode(const Decision* node, const Decision* comingFrom) {
    auto nodePairIt = m_nodes.insert( std::make_pair(node, NavGraphNode(node)) );
    NavGraphNode& nodeObj = nodePairIt.first->second;
    
    if (comingFrom == nullptr) { // Not coming from anywhere - hence a final node.
      m_finalNodes.insert( &nodeObj );
    } else {
      auto comingFromPairIt = m_nodes.insert( std::make_pair(comingFrom, NavGraphNode(comingFrom)) );
      NavGraphNode& comingFromNodeObj = comingFromPairIt.first->second;
      const bool newEdge = comingFromNodeObj.linksTo( &nodeObj );
      if (newEdge) {
        ++m_edges;
      }
    }
  }


  const std::set<NavGraphNode*>& NavGraph::finalNodes() const {
    return m_finalNodes;
  }


  size_t NavGraph::nodes() const {
    return m_nodes.size();
  }


  size_t NavGraph::edges() const {
    return m_edges;
  }


  void NavGraph::printAllPaths(MsgStream& log, MSG::Level msgLevel) const {
    for (const NavGraphNode* finalNode : finalNodes()) {
      recursivePrintNavPath(*finalNode, 0, log, msgLevel);
    }
  }


  void NavGraph::recursivePrintNavPath(const NavGraphNode& nav, size_t level, MsgStream& log, MSG::Level msgLevel) const {
    const Decision* node = nav.node();
    const ElementLink<DecisionContainer> nodeEL = decisionToElementLink( node );
    std::stringstream ss;
    for (size_t i = 0; i < level; ++i) {
      ss << "  ";
    }
    ss << "|-> " << nodeEL.dataID() << " #" << nodeEL.index() << " (" << node->name() << ")";
    log << msgLevel << ss.str() << endmsg;
    for (const NavGraphNode* seed : nav.seeds()) {
      recursivePrintNavPath(*seed, level + 1, log, msgLevel);
    }
  }

}