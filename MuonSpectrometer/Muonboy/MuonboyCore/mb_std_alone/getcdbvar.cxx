/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "getcdbvar.h"

// Fortran routines -------------------------------------------------------------

extern "C" void f1geticdbvar_( int& Long, char* NAMEVAR, int&I1, int& I2, int& I3, int& Ivar );
extern "C" void f1getccdbvar_( int& Long, char* NAMEVAR, int&I1, int& I2, int& I3, int& Lvar,  char* CVAR );


// Get M_MB_CondDB INTEGER (int) Variable -------------------------------------------

int          GetCDBIntVar(std::string VarName, int I1, int I2, int I3){
  char NAMEVAR[120];
  int Long = VarName.size();
  if( Long > 120 ){ Long = 120; }
  for(int I=0; I<Long; I++){ NAMEVAR[I] = VarName[I]; }
  int I3p = I3 + 1;
  int I2p = I2 + 1;
  int I1p = I1 + 1;
  int ToBeReturned=0;
  f1geticdbvar_( Long, NAMEVAR, I1p, I2p, I3p, ToBeReturned );
  return ToBeReturned;
}

// Get M_MB_CondDB CHARACTER (std::string) Variable ---------------------------------

std::string  GetCDBCharVar(std::string VarName, int I1, int I2, int I3){
  char NAMEVAR[120];
  int Long = VarName.size();
  if( Long > 120 ){ Long = 120; }
  for(int I=0; I<Long; I++){ NAMEVAR[I] = VarName[I]; }
  int I3p = I3 + 1;
  int I2p = I2 + 1;
  int I1p = I1 + 1;
  int  Lvar;
  char CVAR[120];
  f1getccdbvar_( Long, NAMEVAR, I1p, I2p, I3p, Lvar, CVAR );
  std::string ToBeReturned=" ";
  ToBeReturned.resize(Lvar);
  for(int I=0; I<Lvar; I++){ ToBeReturned[I] = CVAR[I]; }
  return ToBeReturned;
}
