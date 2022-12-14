/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMXJetTob_ClassDEF_H
#define CMXJetTob_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef CMXJetTob_H
#include "TrigT1CaloEvent/CMXJetTob.h"
#endif

#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"

CLASS_DEF( LVL1::CMXJetTob , 20149142 , 1 )
CLASS_DEF( DataVector<LVL1::CMXJetTob> , 201376881 , 1 )
     //the third field is the version which is currently ignored
#endif
