/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackSegment_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSEGMENT_TRACKSEGMENT_P1_H
#define TRKSEGMENT_TRACKSEGMENT_P1_H

//-----------------------------------------------------------------------------
//
// file:   TrackSegment_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolUtilities/TPObjRef.h"

namespace Trk {

  class TrackSegment_p1 {
    
    public:
      TrackSegment_p1() {};

      TPObjRef m_segment; // Segment.h base class      

      TPObjRef m_associatedSurface;  // mutable const Surface**

};
}

#endif // TRKSEGMENT_TRACKSEGMENT_P1_H


