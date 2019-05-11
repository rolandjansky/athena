/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_FORDFULKERSON_H
#define TRIGHLTJETHYPO_FORDFULKERSON_H


// ********************************************************************
//
// NAME:     TrigHLTJetHypoHelpers.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:
//
// ********************************************************************
//

#include <vector>
#include "./FlowEdgeBag.h"

/**
 * FordFulkerson Algorithm - used, for eaxmple,for multiple eta regions,
 * shere each reagion may have > 1 threshold, and the regions do not
 * share jets
 *
 * The FlowEdge and FlowNetwork classes are used by the FF algorithm.

 * FordFulkerson
 * Adapted from Algorithms, Sedgewick and Wayne, 4th edition.
 * 
 *
 *  The <tt>FordFulkerson</tt> class represents a data type for computing a
 *  <em>maximum st-flow</em> and <em>minimum st-cut</em> in a flow
 *  network.
 *  <p>
 *  This implementation uses the <em>Ford-Fulkerson</em> algorithm with
 *  the <em>shortest augmenting path</em> heuristic.
 *  The constructor takes time proportional to 
 *  <em>E V</em> (<em>E</em> + <em>V</em>)
 *  in the worst case and extra space (not including the network)
 *  proportional to <em>V</em>, where <em>V</em> is the number of vertices
 *  and <em>E</em> is the number of edges. In practice, the algorithm will
 *  run much faster.
 *  Afterwards, the <tt>inCut()</tt> and <tt>value()</tt> methods take
 *  constant time.
 *  <p>
 *  If the capacities and initial flow values are all integers, then this
 *  implementation guarantees to compute an integer-valued maximum flow.
 *  If the capacities and floating-point numbers, then floating-point
 *  roundoff error can accumulate.
 *  <p>
 *  For additional documentation, see 
 * <a href="http://algs4.cs.princeton.edu/64maxflow">Section 6.4</a>
 *  <i>Algorithms, 4th Edition</i> by Robert Sedgewick and Kevin Wayne.
 *
 */

class FlowNetwork;

class FordFulkerson{
 public:

  /**
   * Compute a maximum flow and minimum cut in the network <tt>G</tt>
   * from vertex <tt>s</tt> to vertex <tt>t</tt>.
   * @param G the flow network
   * @param s the source vertex
   * @param t the sink vertex
   *
   * Error state true if:
   * s = t
   * initial flow is infeasible
   *
   * Throws if used while in error state
   */
  
  FordFulkerson(const FlowNetwork& G, int s, int t);

  /**
   * Returns the value of the maximum flow.
   * @return the value of the maximum flow
   */
  double value() const;

  /**
   * Is vertex <tt>v</tt> on the <tt>s</tt> side of the minimum st-cut?
   * @return <tt>true</tt> if vertex <tt>v</tt> is on the <tt>s</tt> side 
   *    of the mincut, and <tt>false</tt> if vertex <tt>v</tt> is on 
   *    the <tt>t</tt> side.
   * @throws std::out_of_range unless 0 <= v < V
   */
  bool error() const {return m_hasError;}
  std::string errMsg() const {return m_errMsg;}
 private:

  bool inCut(int v);
  
  // check if v is outside prescibed range
  bool validate(int v, int V);


  void checkFeasibility(const FlowNetwork& G, int s, int t);

  // return excess flow at vertex v
  double excess(const FlowNetwork& G, int v) const noexcept;

  // check optimality conditions
  void check(const FlowNetwork& G, int s, int t);
  bool hasAugmentingPath(const FlowNetwork& G, int s, int t);

  // m_marked[v] = true iff s->v path in residual graph
  std::vector<bool> m_marked;

  // m_edgeTo[v] = last edge on shortest residual s->v path
  std::vector<spFE> m_edgeTo;

  // current value of max flow
  double m_value;

  bool m_hasError{false};
  std::string m_errMsg{""};
};

#endif
