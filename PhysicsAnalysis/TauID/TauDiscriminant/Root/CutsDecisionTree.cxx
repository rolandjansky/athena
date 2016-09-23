/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * file: Cuts.cxx
 *
 * Author: Noel Dawe (end1@sfu.ca)
 */

#include "TauDiscriminant/CutsDecisionTree.h"

float CutsDecisionTree::response(unsigned int level) const {

    DecisionNode* decision = 0;
    LeafNode<float>* leafNode = 0;
    if (level >= this->m_trees.size()) return false;
    Node* currentNode = this->m_trees[level].first;
    while (!currentNode->isLeaf()) {
        decision = static_cast<DecisionNode*>(currentNode);
        if (decision->goRight()) {
            currentNode = decision->getRightChild();
        } else {
            currentNode = decision->getLeftChild();
        }
        if (!currentNode) return false;
    }
    leafNode = static_cast<LeafNode<float>*>(currentNode);
    return leafNode->getValue();
}
