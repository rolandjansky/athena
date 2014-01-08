/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef getcdbvar_H
#define getcdbvar_H

#include "MuonboyCore/MuonboyCoreMisc.h"

int          GetCDBIntVar (std::string VarName, int I1, int I2, int I3);
std::string  GetCDBCharVar(std::string VarName, int I1, int I2, int I3);


#endif
