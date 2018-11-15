/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrackRecordCollection_H
#define TrackRecordCollection_H

#include "TrackRecord/TrackRecord.h"
#include "HitManagement/AtlasHitsVector.h"
#include "AthenaKernel/CLASS_DEF.h"

typedef AtlasHitsVector<TrackRecord> TrackRecordCollection;
typedef AtlasHitsVector<TrackRecord>::iterator TrackRecordIterator;
typedef AtlasHitsVector<TrackRecord>::const_iterator TrackRecordConstIterator;

CLASS_DEF (TrackRecordCollection, 1235574503, 1)


#endif
