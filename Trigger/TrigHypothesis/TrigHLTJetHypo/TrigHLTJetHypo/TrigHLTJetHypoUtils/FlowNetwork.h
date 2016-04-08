/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_FLOWNETWORK_H
#define TRIGHLTJETHYPO_FLOWNETWORK_H


// ********************************************************************
//
// NAME:     FlowNetwork.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR: P Sherwood
//
// ********************************************************************
//

#include <ostream>
#include <set>
#include <vector>
#include "./FlowEdge.h"
#include "./FlowEdgeBag.h"

//////////////////////////////////////

/**
 * FordFulkerson Algorithm - used, for eaxmple,for multiple eta regions,
 * shere each reagion may have > 1 threshold, and the regions do not
 * share jets
 *
 * The FlowEdge and FlowNetwork classes are used by the FF algorithm.
 */


/**
 * FlowNetwork
 * 
 * Adapted from Algorithms, Sedgewick and Wayne, 4th edition.
 * Their java version supplies implemtations of Bag and Queue classeds.
 * These are replaced here by std::multiset and std::queue classes, which 
 * have different interfaces to the java classes.

 * Accessing the FlowElements is done here through std::shared_ptr objects.

 *  The <tt>FlowNetwork</tt> class represents a capacitated network
 *  with vertices named 0 through <em>V</em> - 1, where each directed
 *  edge is of type {@link FlowEdge} and has a real-valued capacity
 *  and flow.
 *  It supports the following two primary operations: add an edge to the network,
 *  iterate over all of the edges incident to or from a vertex. It also provides
 *  methods for returning the number of vertices <em>V</em> and the number
 *  of edges <em>E</em>. Parallel edges and self-loops are permitted.
 *  <p>
 *  This implementation uses an adjacency-lists representation, which 
 *  is a vertex-indexed array of @link{Bag} objects.
 *  All operations take constant time (in the worst case) except
 *  iterating over the edges incident to a given vertex, which takes
 *  time proportional to the number of such edges.
 *  <p>
 *  For additional documentation,
 *  see <a href="http://algs4.cs.princeton.edu/64maxflow">Section 6.4</a> of
 *  <i>Algorithms, 4th Edition</i> by Robert Sedgewick and Kevin Wayne.
 */

class FlowNetwork {
 public:
  
/**
 * Initializes an empty flow network with <tt>V</tt> vertices and 0 edges.
 * param V the number of vertices
 * @throws  if std::length_error <tt>V</tt> < 0
 */

FlowNetwork(int v);
  
  /**  
   * Initializes a flow network from an input stream.
   * The format is the number of vertices <em>V</em>,
   * followed by the number of edges <em>E</em>,
   * followed by <em>E</em> pairs of vertices and edge capacities,
   * with each entry separated by whitespace.
   * @param in the input stream
   * @throws std::out_of_range if the endpoints of any edge are not in 
   *      the prescribed range or if the number of vertices or edges is 
   *      negative.
   */
  
/* Commenting out this constructor used for testing due to inane complaints
   from Coverity 
   
   FlowNetwork::FlowNetwork(std::fstream& in){FlowNetwork(std::fstream& in);
*/

/**
 * Returns the number of vertices in the edge-weighted graph.
 * @return the number of vertices in the edge-weighted graph
 */
 
int V() const noexcept;

/**
 * Returns the number of edges in the edge-weighted graph.
 * @return the number of edges in the edge-weighted graph
 */
  
int E() const noexcept;

/**
 * Adds the edge <tt>e</tt> to the network.
 * @param e the edge
 * @throws std::out_of_range unless endpoints of edge are between 0 and V-1
 */
 
void addEdge(spFE e);


/**
 * Returns the edges incident on vertex <tt>v</tt> (includes both edges 
 pointing to and from <tt>v</tt>).
 * @return the edges incident on vertex <tt>v</tt> as a FlowEdgeBag 
 * @param v the vertex
 * @throws std::out_of_range unless 0 <= v < V
 */
FlowEdgeBag adj(int v) const;

// return list of all edges - including self loops

std::vector<spFE> edges() const;

 private:

  void validateVertex(int v) const;
  
  int m_v;  // number of vertices
  int m_e;
  std::vector<FlowEdgeBag> m_adj;
};

std::ostream& operator << (std::ostream& out, const FlowNetwork& n);

#endif
