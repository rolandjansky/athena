/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_FLOWEDGE_H
#define TRIGHLTJETHYPO_FLOWEDGE_H


// ********************************************************************
//
// NAME:     FlowEdge.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR: P Sherwood
//
// ********************************************************************
//


#include <ostream>

//////////////////////////////////////

/**
 * FordFulkerson Algorithm - used, for eaxmple,for multiple eta regions,
 * shere each reagion may have > 1 threshold, and the regions do not
 * share jets
 *
 * The FlowEdge and FlowNetwork classes are used by the FF algorithm.
 */


/** FlowEdge: Adapted from Algorithms, Sedgewick and Wayne, 4th edition.

 *  The <tt>FlowEdge</tt> class represents a capacitated edge with a 
 *  flow in a {@link FlowNetwork}. Each edge consists of two integers
 *  (naming the two vertices), a real-valued capacity, and a real-valued
 *  flow. The data type provides methods for accessing the two endpoints
 *  of the directed edge and the weight. It also provides methods for
 *  changing the amount of flow on the edge and determining the residual
 *  capacity of the edge.
 *  <p>
 *  For additional documentation, 
 *  see <a href="http://algs4.cs.princeton.edu/64maxflow">Section 6.4</a> of
 *  <i>Algorithms, 4th Edition</i> by Robert Sedgewick and Kevin Wayne.


 */

class FlowEdge {
    /**
     * Initializes an edge from vertex <tt>v</tt> to vertex <tt>w</tt> with
     * the given <tt>capacity</tt> and zero flow.
     * @param v the tail vertex
     * @param w the head vertex
     * @param capacity the capacity of the edge
     * @throws std::out_of_range if either <tt>v</tt> or <tt>w</tt> is a 
     *   negative integer
     * @throws std::invalid_argument if <tt>capacity</tt> is negative
     */

 public:
  
  FlowEdge();
  FlowEdge(int v, int w, double capacity);

  /**
   * Returns the tail vertex of the edge.
   * @return the tail vertex of the edge
   */
  int from() const noexcept;

  /**
   * Returns the head vertex of the edge.
   * @return the head vertex of the edge
   */
  int to() const noexcept;

  double capacity() const noexcept;

  double flow() const noexcept;

  /**
   * Returns the endpoint of the edge that is different from the given vertex
   * (unless the edge represents a self-loop in which case it
   *     returns the same vertex).
   * @param vertex one endpoint of the edge
   * @return the endpoint of the edge that is different from the given vertex
   *   (unless the edge represents a self-loop in which case it returns the 
   *      same vertex)
   * @throws std::invalid_argument if <tt>vertex</tt> is not one of the 
   *     endpoints of the edge
   */
  double other(int vertex) const;

  /**
   * Returns the residual capacity of the edge in the direction
   *  to the given <tt>vertex</tt>.
   * @param vertex one endpoint of the edge
   * @return the residual capacity of the edge in the direction to the given
   *   vertex
   *   If <tt>vertex</tt> is the tail vertex, the residual capacity equals
   *   <tt>capacity() - flow()</tt>; if <tt>vertex</tt> is the head vertex, the
   *   residual capacity equals <tt>flow()</tt>.
   * @throws std::invalid_argument if <tt>vertex</tt> is not one of the 
   *   endpoints of the edge
   */
  double residualCapacityTo(int vertex) const;
  void addResidualFlowTo(int vertex, double delta);

 private:
  int m_v;  // from
  int m_w;  // to 
  double m_capacity;
  double m_flow;
};

std::ostream& operator << (std::ostream& out, const FlowEdge& e);
bool operator < (const FlowEdge& lhs, const FlowEdge& rhs);

#endif
