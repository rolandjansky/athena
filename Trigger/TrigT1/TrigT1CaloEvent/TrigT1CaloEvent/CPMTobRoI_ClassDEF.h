/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CPMTobRoI_ClassDEF_H
#define CPMTobRoI_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef TRIGT1CALO_CPMTOBROI_H
#include "TrigT1CaloEvent/CPMTobRoI.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

CLASS_DEF( LVL1::CPMTobRoI , 168051847 , 1 )
CLASS_DEF( DataVector<LVL1::CPMTobRoI> , 80981142 , 1 )
     //the third field is the version which is currently ignored
#endif
