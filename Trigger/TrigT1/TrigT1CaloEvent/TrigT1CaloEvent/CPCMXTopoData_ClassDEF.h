/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CPCMXTopoData_ClassDEF_H
#define CPCMXTopoData_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef TRIGT1CALO_CPCMXTOPODATA_H
#include "TrigT1CaloEvent/CPCMXTopoData.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

CLASS_DEF( LVL1::CPCMXTopoData , 195717641 , 1 )
CLASS_DEF( DataVector<LVL1::CPCMXTopoData> , 51414472 , 1 )
     //the third field is the version which is currently ignored
#endif
