///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ZdcDigitsCollection_p1.h 
// Persistent represenation of a ZdcDigitsCollection
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef ZDCDIGITSCOLLECTION_P1_H
#define ZDCDIGITSCOLLECTION_P1_H

#include "ZdcEventTPCnv/ZdcDigits_p1.h"
#include <vector>

class ZdcDigitsCollection_p1 : public std::vector<ZdcDigits_p1>
{
public:
    /// typedefs
    typedef std::vector<ZdcDigits_p1> ElemVector;
    typedef ElemVector::const_iterator const_iterator;
    typedef ElemVector::iterator       iterator;

    /// Default constructor
    ZdcDigitsCollection_p1 () {}

      
private:
};

#endif
