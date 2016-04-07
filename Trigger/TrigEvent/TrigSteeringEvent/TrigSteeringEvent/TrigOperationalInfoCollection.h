/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
#ifndef TRIGSTEERINGEVENT_TRIGOPERATIONALINFOCollection_H
#define TRIGSTEERINGEVENT_TRIGOPERATIONALINFOCollection_H
#include "AthContainers/DataVector.h"
#include "SGTools/BaseInfo.h"

#ifndef TRIGSTEERINGEVENT_TRIGOPERATIONALINFO_H
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#endif 

class TrigOperationalInfoCollection : public DataVector<TrigOperationalInfo> {

};
CLASS_DEF( TrigOperationalInfoCollection , 1320355091 , 1 )

SG_BASE(TrigOperationalInfoCollection, DataVector<TrigOperationalInfo>);

#endif
