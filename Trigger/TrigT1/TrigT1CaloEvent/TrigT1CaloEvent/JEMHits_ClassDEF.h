/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JEMHits_ClassDEF_H
#define JEMHits_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef JEMHits_H
#include "TrigT1CaloEvent/JEMHits.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

CLASS_DEF( LVL1::JEMHits , 104962406 , 1 )
CLASS_DEF( DataVector<LVL1::JEMHits> , 210130025 , 1 )
     //the third field is the version which is currently ignored
#endif
