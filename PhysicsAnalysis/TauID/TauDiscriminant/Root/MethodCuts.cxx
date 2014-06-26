/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * file: CutsMethod.cxx
 *
 * Author: Noel Dawe (end1@sfu.ca)
 */

#include "TauDiscriminant/MethodCuts.h"

using namespace TauID;

float MethodCuts::response(unsigned int level) const
{
    if (!this->isBuilt) return 0.;
    CutsDecisionTree* cuts = getCurrentCategory();
    return cuts ? cuts->response(level) : 0.;
}

bool MethodCuts::build(const string& filename, bool checkTree)
{    
    #ifdef __STANDALONE
    TreeReader reader(this->verbose);
    #else
    TreeReader reader;
    #endif
    reader.setVariables(&this->floatVariables,&this->intVariables);
    this->categoryTree = reader.build(filename,checkTree);
    if (this->categoryTree != 0)
    {
        this->isBuilt = true;
        return true;
    }
    return false;
}

CutsDecisionTree* MethodCuts::getCurrentCategory() const
{
    PointerLeafNode<CutsDecisionTree>* leafNode;
    DecisionNode* decision;
    Node* currentNode = this->categoryTree;
    while (!currentNode->isLeaf())
    {
        decision = static_cast<DecisionNode*>(currentNode);
        if (decision->goRight())
        {
            currentNode = decision->getRightChild();
        }
        else
        {
            currentNode = decision->getLeftChild();
        }
    }
    leafNode = static_cast<PointerLeafNode<CutsDecisionTree>*>(currentNode);
    return leafNode->getValue();
}
