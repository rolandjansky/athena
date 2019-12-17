/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MVAUtils_TMVATOMVAUtils_H
#define MVAUtils_TMVATOMVAUtils_H

#include "MVAUtils/BDT.h"
#include "MVAUtils/ForestTMVA.h"
#include <stack>
#include "TMVA/Reader.h"
#include "TMVA/MethodBDT.h"

namespace TMVAToMVAUtils{
void newTree(MVAUtils::ForestTMVA* forest, 
             const TMVA::DecisionTreeNode *node, 
             float weight, 
             bool isRegression, 
             bool useYesNoLeaf)
{
  int max_var=0;
  // index is relative to the current node
  std::vector<MVAUtils::index_t> right;
  {
    // not strictly parent if doing a right node
    std::stack<const TMVA::DecisionTreeNode *> parent;
    std::stack<MVAUtils::index_t> parentIndex;
    parentIndex.push(-1);
    parent.push(nullptr);
    auto currNode = node;
    int i = -1;
    while (currNode) {
      ++i;
      right.push_back(-1);
      if (!currNode->GetLeft()) {
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
    std::vector<MVAUtils::NodeTMVA> nodes;
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

        if (currNode->GetSelector() >max_var) { max_var = currNode->GetSelector(); }

        nodes.emplace_back(currNode->GetSelector(), currNode->GetCutValue(), right[i]);

        currNode = currNode->GetCutType() ? currNode->GetRight() : currNode->GetLeft();
      }
    }
    forest->newTree(nodes, weight);
  }
   forest->setNVars(max_var);
}

std::unique_ptr<MVAUtils::ForestTMVA>createForestTMVA ( TMVA::MethodBDT* bdt, 
                                                        bool isRegression , 
                                                        bool useYesNoLeaf){
  auto forest=std::make_unique<MVAUtils::ForestTMVA>();
  std::vector<TMVA::DecisionTree*>::const_iterator it;
  for(it = bdt->GetForest().begin(); it != bdt->GetForest().end(); ++it) {
    uint index = it - bdt->GetForest().begin();
    float weight = 0.;
    if(bdt->GetBoostWeights().size() > index) {
      weight = bdt->GetBoostWeights()[index];
    }

    newTree(forest.get(),(*it)->GetRoot(), weight, isRegression, useYesNoLeaf);
  }
  return forest;
}

std::unique_ptr<MVAUtils::BDT> convert(TMVA::MethodBDT* bdt, 
                                       bool isRegression = true, 
                                       bool useYesNoLeaf = false){

  std::unique_ptr<MVAUtils::IForest> forest= createForestTMVA(bdt,isRegression,useYesNoLeaf);
  return std::make_unique<MVAUtils::BDT> (std::move(forest));
}  
}
#endif
