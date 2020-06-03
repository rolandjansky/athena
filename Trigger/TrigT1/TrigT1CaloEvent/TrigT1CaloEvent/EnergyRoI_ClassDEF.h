/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef ENERGYROI_CLASSDEF_H
#define ENERGYROI_CLASSDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#ifndef EnergyRoI_H
#include "TrigT1CaloEvent/EnergyRoI.h"
#endif

#include "CLIDSvc/CLASS_DEF.h"

CLASS_DEF(LVL1::EnergyRoI, 6255, 1)
// I would like to renumber this 6208, but it seems to cause problems.
     //the third field is the version which is currently ignored
#endif
