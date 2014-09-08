/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SEGMENT_COLLECTION_P1_H
#define SEGMENT_COLLECTION_P1_H

//-----------------------------------------------------------------------------
//
// file:   SegmentCollection_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

namespace Trk
{
   class SegmentCollection_p1
      : public std::vector<TPObjRef>
   {
  public:
     SegmentCollection_p1() {}
   };
}

#endif // SEGMENT_COLLECTION_P1_H
