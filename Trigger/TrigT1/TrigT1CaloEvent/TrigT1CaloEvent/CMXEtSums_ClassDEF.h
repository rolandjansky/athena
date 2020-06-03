/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMXEtSums_ClassDEF_H
#define CMXEtSums_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef CMXEtSums_H
#include "TrigT1CaloEvent/CMXEtSums.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

CLASS_DEF( LVL1::CMXEtSums , 247102965 , 1 )
CLASS_DEF( DataVector<LVL1::CMXEtSums> , 87855916 , 1 )
     //the third field is the version which is currently ignored
#endif
