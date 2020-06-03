/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CPMRoI_ClassDEF_H
#define CPMRoI_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef TRIGT1CALO_CPMROI_H
#include "TrigT1CaloEvent/CPMRoI.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

CLASS_DEF( LVL1::CPMRoI , 219018335 , 1 )
CLASS_DEF( DataVector<LVL1::CPMRoI> , 216508938 , 1 )
     //the third field is the version which is currently ignored
#endif
