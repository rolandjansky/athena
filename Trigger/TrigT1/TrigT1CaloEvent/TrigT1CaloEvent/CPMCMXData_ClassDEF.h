/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CPMCMXData_ClassDEF_H
#define CPMCMXData_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef TRIGT1CALO_CPMCMXDATA_H
#include "TrigT1CaloEvent/CPMCMXData.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

CLASS_DEF( LVL1::CPMCMXData , 57352585 , 1 )
CLASS_DEF( DataVector<LVL1::CPMCMXData> , 55455300 , 1 )
     //the third field is the version which is currently ignored
#endif
