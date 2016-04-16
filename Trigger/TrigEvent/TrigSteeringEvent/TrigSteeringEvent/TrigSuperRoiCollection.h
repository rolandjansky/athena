/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
// #ifndef  TRIGSTEERINGEVENT_TRIGSUPERROICOLLECTION_H
// #define  TRIGSTEERINGEVENT_TRIGSUPERROICOLLECTION_H

#include "AthContainers/DataVector.h"
#include "SGTools/BaseInfo.h"

#ifndef TRIGSTEERINGEVENT_TRIGSUPERROI_H
#include "TrigSteeringEvent/TrigSuperRoi.h"
#endif

class TrigSuperRoiCollection : public DataVector<TrigSuperRoi> { };

CLASS_DEF( TrigSuperRoiCollection, 1078197961, 1 )

SG_BASE(TrigSuperRoiCollection, DataVector<TrigSuperRoi>);

// #endif //  TRIGSTEERINGEVENT_TRIGSUPERROICOLLECTION_H
