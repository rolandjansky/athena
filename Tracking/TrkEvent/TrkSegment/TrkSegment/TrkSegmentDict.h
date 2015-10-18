/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKSEGMENT_TRKSEGMENTDICT_H
#define TRKSEGMENT_TRKSEGMENTDICT_H

#include "TrkSegment/SegmentCollection.h"
#include "TrkSegment/TrackSegment.h"
#include "TrkSegment/Segment.h"
#include "AthLinks/ElementLink.h"
#include "TrkSegment/SegmentCollection.h"

class TrkSegmentDummyDict {
  class RIO_onTrack;
	Trk::SegmentCollection m_mySegColl;
	Trk::TrackSegment m_myTempSeg;
  std::vector<const Trk::RIO_OnTrack*> m_myRioVec;
  ElementLink< Trk::SegmentCollection >                               m_i3;
  std::vector<ElementLink< Trk::SegmentCollection > >                 m_i4;
};

#endif 
