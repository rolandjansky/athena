/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrackRecordCollection_H
#define TrackRecordCollection_H

#include "TrackRecord/TrackRecord.h"
#include "HitManagement/AthenaHitsVector.h"

#ifndef __CINT__
#include "CLIDSvc/CLASS_DEF.h"
#endif

typedef AthenaHitsVector<TrackRecord> TrackRecordCollection;
typedef AthenaHitsVector<TrackRecord>::iterator TrackRecordIterator;
typedef AthenaHitsVector<TrackRecord>::const_iterator TrackRecordConstIterator;

#ifndef __CINT__
CLASS_DEF (TrackRecordCollection, 1235574503, 1)
#endif


#endif
