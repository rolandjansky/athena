/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JetElement_ClassDEF_H
#define JetElement_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef JetElement_H
#include "TrigT1CaloEvent/JetElement.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

CLASS_DEF(LVL1::JetElement,  6202, 1)
CLASS_DEF(DataVector<LVL1::JetElement>,  6203, 1)
     //the third field is the version which is currently ignored
#endif
