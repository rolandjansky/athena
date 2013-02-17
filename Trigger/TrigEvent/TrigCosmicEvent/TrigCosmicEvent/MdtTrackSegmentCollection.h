/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __MdtTrackSegmentCollection_h__
#define __MdtTrackSegmentCollection_h__
/*
  MdtTrackSegmentCollection.h
*/
#include "TrigCosmicEvent/MdtTrackSegment.h"
#include "SGTools/BaseInfo.h"

class MdtTrackSegmentCollection : public DataVector<MdtTrackSegment> {
};

// typedef DataVector<MdtTrackSegment> MdtTrackSegmentCollection;
CLASS_DEF( DataVector<MdtTrackSegment> , 201249465 , 1 )
CLASS_DEF( MdtTrackSegmentCollection , 1334567292 , 2 )

SG_BASE(MdtTrackSegmentCollection, DataVector<MdtTrackSegment>);

#endif // __MdtTrackSegmentCollection_h__
