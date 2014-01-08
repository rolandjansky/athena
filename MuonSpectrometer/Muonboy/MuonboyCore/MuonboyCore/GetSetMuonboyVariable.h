/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GetSetMuonboyVariable_H
#define GetSetMuonboyVariable_H

#include "MuonboyCore/MuonboyCoreMisc.h"

double       GetMuboRealVar(std::string VarName, int I1, int I2, int I3);
int          GetMuboIntVar (std::string VarName, int I1, int I2, int I3);
std::string  GetMuboCharVar(std::string VarName, int I1, int I2, int I3);

void         SetMuboRealVar(std::string VarName, int I1, int I2, int I3, double Rvar);
void         SetMuboIntVar (std::string VarName, int I1, int I2, int I3, int Ivar);

int          GetMuboIntVarBaz  (std::string VarName, int I1, int I2, int I3, int& value);
int          GetMuboRealVarBaz (std::string VarName, int I1, int I2, int I3, double& value);

int          SetMuboRealVarBaz(std::string VarName, int I1, int I2, int I3, double Rvar);
int          SetMuboIntVarBaz (std::string VarName, int I1, int I2, int I3, int Ivar);


#endif
