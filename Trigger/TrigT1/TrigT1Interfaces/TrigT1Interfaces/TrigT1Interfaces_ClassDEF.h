/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TrigT1Interfaces_ClassDEF_H
#define TrigT1Interfaces_ClassDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#include "TrigT1Interfaces/SlinkWord.h"

// These includes are only left here not to break other packages.
// But they should be removed later...
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigT1Interfaces/RecJetRoI.h"

//#include "StoreGate/tools/ClassID_traits.h"
#ifndef CLIDSVC_CLASSDEF_H
#include "CLIDSvc/CLASS_DEF.h"
#endif
#include "AthContainers/DataVector.h"                  

CLASS_DEF(DataVector<LVL1CTP::SlinkWord>,   6250, 1)
CLASS_DEF(LVL1CTP::SlinkWord,   6251, 1)

#endif
