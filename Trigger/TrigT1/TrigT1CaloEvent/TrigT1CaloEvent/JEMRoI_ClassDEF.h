/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JEMRoI_ClassDEF_H
#define JEMRoI_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef TRIGT1CALO_JEMROI_H
#include "TrigT1CaloEvent/JEMRoI.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

CLASS_DEF( LVL1::JEMRoI , 244235973 , 1 )
CLASS_DEF( DataVector<LVL1::JEMRoI> , 182624508 , 1 )
     //the third field is the version which is currently ignored
#endif
