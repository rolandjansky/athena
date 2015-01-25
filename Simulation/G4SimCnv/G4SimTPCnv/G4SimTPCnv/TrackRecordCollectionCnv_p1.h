/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "G4SimTPCnv/TrackRecordCollection_p1.h"
#include "TrackRecordCnv_p1.h"

typedef TPCnvStdVector< TrackRecordCollection, TrackRecordCollection_p1, TrackRecordCnv_p1 >  TrackRecordCollectionCnv_p1;

