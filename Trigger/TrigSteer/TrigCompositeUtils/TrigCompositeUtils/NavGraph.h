/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigCompositeUtils_NavGraph_h
#define TrigCompositeUtils_NavGraph_h

#include <set>

#include "AthContainers/AuxElement.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"


namespace TrigCompositeUtils {

  /**
   * @class NavGraphNode
   * @brief Transient utility class to represent a node in a graph (m_decisionObject), and a set of edges (m_filteredSeeds)
   * to other nodes which are parents of this node.
   **/
  class NavGraphNode {
    public:

      /**
       * @brief Construct a NavGraphNode shadowing a node in the full xAOD navigation graph
       * @param[in] me The Decision object node from the full xAOD navigation graph which this object is representing.
       **/
      NavGraphNode(const Decision* me);

      /**
       * @brief Form an edge in the graph from this node to another one.
       * @param[in] to The "parent" or "seed" Decision object from the perspective of this Node's shadowed Decision object.
       * @return True if a new edge was added. False if this was a duplicated call to add this edge.
       **/
      bool linksTo(const NavGraphNode* to);

      /**
       * @brief Return a const pointer to the Decision object node which this NavGraphNode is shadowing.
       **/
      const Decision* node() const;

      /**
       * @brief Return a set of const pointers to the Decision object nodes which this NavGraphNode seeds from.
       * Note: NavGraph is used to represent a sub-graph of the full navigation graph, hence it is expected that
       * the set of seeds returned from this function may be smaller than the set of seeds returned from the
       * shadowed xAOD Decision Object.
       **/
      const std::set<const NavGraphNode*>& seeds() const;

    private:
      const Decision* m_decisionObject; //!< The Decision object node which I shadow
      std::set<const NavGraphNode*> m_filteredSeeds; //!< My seeds (edges in the graph), filtered on per-chain requirements.

  };

  /**
   * @class NavGraph
   * @brief Structure to hold a transient Directed Acyclic Graph (DAG) structure.
   * NavGraph is populated from, and forms a sub-graph over the full Run 3 trigger navigation graph in a single event.
   * Requirements on specific chains, and the specification of allowable graph entry-points are considered in the construction of the NavGraph sub-graph.
   * Once one of these sub-graphs is fully populated to a given specification, it is searched by the feature-retrieval code to find features.
   **/
  class NavGraph {

    public:

      /**
       * @brief Construct an empty NavGraph
       **/
      NavGraph();

      /**
       * @brief Add a new NavGraphNode which shadows the xAOD Decision object "node" from the full navigation graph
       * @param[in] node The xAOD Decision object which the new node will shadow. Will not cause duplication if node has already been added.
       * @param[in] comingFrom If not null, used to indicate which xAOD Decision object was the seed of "node". This is used to form an edge in the graph.
       * Alternately, if comingFrom is null then "node" is taken as a final node (one of the locations from which the graph should be explored) and hence is added
       * to the finalNodes set. 
       **/
      void addNode(const Decision* node, const Decision* comingFrom = nullptr);

      /**
       * @brief Get all final nodes.
       * @return Set of final nodes. These are the nodes which were added without any "comingFrom". 
       * To explore the NavGraph fully, one should explore recursively from each of the final nodes.
       **/
      const std::set<NavGraphNode*>& finalNodes() const;

      /**
       * @return Total number of unique nodes in the NavGraph. 
       **/
      size_t nodes() const;

      /**
       * @return Total number of unique edges in the NavGraph. 
       **/
      size_t edges() const;

      /**
       * @bried Helper function. Print the internal graph structure to the terminal.
       * @param[in] log Athena messaging service reference.
       * @param[in] msgLevel Athena messaging service verbosity level.
       **/
      void printAllPaths(MsgStream& log, MSG::Level msgLevel = MSG::VERBOSE) const;

    private:

      /**
       * @bried Internal helper function. Recursively print the graph structure from a single given starting node.
       * @param[in] nav The node to recursively explore.
       * @param[in] level The current depth of recursion. Used to pad output format.
       * @param[in] log Athena messaging service reference.
       * @param[in] msgLevel Athena messaging service verbosity level.
       **/
      void recursivePrintNavPath(const NavGraphNode& nav, size_t level, MsgStream& log, MSG::Level msgLevel) const;

      std::map<const Decision*, NavGraphNode> m_nodes; //!< Map of nodes in the graph. Indexed on the underlying Decision object's transient pointer.
      std::set<NavGraphNode*> m_finalNodes; //!< Entry points into the navigation graph. When iterating over the graph, start from all of these places.
      size_t m_edges; //!< Statistics on the number of edges, connecting the nodes in the graph.
  };

}

#endif // TrigCompositeUtils_NavGraph_h
