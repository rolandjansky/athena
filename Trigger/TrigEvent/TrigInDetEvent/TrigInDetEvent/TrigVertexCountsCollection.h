/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETEVENT_TRIGVERTEXCOUNTSCOLLECTION_H
#define TRIGINDETEVENT_TRIGVERTEXCOUNTSCOLLECTION_H

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"
#include "TrigInDetEvent/TrigVertexCounts.h"
#include "SGTools/BaseInfo.h"

class TrigVertexCountsCollection : public DataVector<TrigVertexCounts> { }; 

CLASS_DEF( TrigVertexCountsCollection , 1092392202 , 1 )

SG_BASE(TrigVertexCountsCollection, DataVector<TrigVertexCounts>);

#endif
