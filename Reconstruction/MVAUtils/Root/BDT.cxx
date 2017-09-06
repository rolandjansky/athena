/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MVAUtils/BDT.h"

#include "TMVA/MethodBDT.h"
#include "TMVA/DecisionTree.h"
#include "TTree.h"
#include <stack>

// for debugging:
#include <iostream>

using namespace MVAUtils; 


/** c-tor from TTree **/
BDT::BDT(TTree *tree)
  : m_sumWeights(0)
{
    std::vector<int> *vars = 0;
    std::vector<float> *values = 0;
    
    tree->SetBranchAddress("offset", &m_offset);
    tree->SetBranchAddress("vars", &vars);
    tree->SetBranchAddress("values", &values);
    
    for (int i = 0; i < tree->GetEntries(); ++i)
    {
	tree->GetEntry(i);
	assert (vars);
	assert (values);
	m_forest.push_back(m_nodes.size());
	newTree(*vars, *values);
	m_weights.push_back(m_offset);
	m_sumWeights+=m_offset;
    }
    
    m_offset = m_weights[0];//original use of m_offset

    delete vars;
    delete values;

    // // For Debug
    // std::cout << "Constructed from a TTree" << std::endl;
    // PrintForest();

}

/** c-tor from TMVA::MethodBDT **/
BDT::BDT(TMVA::MethodBDT* bdt, bool isRegression, bool useYesNoLeaf)
 : m_sumWeights(0)
{
    assert(bdt);
    m_offset = bdt->GetBoostWeights().size() ? bdt->GetBoostWeights()[0] : 0.;
    std::vector<TMVA::DecisionTree*>::const_iterator it;
    for(it = bdt->GetForest().begin(); it != bdt->GetForest().end(); ++it) {
      m_forest.push_back(m_nodes.size());
      uint index=it - bdt->GetForest().begin();
      if( bdt->GetBoostWeights().size() > index ) {
	m_weights.push_back( bdt->GetBoostWeights()[index]);
	m_sumWeights+=m_weights.back();
      }
      else m_weights.push_back(0);
      newTree((*it)->GetRoot(), isRegression, useYesNoLeaf);      
    }
    // // For Debug
    // std::cout << "Constructed from a MethodBDT" << std::endl;
    // PrintForest();
}


/** Creates the full tree structure from what is stored in root file **/
void BDT::newTree(const std::vector<int>& vars, const std::vector<float>& values)
{
  assert ( vars.size() == values.size());

  // parent index is relative to root of tree (and only used inside this function)
  // right index is relative to the then processed node
  std::vector<Node::index_t> right(vars.size());
  std::stack<Node::index_t> parent; // not strictly parent if doing a right node

  parent.push(-1);
  for (size_t i = 0; i < vars.size(); ++i) {
    if (vars.at(i) >= 0) { // not a leaf
      parent.push(i);
    } else {
      // a leaf
      auto currParent = parent.top();
      // if right has not been visited, next will be right
      if (currParent >= 0) {
	right[currParent] = i+1-currParent;
      }
      parent.pop();
    }
  }

  for (size_t i = 0; i < vars.size(); ++i) {
    //std::cout << "    i = " << i << ", vars = " << vars[i] << ", values = " << values[i] << ", right = " <<  right[i] << std::endl;
    m_nodes.emplace_back(vars[i], values[i], right[i]);
  }
}

/**
 * Creates the full tree structure from TMVA::DecisionTree node.
 **/
void BDT::newTree(const TMVA::DecisionTreeNode *node, bool isRegression, bool useYesNoLeaf)
{

  // index is relative to the current node
  std::vector<Node::index_t> right;
  {

    // not strictly parent if doing a right node
    std::stack<const TMVA::DecisionTreeNode *> parent; 
    std::stack<Node::index_t> parentIndex;
    
    parentIndex.push(-1);
    parent.push(nullptr);
    
    auto currNode = node;
    int i = -1;
    while (currNode) {
      ++i;
      right.push_back(-1);
      if (!currNode->GetLeft()){
	// a leaf
	auto currParent = parent.top();
	auto currParentIndex = parentIndex.top();
	// if right has not been visited, next will be right
	if (currParentIndex >= 0) {
	  right[currParentIndex] = i + 1 - currParentIndex;
	  currNode = currParent->GetCutType() ? currParent->GetLeft() : currParent->GetRight();
	} else {
	  currNode = nullptr;
	}
	parent.pop();
	parentIndex.pop();
      } else {
	// not a leaf
	parent.push(currNode);
	parentIndex.push(i);
	currNode = currNode->GetCutType() ? currNode->GetRight() : currNode->GetLeft();
      }
    }
  }
  {
    std::stack<const TMVA::DecisionTreeNode *> parent; // not strictly parent if doing a right node
    
    parent.push(nullptr);

    auto currNode = node;
    int i = -1;
    while (currNode) {
      ++i;
      if (!currNode->GetLeft()){
	// a leaf
	m_nodes.emplace_back(-1, 
			     isRegression ? 
			     currNode->GetResponse() : useYesNoLeaf ? currNode->GetNodeType() : currNode->GetPurity(), 
			     right[i]);
	auto currParent = parent.top();
	// if right has not been visited, next will be right
	if (currParent) {
	  currNode = currParent->GetCutType() ? currParent->GetLeft() : currParent->GetRight();
	} else {
	  currNode = nullptr;
	}
	parent.pop();
      } else {
	// not a leaf
	parent.push(currNode);
	m_nodes.emplace_back(currNode->GetSelector(), currNode->GetCutValue(), right[i]);
	
	currNode = currNode->GetCutType() ? currNode->GetRight() : currNode->GetLeft();
      }
    }
  }
}

/** Return offset + the sum of the response of each tree  **/
float BDT::GetResponse(const std::vector<float>& values) const
{
    float result = m_offset;
    for (auto it = m_forest.begin(); it != m_forest.end(); ++it)
      result += GetTreeResponse(values, *it);
    return result;
}

/** Return offset + the sum of the response of each tree  **/
float BDT::GetResponse(const std::vector<float*>& pointers) const
{
    float result = m_offset;
    for (auto it = m_forest.begin(); it != m_forest.end(); ++it)
      result += GetTreeResponse(pointers, *it);
    return result;
}

float BDT::GetClassification(const std::vector<float*>& pointers) const
{
    float result = 0;
    for (auto it = m_forest.begin(); it != m_forest.end(); ++it){
      uint index = it-m_forest.begin();
      result += GetTreeResponse(pointers, *it) * m_weights[index];
    }
    return result/m_sumWeights;
}

/** Return 2.0/(1.0+exp(-2.0*sum))-1, with no offset  **/
float BDT::GetGradBoostMVA(const std::vector<float>& values) const
{
  float sum = 0; // ignores the offset
  for (auto it = m_forest.begin(); it != m_forest.end(); ++it)
    sum += GetTreeResponse(values, *it);
  return 2./(1+exp(-2*sum))-1;//output shaping for gradient boosted decision tree (-1,1)
}

/** Return 2.0/(1.0+exp(-2.0*sum))-1, with no offset  **/
float BDT::GetGradBoostMVA(const std::vector<float*>& pointers) const
{
  float sum = 0; // ignores the offset
  for (auto it = m_forest.begin(); it != m_forest.end(); ++it)
    sum += GetTreeResponse(pointers, *it);
  return 2./(1+exp(-2*sum))-1;//output shaping for gradient boosted decision tree (-1,1)
}

/** special function when there are mutliple classes (for b-tagging) **/
std::vector<float> BDT::GetMultiResponse(const std::vector<float>& values, 
					 unsigned int numClasses) const
{
  std::vector<float> sum;
  std::vector<float> v_out;
  if (numClasses > 0) {
    sum.resize(numClasses); // ignores the offset
    for (unsigned int i = 0; i < m_forest.size(); ++i) {
      sum[i%numClasses] += GetTreeResponse(values, m_forest[i]);
    }

    for (unsigned icl=0; icl < numClasses; icl++) {
      float norm=0;
      for (unsigned jcl=0; jcl < numClasses; jcl++) {
    	if (icl!=jcl) norm += exp(sum[jcl]-sum[icl]);
      }
      v_out.push_back(1/(1+norm));
    }
  }
  return v_out;
}

/** special function when there are mutliple classes (for b-tagging) **/
std::vector<float> BDT::GetMultiResponse(const std::vector<float*>& pointers, 
					 unsigned int numClasses) const
{
  std::vector<float> sum;
  std::vector<float> v_out;
  if (numClasses > 0) {
    sum.resize(numClasses); // ignores the offset
    for (unsigned int i = 0; i < m_forest.size(); ++i) {
      sum[i%numClasses] += GetTreeResponse(pointers, m_forest[i]);
    }

    for (unsigned icl=0; icl < numClasses; icl++) {
      float norm=0;
      for (unsigned jcl=0; jcl < numClasses; jcl++) {
    	if (icl!=jcl) norm += exp(sum[jcl]-sum[icl]);
      }
      v_out.push_back(1/(1+norm));
    }
  }
  return v_out;
}

std::vector<float> BDT::GetMultiResponse(unsigned int numClasses) const {
  if (m_pointers.size()) {
    return GetMultiResponse(m_pointers, numClasses);
  } else {
    return std::vector<float> (numClasses, -9999.);
  }
}

/** Return the response of the response of each at index "index"  **/
float BDT::GetTreeResponse(const std::vector<float>& values, Node::index_t index) const
{
  while (!m_nodes[index].IsLeaf()) {
    index = m_nodes[index].GetNext(values[m_nodes[index].GetVar()], index);
  }
  return m_nodes[index].GetVal();
}

/** Return the response of the response of each at index "index"  **/
float BDT::GetTreeResponse(const std::vector<float*>& pointers, Node::index_t index) const
{
  while (!m_nodes[index].IsLeaf()) {
    index = m_nodes[index].GetNext(*(pointers[m_nodes[index].GetVar()]), index);
  }
  return m_nodes[index].GetVal();
}

/** Return the values corresponding to m_pointers (or an empty vector) **/
std::vector<float> BDT::GetValues() const
{
    std::vector<float> result;
    for (auto it = m_pointers.begin(); it != m_pointers.end(); ++it)
    {
	float *ptr = *it;
	assert (ptr);
	assert (*ptr);
	result.push_back(*ptr);
    }
    return result;
}

/** Return a TTree representing the BDT:
  * each entry is a binary tree, each element of the vectors is a node
  **/
TTree* BDT::WriteTree(TString name)
{
    std::vector<int> vars;
    std::vector<float> values;
    
    TTree *tree = new TTree(name.Data(), "BDT tree");
    tree->Branch("offset", &m_offset);
    tree->Branch("vars", &vars);
    tree->Branch("values", &values);

    auto forSize = m_forest.size();
    auto nodeSize = m_nodes.size();
    for (size_t i = 0; i < forSize; ++i) {
      vars.clear();
      values.clear();
      auto beg = m_forest[i];
      auto end = static_cast<Node::index_t>(nodeSize);
      if (i+1 < forSize) end = m_forest[i+1];
      for(auto j = beg; j < end; ++j) {
	vars.push_back(m_nodes[j].GetVar());
	values.push_back(m_nodes[j].GetVal());
      }
      m_offset = m_weights[i];
      tree->Fill();
    }	
    return tree;
}

/** For debugging only:
  * print the forest
  **/
void BDT::PrintForest() const
{
  std::cout << "***BDT: Printing entire forest***" << std::endl; 
  std::cout << "offset: " << m_offset << std::endl; 
  for (size_t i = 0; i < m_forest.size(); i++) {
    std::cout << "Tree number: " << i << std::endl; 
    PrintTree(m_forest[i]);
  }
    
}

/** For debugging only:
  * Print the tree in a way that can compare implementations
  * Using pre-order search for now
  **/
void BDT::PrintTree(Node::index_t index) const
{
  std::stack<Node::index_t> s;
  s.push(index);
  while (!s.empty()) {
    auto node = s.top();
    s.pop();
    m_nodes.at(node).Print(node);
    if (!m_nodes[node].IsLeaf()) {
      s.push(m_nodes[node].GetRight(node));
      s.push(m_nodes[node].GetLeft(node));
    }
  }
}
