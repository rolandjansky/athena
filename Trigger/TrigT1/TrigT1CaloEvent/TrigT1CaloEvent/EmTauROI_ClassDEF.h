/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef EmTauROI_ClassDEF_H
#define EmTauROI_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate
#include "TrigT1CaloEvent/EmTauROI.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"


CLASS_DEF(LVL1::EmTauROI,  6200, 1)
CLASS_DEF(DataVector<LVL1::EmTauROI>,  6201, 1)
     //the third field is the version which is currently ignored
#endif
