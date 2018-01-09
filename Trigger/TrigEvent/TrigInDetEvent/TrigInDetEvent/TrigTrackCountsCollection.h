/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETEVENT_TRIGTRACKCOUNTSCOLLECTION_H
#define TRIGINDETEVENT_TRIGTRACKCOUNTSCOLLECTION_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"
#include "TrigInDetEvent/TrigTrackCounts.h"
#include "AthenaKernel/BaseInfo.h"

class TrigTrackCountsCollection : public DataVector<TrigTrackCounts> { }; 

CLASS_DEF( TrigTrackCountsCollection , 1117551094 , 1 )

SG_BASE(TrigTrackCountsCollection, DataVector<TrigTrackCounts>);

#endif
