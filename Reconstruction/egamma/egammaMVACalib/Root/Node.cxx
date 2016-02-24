/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaMVACalib/Node.h"
#include <cassert>

using namespace std; // has to come before TMVA include (errors on header)
#include "TMVA/DecisionTreeNode.h"

using namespace egammaMVACalibNmsp; 

/** c-tor from TMVA::DecisionTree node. 
 * Creates the full tree structure when called from the top node 
 **/
Node::Node(TMVA::DecisionTreeNode *node) : m_left(0), m_right(0)
{
  if (!node->GetLeft())
  {
      m_var = -1;
      m_val = node->GetResponse();
  }
  else
  {
      m_var = node->GetSelector();
      m_val = node->GetCutValue();
      m_left = new Node(node->GetCutType() ? node->GetRight() : node->GetLeft() );
      m_right = new Node(node->GetCutType() ? node->GetLeft() : node->GetRight() );
  }
}

/** Recursively deletes the node and its children **/
Node::~Node()
{
    delete m_left; 
    delete m_right; 
    m_left = 0;
    m_right = 0;
}

/** Recursive function to create nodes and their children. 
    Creates the full tree structure when called with i=0 **/
Node* Node::newNode(std::vector<int>& vars, std::vector<float>& values, int& i)
{
    assert ( (int) vars.size() >= i);
    Node *node = new Node(vars.at(i), values.at(i));
    if (node->GetVar() >= 0)
    {
	node->m_left = newNode(vars, values, ++i);
	node->m_right = newNode(vars, values, ++i);
    }
    return node;
}

/** Recursive function to evaluate the response of the decision tree given a vector with
 * values for each variable
 **/
float Node::GetResponse(const std::vector<float>& values) const
{
    if (m_var < 0) return m_val;
    assert ( (int) values.size() >= m_var);
    Node *node = GetNext(values[m_var]);
    assert (node);
    return node->GetResponse(values);
}

/** Recursive function to evaluate the response of the decision tree given a vector with
 * pointers to each variable
 **/
float Node::GetResponse(const std::vector<float*>& pointers) const
{
    if (m_var < 0) return m_val;
    assert ( (int) pointers.size() >= m_var && pointers[m_var]);
    Node *node = GetNext( *(pointers[m_var]) );
    assert (node);
    return node->GetResponse(pointers);
}

/** Recursive function to add the node info to vectors **/
void Node::AddToVectors(std::vector<int>& vars, std::vector<float>& values) const
{
    vars.push_back(m_var);
    values.push_back(m_val);
    if (m_left) m_left->AddToVectors(vars, values);
    if (m_right) m_right->AddToVectors(vars, values);
}
