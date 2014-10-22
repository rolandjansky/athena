/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef egamma_BDT_H
#define egamma_BDT_H

#include<vector>
#include "TString.h"

namespace TMVA { class MethodBDT; }
class TTree;

namespace egammaMVACalibNmsp 
{
    class Node;
    
    /** Simplified Boosted Regression Tree, based on TMVA::DecisionTree.
      * Holds a forest (vector of top nodes of each decision tree) and a constant offset.
      * The response is given by offset + the sum of the response of each tree
      * Can be constructed from TMVA::MethodBDT or a TTree.
      * Each entry of the TTree represents a binary tree and each element of the
      * vectors stored in the TTree represent a node
      *
      * The response can be evaluated from a vector of floats or a vector of pointers
      * (to avoid creating vectors at each call) which can be stored internally
      * (m_pointers)
      **/
    class BDT
    {
      public:
	BDT(TTree *tree);
	BDT(TMVA::MethodBDT *bdt);
	virtual ~BDT();
	
	unsigned int GetNTrees() const { return m_forest.size(); }
	float GetOffset() const { return m_offset; }
	std::vector<egammaMVACalibNmsp::Node*>& GetForest() { return m_forest; }
	
	float GetResponse(const std::vector<float>& values) const;
	float GetResponse(const std::vector<float*>& pointers) const;
	float GetResponse() const { 
	    return (m_pointers.size() ? GetResponse(m_pointers) : -9999.); 
	}
	
	std::vector<float> GetValues() const;
	
	std::vector<float*> GetPointers() const { return m_pointers; }
	void SetPointers(std::vector<float*>& pointers) { m_pointers = pointers; }
	
	TTree* WriteTree(TString name = "BDT");
	
      private:
	float m_offset;
	std::vector<egammaMVACalibNmsp::Node*> m_forest;
	std::vector<float*> m_pointers;
	
    };
}

#endif
