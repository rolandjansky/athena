/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MVAUtils_TMVATOMVAUtils_H                                                                                                  
#define MVAUtils_TMVATOMVAUtils_H                                                                                                 

#include "MVAUtils/BDT.h"
#include <stack>
#include "TMVA/Reader.h"
#include "TMVA/MethodBDT.h"

namespace TMVAToMVAUtils{
/** Creates the full tree structure from what is stored in root file **/
void newTree(const TMVA::DecisionTreeNode *node, 
             bool isRegression, bool useYesNoLeaf, 
             std::vector<MVAUtils::Node>& nodes)
{
  //index is relative to the current node
  std::vector<MVAUtils::Node::index_t> right;
  {

    // not strictly parent if doing a right node
    std::stack<const TMVA::DecisionTreeNode *> parent; 
    std::stack<MVAUtils::Node::index_t> parentIndex;
                    
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
        nodes.emplace_back(-1, 
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
        nodes.emplace_back(currNode->GetSelector(), currNode->GetCutValue(), right[i]);

        currNode = currNode->GetCutType() ? currNode->GetRight() : currNode->GetLeft();
      }
    }
  }
}

std::unique_ptr<MVAUtils::BDT> convert(TMVA::MethodBDT* bdt, bool isRegression, bool useYesNoLeaf){

  float sumWeights=0; 
  float offset=bdt->GetBoostWeights().size() ? bdt->GetBoostWeights()[0] : 0.;  
  std::vector<MVAUtils::Node::index_t> forest;
  std::vector<float> weights;
  std::vector<MVAUtils::Node> nodes;
  
  /* Build the BDT*/
  std::vector<TMVA::DecisionTree*>::const_iterator it;
  for(it = bdt->GetForest().begin(); it != bdt->GetForest().end(); ++it) {
    forest.push_back(nodes.size());
    uint index=it - bdt->GetForest().begin();
    if( bdt->GetBoostWeights().size() > index ) {
      weights.push_back( bdt->GetBoostWeights()[index]);
      sumWeights+=weights.back();
    }
    else {
      weights.push_back(0);
    }
    newTree((*it)->GetRoot(), isRegression, useYesNoLeaf,nodes);      
  }
  std::unique_ptr<MVAUtils::BDT> MVAUtils_bdt;
  MVAUtils_bdt->SetOffset(offset);
  MVAUtils_bdt->SetSumWeights(sumWeights); 
  MVAUtils_bdt->SetForest(forest); 
  MVAUtils_bdt->SetWeights(weights);
  MVAUtils_bdt->SetNodes(nodes); 
  return MVAUtils_bdt;
}  
}
#endif
