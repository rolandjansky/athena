/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Author: Noel Dawe (Noel%dot%Dawe%at%cern%dot%ch)
 */

#ifndef BOOSTEDDECISIONTREE_H
#define BOOSTEDDECISIONTREE_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include "TauDiscriminant/Node.h"
#include "TauDiscriminant/TreeVector.h"

//using namespace std;

class BoostedDecisionTree : public TreeVector {

    public:

        //!< Default Constructor
        BoostedDecisionTree() {}

        /**
         * @brief Returns the @c float score for the set of variables and values in @c variableMap.
         * @param variableMap
         */
        float response() const;

};

#endif
