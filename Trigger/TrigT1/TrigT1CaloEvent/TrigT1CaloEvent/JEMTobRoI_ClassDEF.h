/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JEMTobRoI_ClassDEF_H
#define JEMTobRoI_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef TRIGT1CALO_JEMTOBROI_H
#include "TrigT1CaloEvent/JEMTobRoI.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

CLASS_DEF( LVL1::JEMTobRoI , 243652413 , 1 )
CLASS_DEF( DataVector<LVL1::JEMTobRoI> , 165898968 , 1 )
     //the third field is the version which is currently ignored
#endif
