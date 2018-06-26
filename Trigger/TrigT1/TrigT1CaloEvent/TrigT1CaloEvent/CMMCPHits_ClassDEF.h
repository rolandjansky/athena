/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMMCPHits_ClassDEF_H
#define CMMCPHits_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef CMMCPHits_H
#include "TrigT1CaloEvent/CMMCPHits.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

CLASS_DEF( LVL1::CMMCPHits , 35099144 , 1 )
CLASS_DEF( DataVector<LVL1::CMMCPHits> , 252967879 , 1 )
     //the third field is the version which is currently ignored
#endif
