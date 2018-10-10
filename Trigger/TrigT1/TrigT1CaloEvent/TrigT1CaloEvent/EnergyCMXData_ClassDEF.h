/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef EnergyCMXData_ClassDEF_H
#define EnergyCMXData_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef TRIGT1CALO_ENERGYCMXDATA_H
#include "TrigT1CaloEvent/EnergyCMXData.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

CLASS_DEF( LVL1::EnergyCMXData , 152254774 , 1 )
CLASS_DEF( DataVector<LVL1::EnergyCMXData> , 227556281 , 1 )
     //the third field is the version which is currently ignored
#endif
