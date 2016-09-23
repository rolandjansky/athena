/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * file: BoostedDecisionTree.cxx
 *
 * Author: Noel Dawe (end1@sfu.ca)
 */

#include "TauDiscriminant/BoostedDecisionTree.h"

using namespace std;

float BoostedDecisionTree::response() const {

    float sum = 0.;
    float norm = 0.;
    Node* currentNode = 0;
    DecisionNode* decision = 0;
    LeafNode<float>* leafNode = 0;
    vector<pair<Node*,float> >::const_iterator tree = this->m_trees.begin();
    while(tree != this->m_trees.end()) {
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
        sum += ((*tree).second)*leafNode->getValue();
        norm += (*tree).second;
        ++tree;
    }
    return norm > 0. ? sum/norm : -100.;
}
