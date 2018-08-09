/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JEMEtSums_ClassDEF_H
#define JEMEtSums_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef JEMEtSums_H
#include "TrigT1CaloEvent/JEMEtSums.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

CLASS_DEF( LVL1::JEMEtSums , 9623557 , 1 )
CLASS_DEF( DataVector<LVL1::JEMEtSums> , 3490292 , 1 )
     //the third field is the version which is currently ignored
#endif
