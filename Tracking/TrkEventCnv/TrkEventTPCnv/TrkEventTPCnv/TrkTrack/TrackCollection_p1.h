/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_COLLECTION_P1_TRK_H
#define TRACK_COLLECTION_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TrackCollection_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

namespace Trk
{
   class TrackCollection_p1
      : public std::vector<TPObjRef>
   {
  public:
     TrackCollection_p1() {}
   };
}

#endif // TRACK_COLLECTION_P1_TRK_H



