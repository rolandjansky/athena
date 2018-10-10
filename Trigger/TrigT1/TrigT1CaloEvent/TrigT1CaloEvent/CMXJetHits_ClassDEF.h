/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMXJetHits_ClassDEF_H
#define CMXJetHits_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef CMXJetHits_H
#include "TrigT1CaloEvent/CMXJetHits.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

CLASS_DEF( LVL1::CMXJetHits , 161098020 , 1 )
CLASS_DEF( DataVector<LVL1::CMXJetHits> , 40333703 , 1 )
     //the third field is the version which is currently ignored
#endif
