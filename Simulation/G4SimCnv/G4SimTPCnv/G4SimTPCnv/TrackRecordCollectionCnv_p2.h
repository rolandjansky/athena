/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4SIMTPCNV_TRACKRECORDCOLLECTIONCNV_P2_H
#define G4SIMTPCNV_TRACKRECORDCOLLECTIONCNV_P2_H

#include "G4SimTPCnv/TrackRecordCollection_p2.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "G4SimTPCnv/TrackRecordCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
                                                                                                                                                           
typedef T_AtlasHitsVectorCnv< TrackRecordCollection, TrackRecordCollection_p2, TrackRecordCnv_p1 >  TrackRecordCollectionCnv_p2;


#endif // not G4SIMTPCNV_TRACKRECORDCOLLECTIONCNV_P2_H
