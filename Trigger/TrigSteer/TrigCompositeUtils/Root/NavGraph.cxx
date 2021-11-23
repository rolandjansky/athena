/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/NavGraph.h"

namespace TrigCompositeUtils {

  NavGraphNode::NavGraphNode(const Decision* me) : m_decisionObject(me), 
    m_filteredSeeds(), m_filteredChildren(), m_keepFlag(false)
  {
  }


  bool NavGraphNode::linksTo(NavGraphNode* to) {
    to->m_filteredChildren.insert(this);
    return (m_filteredSeeds.insert(to)).second; // Return TRUE if a new edge is added
  }


  void NavGraphNode::dropLinks(NavGraphNode* node) {
    m_filteredChildren.erase(node);
    m_filteredSeeds.erase(node);
  }


  const Decision* NavGraphNode::node() const {
    return m_decisionObject;
  }


  const std::set<NavGraphNode*>& NavGraphNode::seeds() const {
    return m_filteredSeeds;
  }

  const std::set<NavGraphNode*>& NavGraphNode::children() const {
    return m_filteredChildren;
  }


  void NavGraphNode::keep() {
    m_keepFlag = true;
  }


  void NavGraphNode::resetKeep() {
    m_keepFlag = false;
  }

 
  bool NavGraphNode::getKeep() const {
    return m_keepFlag;
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


  std::set<NavGraphNode*> NavGraph::finalNodes() const {
    return m_finalNodes;
  }

  std::set<NavGraphNode*> NavGraph::allNodes() {
    std::set<NavGraphNode*> returnSet;
    for (auto& entry : m_nodes) {
      NavGraphNode& n = entry.second;
      returnSet.insert( &n );
    }
    return returnSet;
  }


  size_t NavGraph::nodes() const {
    return m_nodes.size();
  }


  size_t NavGraph::edges() const {
    return m_edges;
  }

  std::set<const Decision*> NavGraph::thin() {
    std::set<const Decision*> returnSet;
    std::map<const Decision*, NavGraphNode>::iterator it;
    for (it = m_nodes.begin(); it != m_nodes.end(); /*noop*/) {
      if (it->second.getKeep()) {
        it->second.resetKeep();
        ++it;
      } else {
        returnSet.insert(it->first);
        rewireNodeForRemoval(it->second);
        it = m_nodes.erase(it);
      }
    }
    return returnSet;
  }

  void NavGraph::rewireNodeForRemoval(NavGraphNode& toBeDeleted) {
    const std::set<NavGraphNode*> myParents = toBeDeleted.seeds();
    const std::set<NavGraphNode*> myChildren = toBeDeleted.children();

    // Perform the (potentially) many-to-many re-linking required to remove toBeDeleted from the graph
    for (NavGraphNode* child : myChildren) {
      for (NavGraphNode* parent : myParents) {
        bool newEdge = child->linksTo(parent);
        if (newEdge) {
          ++m_edges;
        }
      }
    }

    // Remove the edges connecting to toBeDeleted from all of its children and all of parents
    for (NavGraphNode* child : myChildren) {
      child->dropLinks(&toBeDeleted);
      --m_edges;
    }
    for (NavGraphNode* parent : myParents) {
      parent->dropLinks(&toBeDeleted);
      --m_edges;
    }
  }


  void NavGraph::printAllPaths(MsgStream& log, MSG::Level msgLevel) const {
    // finalNodes() is a set<NavGraphNode*>, so iteration over it
    // will not be in any well-defined order.  Sort the set of pointers
    // by name so that the output is predictable.
    std::vector<const NavGraphNode*> nodes (m_finalNodes.begin(),
                                            m_finalNodes.end());
    std::sort (nodes.begin(), nodes.end(),
               [] (const NavGraphNode* a, const NavGraphNode* b)
               {
                 return a->node()->name() < b->node()->name();
               });

    for (const NavGraphNode* finalNode : nodes) {
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

    ss << "|-> " << nodeEL.dataID() << " #" << nodeEL.index() << " Name(" << node->name() << ") Passing(" << node->decisions().size() << ")";
    if (node->hasObjectLink("feaure")) {
      uint32_t key, clid;
      uint16_t index;
      node->typelessGetObjectLink("feature", key, clid, index);
      ss << " feature(" << key << ")";
    }
    log << msgLevel << ss.str() << endmsg;
    for (const NavGraphNode* seed : nav.seeds()) {
      recursivePrintNavPath(*seed, level + 1, log, msgLevel);
    }
  }

}
