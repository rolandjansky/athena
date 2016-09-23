/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Author: Noel Dawe (Noel%dot%Dawe%at%cern%dot%ch)
 */

#ifndef CUTSDECISIONTREE_H
#define CUTSDECISIONTREE_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include "TauDiscriminant/Node.h"
#include "TauDiscriminant/TreeVector.h"

//using namespace std;

class CutsDecisionTree: public TreeVector {

    public:

        //!< Default Constructor
        CutsDecisionTree() {}

        float response(unsigned int level) const;
};

#endif
