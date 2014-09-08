/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_COLLECTION_CNV_P4_TRK_H
#define TRACK_COLLECTION_CNV_P4_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TrackCollectionCnv_p4.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkEventTPCnv/TrkTrack/TrackCollection_p1.h"

#include "TrkEventTPCnv/TrkTrack/TrackCnv_p4.h"

class MsgStream;

class TrackCollectionCnv_p4
   : public T_AthenaPoolTPPtrVectorCnv< TrackCollection, Trk::TrackCollection_p1, TrackCnv_p4 >
{
public:
  TrackCollectionCnv_p4() {}
};



#endif
