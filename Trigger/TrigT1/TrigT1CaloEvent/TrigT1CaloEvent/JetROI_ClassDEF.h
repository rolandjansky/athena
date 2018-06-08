/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JetROI_ClassDEF_H
#define JetROI_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#include "TrigT1CaloEvent/JetROI.h"

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

CLASS_DEF(LVL1::JetROI, 6204, 1)
CLASS_DEF(DataVector<LVL1::JetROI>, 6205, 1)
     //the third field is the version which is currently ignored
#endif
