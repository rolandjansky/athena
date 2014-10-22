/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaMVACalib/BDT.h"
#include "egammaMVACalib/Node.h"

#include "TMVA/MethodBDT.h"
#include "TMVA/DecisionTree.h"
#include "TTree.h"

using namespace egammaMVACalibNmsp; 

/** c-tor from TTree **/
BDT::BDT(TTree *tree)
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
	int j = 0;
	m_forest.push_back( Node::newNode(*vars, *values, j) );
    }
}

/** c-tor from TMVA::MethodBDT **/
BDT::BDT(TMVA::MethodBDT* bdt)
{
    assert(bdt);
    m_offset = bdt->GetBoostWeights().size() ? bdt->GetBoostWeights()[0] : 0.;
    std::vector<TMVA::DecisionTree*>::const_iterator it;
    for(it = bdt->GetForest().begin(); it != bdt->GetForest().end(); ++it)
	m_forest.push_back( new Node( (*it)->GetRoot()) );
}

BDT::~BDT()
{
    std::vector<Node*>::iterator it;
    for(it = m_forest.begin(); it != m_forest.end(); ++it)
	delete *it;
}

/** Return offset + the sum of the response of each tree  **/
float BDT::GetResponse(const std::vector<float>& values) const
{
    float result = m_offset;
    std::vector<Node*>::const_iterator it;
    for (it = m_forest.begin(); it != m_forest.end(); ++it)
	result += (*it)->GetResponse(values);
    return result;
}

/** Return offset + the sum of the response of each tree  **/
float BDT::GetResponse(const std::vector<float*>& pointers) const
{
    float result = m_offset;
    std::vector<Node*>::const_iterator it;
    for (it = m_forest.begin(); it != m_forest.end(); ++it)
	result += (*it)->GetResponse(pointers);
    return result;
}

/** Return the values corresponding to m_pointers (or an empty vector) **/
std::vector<float> BDT::GetValues() const
{
    std::vector<float> result;
    std::vector<float*>::const_iterator it;
    for (it = m_pointers.begin(); it != m_pointers.end(); ++it)
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
    
    std::vector<Node*>::iterator it;
    for(it = m_forest.begin(); it != m_forest.end(); ++it)
    {
	vars.clear();
	values.clear();
	(*it)->AddToVectors(vars, values);
	tree->Fill();
    }
    return tree;
}
