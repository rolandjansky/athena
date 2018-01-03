/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETEVENT_TRIGVERTEXCOLLECTION_H
#define TRIGINDETEVENT_TRIGVERTEXCOLLECTION_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"
#include "TrigInDetEvent/TrigVertex.h"
#include "AthenaKernel/BaseInfo.h"

class TrigVertexCollection: public DataVector< TrigVertex > {};
//typedef   DataVector<TrigVertex>  TrigVertexCollection;

// obtained using ./clid -m TrigVertexCollection

CLASS_DEF( TrigVertexCollection , 1299073032 , 1 )
CLASS_DEF( DataVector<TrigVertexCollection> , 1089493875 , 1 )

SG_BASE(TrigVertexCollection, DataVector<TrigVertex>);

#endif // TRIGINDETEVENT_TRIGVERTEXCOLLECTION_H
