///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ZDC_SimStripHit_Collection_p1.h 
// Persistent represenation of a ZDC_SimStripHit_Collection
/////////////////////////////////////////////////////////////////// 
#ifndef ZDC_SIMSTRIPHIT_COLLECTION_P1_H
#define ZDC_SIMSTRIPHIT_COLLECTION_P1_H

#include "ZdcEventTPCnv/ZDC_SimStripHit_p1.h"
#include <vector>

class ZDC_SimStripHit_Collection_p1 : public std::vector<ZDC_SimStripHit_p1>
{
public:
    /// typedefs
    typedef std::vector<ZDC_SimStripHit_p1> StripHitVector;
    typedef StripHitVector::const_iterator  const_iterator;
    typedef StripHitVector::iterator        iterator;

    /// Default constructor
    ZDC_SimStripHit_Collection_p1 () {}
      
private:
};

#endif
