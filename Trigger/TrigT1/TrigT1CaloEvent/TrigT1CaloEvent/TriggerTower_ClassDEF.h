/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TriggerTower_ClassDEF_H
#define TriggerTower_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef TriggerTower_H
#include "TrigT1CaloEvent/TriggerTower.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"


CLASS_DEF(LVL1::TriggerTower,   6206, 1)
CLASS_DEF(DataVector<LVL1::TriggerTower>,   6207, 1)

     //the third field is the version which is currently ignored
#endif
