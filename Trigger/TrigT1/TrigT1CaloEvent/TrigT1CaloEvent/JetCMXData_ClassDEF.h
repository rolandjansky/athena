/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JetCMXData_ClassDEF_H
#define JetCMXData_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef TRIGT1CALO_JETCMXDATA_H
#include "TrigT1CaloEvent/JetCMXData.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

CLASS_DEF( LVL1::JetCMXData , 162528320 , 1 )
CLASS_DEF( DataVector<LVL1::JetCMXData> , 161226587 , 1 )
     //the third field is the version which is currently ignored
#endif
