// dear emacs, this is really -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MVAUtils_Node_H
#define MVAUtils_Node_H

#include <vector>
#include <cstdint>

namespace MVAUtils
{
  /** Node of a binary tree, defined by the index of the variable
   * (m_var, set to -1 for leaf nodes), the cut value or the response
   * (m_val) and ways to access the left and right children.
   *
   * NOTE: This class should not be used directly by any class other than
   * BDT.
   **/
  class Node
  {
  public:

    typedef int32_t index_t; //!< The index type of the node in the vector
    typedef int8_t var_t; //!< The variable type (i.e., the index of the variable to cut)

    /** The constructor gets the index of the variable to cut on (-1 if leaf),
	the index of the right child (value not used if a leaf), and the value to 
	cut on (if not leaf) or the respons (if leaf).
    */
    Node(const int ivar, const float val, const index_t right): 
      m_var(ivar), m_right(right), m_val(val) {}
	    
    bool IsLeaf() const { return m_var < 0; } //!< is the current node a leaf node

    /** Based on the value of the variable that's passed in, 
	return the index of the appropriate child. Assumes the node is not a leaf
     	(i.e., there is no check). Must also pass the current node index.
    */
    index_t GetNext(const float value, index_t index) const ;
	
    /** The variable index to cut on (or -1 if leaf, but use IsLeaf instead if checking for leaf) */
    var_t GetVar() const { return m_var; }

    /** The value to cut on (if not leaf), or the response (if leaf). */
    float GetVal() const { return m_val; }

    // User should not really need these, but useful for debugging

    /** For debugging: returns the index of the left node; is passed the current node index.
     */
    index_t GetLeft(index_t index) const {return index + 1; }
    
    /** For debugging: returns the index of the right node; is passed the current node index.
     */
    index_t GetRight(index_t index) const {return index + m_right; }

    /** For debugging only: print the node values */
    void Print(index_t index) const;

  private:

    var_t m_var; //!< index of the variable to cut for internal nodes, -1 for leaf nodes

    // note: this doesn't need to be the same type as the external index
    int16_t m_right; //!< right relative index (to be added to current) (left is always current + 1)
    float m_val; //!< cut value for internal nodes or response for leaf nodes
  };
}

#endif
