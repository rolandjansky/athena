/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
#ifndef TrigSteeringEvent_TrigPassFlagsCollection_h
#define TrigSteeringEvent_TrigPassFlagsCollection_h

#include "AthContainers/DataVector.h"
#include "xAODCore/BaseInfo.h"

#ifndef TrigSteeringEvent_TrigPassFlags_h
#include "TrigSteeringEvent/TrigPassFlags.h"
#endif

class TrigPassFlagsCollection : public DataVector<TrigPassFlags> {
};

CLASS_DEF( TrigPassFlagsCollection , 1210268481 , 1 )

      SG_BASE(TrigPassFlagsCollection, DataVector<TrigPassFlags>);

#endif // TrigSteeringEvent_TrigPassFlagsCollection_h
