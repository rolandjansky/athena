/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMMJetHits_ClassDEF_H
#define CMMJetHits_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef CMMJetHits_H
#include "TrigT1CaloEvent/CMMJetHits.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

CLASS_DEF( LVL1::CMMJetHits , 45481339 , 1 )
CLASS_DEF( DataVector<LVL1::CMMJetHits> , 240136234 , 1 )
     //the third field is the version which is currently ignored
#endif
