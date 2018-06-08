/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef InternalTriggerTower_ClassDEF_H
#define InternalTriggerTower_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef InternalTriggerTower_H
#include "TrigT1CaloUtils/InternalTriggerTower.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"


CLASS_DEF(LVL1::InternalTriggerTower,   119250601, 1)
CLASS_DEF(DataVector<LVL1::InternalTriggerTower>,   180742156, 1)

     //the third field is the version which is currently ignored
#endif
