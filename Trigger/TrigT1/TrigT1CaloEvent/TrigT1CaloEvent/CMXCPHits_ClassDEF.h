/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMXCPHits_ClassDEF_H
#define CMXCPHits_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef CMXCPHits_H
#include "TrigT1CaloEvent/CMXCPHits.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

CLASS_DEF( LVL1::CMXCPHits , 196617589 , 1 )
CLASS_DEF( DataVector<LVL1::CMXCPHits> , 99664276 , 1 )
     //the third field is the version which is currently ignored
#endif
