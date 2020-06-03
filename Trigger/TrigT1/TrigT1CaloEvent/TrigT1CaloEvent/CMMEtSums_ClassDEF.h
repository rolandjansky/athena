/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMMEtSums_ClassDEF_H
#define CMMEtSums_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef CMMEtSums_H
#include "TrigT1CaloEvent/CMMEtSums.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

CLASS_DEF( LVL1::CMMEtSums , 107516024 , 1 )
CLASS_DEF( DataVector<LVL1::CMMEtSums> , 172060287 , 1 )
     //the third field is the version which is currently ignored
#endif
