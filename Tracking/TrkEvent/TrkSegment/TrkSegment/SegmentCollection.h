/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKSEGMENT_SEGMENTCOLLECTION_H
#define TRKSEGMENT_SEGMENTCOLLECTION_H

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "TrkSegment/Segment.h"

namespace Trk {
  typedef DataVector<Trk::Segment> SegmentCollection;
}

CLASS_DEF(Trk::SegmentCollection, 98849495, 1)

#endif
