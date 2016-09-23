/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * file: Transformation.cxx
 *
 * Author: Noel Dawe (end1@sfu.ca)
 */

#include "TauDiscriminant/Transformation.h"

using namespace std;

float Transformation::response() const {

    Node* currentNode = 0;
    DecisionNode* decision = 0;
    LeafNode<float>* leafNode = 0;
    vector<pair<Node*,float> >::const_iterator tree = this->m_trees.begin();
    if (tree == this->m_trees.end()) return -200.;
    currentNode = (*tree).first;
    while (!currentNode->isLeaf()) {
        decision = static_cast<DecisionNode*>(currentNode);
        if (decision->goRight()) {
            currentNode = decision->getRightChild();
        } else {
            currentNode = decision->getLeftChild();
        }
        if (!currentNode) return -200.;
    }
    leafNode = static_cast<LeafNode<float>*>(currentNode);
    return leafNode->getValue();
}
