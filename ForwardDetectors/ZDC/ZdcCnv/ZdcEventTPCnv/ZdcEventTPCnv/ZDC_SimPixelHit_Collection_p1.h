///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ZDC_SimPixelHit_Collection_p1.h 
// Persistent represenation of a ZDC_SimPixelHit_Collection
/////////////////////////////////////////////////////////////////// 
#ifndef ZDC_SIMPIXELHIT_COLLECTION_P1_H
#define ZDC_SIMPIXELHIT_COLLECTION_P1_H

#include "ZdcEventTPCnv/ZDC_SimPixelHit_p1.h"
#include <vector>

class ZDC_SimPixelHit_Collection_p1 : public std::vector<ZDC_SimPixelHit_p1>
{
public:
    /// typedefs
    typedef std::vector<ZDC_SimPixelHit_p1> PixelHitVector;
    typedef PixelHitVector::const_iterator  const_iterator;
    typedef PixelHitVector::iterator        iterator;

    /// Default constructor
    ZDC_SimPixelHit_Collection_p1 () {}
      
private:
};

#endif
