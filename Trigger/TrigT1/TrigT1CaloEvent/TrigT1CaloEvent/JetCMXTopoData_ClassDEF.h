/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JetCMXTopoData_ClassDEF_H
#define JetCMXTopoData_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef TRIGT1CALO_JETCMXTOPODATA_H
#include "TrigT1CaloEvent/JetCMXTopoData.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

CLASS_DEF( LVL1::JetCMXTopoData , 187108912 , 1 )
CLASS_DEF( DataVector<LVL1::JetCMXTopoData> , 42187659 , 1 )
     //the third field is the version which is currently ignored
#endif
