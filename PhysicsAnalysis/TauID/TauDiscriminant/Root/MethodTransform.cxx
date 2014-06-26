/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * file: MethodTransform.cxx
 *
 * Author: Noel Dawe (end1@sfu.ca)
 */

#include "TauDiscriminant/MethodTransform.h"

using namespace TauID;

float MethodTransform::response() const
{
    if (!this->isBuilt) return -101.;
    Transformation* trans = getCurrentCategory();
    return trans ? trans->response() : -201.;
}

bool MethodTransform::build(const string& filename, bool checkTree)
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

Transformation* MethodTransform::getCurrentCategory() const
{
    PointerLeafNode<Transformation>* leafNode;
    DecisionNode* decision;
    Node* currentNode = this->categoryTree;
    if (!currentNode) return 0;
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
        if (!currentNode) return 0;
    }
    leafNode = static_cast<PointerLeafNode<Transformation>*>(currentNode);
    return leafNode->getValue();
}
