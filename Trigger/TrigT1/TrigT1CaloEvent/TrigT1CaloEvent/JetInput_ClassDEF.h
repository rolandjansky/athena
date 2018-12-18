/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JetInput_ClassDEF_H
#define JetInput_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef JetInput_H
#include "TrigT1CaloEvent/JetInput.h"
#endif

#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"

CLASS_DEF(LVL1::JetInput,  36131454, 1)
CLASS_DEF(DataVector<LVL1::JetInput>,  54053401, 1)
     //the third field is the version which is currently ignored
#endif
