/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CPMTower_ClassDEF_H
#define CPMTower_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef CPMTower_H
#include "TrigT1CaloEvent/CPMTower.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"


CLASS_DEF(LVL1::CPMTower, 105961588 , 1)
CLASS_DEF(DataVector<LVL1::CPMTower>,  88388307 , 1)

     //the third field is the version which is currently ignored
#endif
