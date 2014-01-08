/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GetAmdcsimrecVariable_H
#define GetAmdcsimrecVariable_H

#include "AmdcStand/AmdcStandMisc.h"

double       GetAmdcRealVar(std::string VarName, int I1, int I2, int I3);
int          GetAmdcIntVar (std::string VarName, int I1, int I2, int I3);
std::string  GetAmdcCharVar(std::string VarName, int I1, int I2, int I3);

#endif
