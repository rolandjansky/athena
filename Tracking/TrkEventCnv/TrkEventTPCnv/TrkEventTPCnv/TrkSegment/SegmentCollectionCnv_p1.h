/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SEGMENT_COLLECTION_CNV_P1_H
#define SEGMENT_COLLECTION_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   SegmentCollectionCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkSegment/SegmentCollection.h"
#include "TrkEventTPCnv/TrkSegment/SegmentCollection_p1.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkSegment/Segment.h"

class MsgStream;

class SegmentCollectionCnv_p1
   : public T_AthenaPoolTPCnvBase< Trk::SegmentCollection, Trk::SegmentCollection_p1 >
{
public:
  SegmentCollectionCnv_p1() {}

// remark: the usual templated needed to be overwritten in this case because
// 1) SegmentCollection is DV< Segments >
// 2) but Segment and TrkSegment have their T/P entry point at MeasurementBase which
//    is their highers abstract base class
 virtual void transToPers(const Trk::SegmentCollection* transVect, Trk::SegmentCollection_p1* persVect, MsgStream &log) {
     persVect->clear();
     persVect->reserve( transVect->size() );
     // convert vector entries one by one
     for( Trk::SegmentCollection::const_iterator it = transVect->begin();
           it != transVect->end();  ++it ) {
        persVect->push_back( toPersistent( (ITPConverterFor<Trk::MeasurementBase>**)0, *it, log ) );
     } 
  }
  
  
 virtual void persToTrans(const Trk::SegmentCollection_p1* persVect, Trk::SegmentCollection* transVect, MsgStream &log)
 {
     transVect->clear();
     transVect->reserve( persVect->size() );
     // convert vector entries one by one
     for( Trk::SegmentCollection_p1::const_iterator it  = persVect->begin();
                                                    it != persVect->end();  ++it )
     {
       transVect->push_back( dynamic_cast<Trk::Segment*>( createTransFromPStore( (ITPConverterFor<Trk::MeasurementBase>**)0, *it, log ) ) );
     }
 }

};


#endif 
