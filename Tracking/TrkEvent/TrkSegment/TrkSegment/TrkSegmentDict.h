// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRKSEGMENT_TRKSEGMENTDICT_H
#define TRKSEGMENT_TRKSEGMENTDICT_H

// Local include(s).
#include "TrkSegment/SegmentCollection.h"
#include "TrkSegment/TrackSegment.h"
#include "TrkSegment/Segment.h"

// EDM include(s).
#include "AthContainers/tools/AuxTypeVectorFactory.h"
#include "AthLinks/ElementLink.h"

class TrkSegmentDummyDict {
  class RIO_onTrack;
	Trk::SegmentCollection m_mySegColl;
	Trk::TrackSegment m_myTempSeg;
  std::vector<const Trk::RIO_OnTrack*> m_myRioVec;
  ElementLink< Trk::SegmentCollection >                               m_i3;
  std::vector<ElementLink< Trk::SegmentCollection > >                 m_i4;
  SG::AuxTypeVectorFactory< ElementLink< Trk::SegmentCollection > >   m_i5;
};

#endif // TRKSEGMENT_TRKSEGMENTDICT_H
