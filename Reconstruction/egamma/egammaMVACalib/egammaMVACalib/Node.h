/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef egamma_Node_H
#define egamma_Node_H

#include <vector>
namespace TMVA { class DecisionTreeNode; }

namespace egammaMVACalibNmsp
{
    /** Node of a binary tree, defined by the index of the variable 
      * (m_var, set to -1 for leaf nodes),
      * the cut value or the response (m_val) and pointers to the left
      * and right child nodes.
      * The full tree structure can be accessed from the top node and the
      * structure obeys: value > m_val -> left node, else right node.
      *
      * The class provides (recursive) functions to create, destroy and evaluate
      * the response of the complete decision or regression tree 
      * (when called from the top node).
      **/
    class Node
    {
      public:    
	Node(int ivar, float val): m_var(ivar), m_val(val), m_left(0), m_right(0) {}
	Node(TMVA::DecisionTreeNode *node);
        ~Node();
	
	float GetVal() const { return m_val; }
	float GetVar() const { return m_var; }
	Node* GetNext(float value) const { return (value > m_val) ? m_left : m_right; }
    
	float GetResponse(const std::vector<float>& values) const;
	float GetResponse(const std::vector<float*>& pointers) const;
	
	void AddToVectors(std::vector<int>& vars, std::vector<float>& values) const;
	
	static Node* newNode(std::vector<int>& vars, std::vector<float>& values, int &i);
	
      private:
	char m_var; //! index of the variable to cut for internal nodes, -1 for leaf nodes
	float m_val; //! cut value for internal nodes or response for leaf nodes
	Node *m_left; //! pointer to left child node (value > m_val)
	Node *m_right; //! pointer to right child node (value <= m_val)
    };
}

#endif
