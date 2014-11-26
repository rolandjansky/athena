/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
/// not needed with pragma once ? 
#ifndef TrigSteeringEvent_TrigPassBitsCollection_h
#define TrigSteeringEvent_TrigPassBitsCollection_h

#include "AthContainers/DataVector.h"
#include "xAODCore/BaseInfo.h"

#ifndef TrigSteeringEvent_TrigPassBits_h
#include "TrigSteeringEvent/TrigPassBits.h"
#endif

class TrigPassBitsCollection : public DataVector<TrigPassBits> {
};

CLASS_DEF( TrigPassBitsCollection , 1311696963 , 1 )

SG_BASE(TrigPassBitsCollection, DataVector<TrigPassBits>);

// not needed with prgma once ?
#endif // TrigSteeringEvent_TrigPassBitsCollection_h
