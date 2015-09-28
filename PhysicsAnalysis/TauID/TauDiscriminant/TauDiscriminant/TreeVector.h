/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Author: Noel Dawe (Noel%dot%Dawe%at%cern%dot%ch)
 */

#ifndef TREEVECTOR_H
#define TREEVECTOR_H

#include <vector>
#include <utility>
#include "TauDiscriminant/Node.h"

using namespace std;

class TreeVector {

    public:
        
        //!< Destructor
        ~TreeVector() {
            vector<pair<Node*,float> >::iterator it(this->trees.begin());
            while (it != this->trees.end()) delete (*it++).first;
        }
       
        /**
         * @brief Adds a decision tree rooted at the node @c root with a weight of @c weight.
         * @param root a @c Node* pointer to the root node.
         * @param weight the @c float weight of the decision tree.
         */
        void addTree(Node* root, float weight) {
            this->trees.push_back(pair<Node*,float>(root,weight));
        }

    protected:

        vector<pair<Node*,float> > trees;
};

#endif
