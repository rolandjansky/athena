/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AmdcStand/AmdcStandMisc.h"


// Fortran routines -------------------------------------------------------------
#include "f1get.h"

// Get Amdcsimrec REAL (double) Variable -------------------------------------------

double       GetAmdcRealVar(std::string VarName, int I1, int I2, int I3){
  char NAMEVAR[32];
  int Long = VarName.size();
  if( Long > 32 ){ Long = 32; }
  for(int I=0; I<Long; I++){ NAMEVAR[I] = VarName[I]; }
  double ToBeReturned=0.;
  f1getramdcvar_( Long, NAMEVAR, I1, I2, I3, ToBeReturned );
  return ToBeReturned;
}

// Get Amdcsimrec INTEGER (int) Variable -------------------------------------------

int          GetAmdcIntVar(std::string VarName, int I1, int I2, int I3){
  char NAMEVAR[32];
  int Long = VarName.size();
  if( Long > 32 ){ Long = 32; }
  for(int I=0; I<Long; I++){ NAMEVAR[I] = VarName[I]; }
  int ToBeReturned=0;
  f1getiamdcvar_( Long, NAMEVAR, I1, I2, I3, ToBeReturned );
  return ToBeReturned;
}

// Get Amdcsimrec CHARACTER (std::string) Variable ---------------------------------

std::string  GetAmdcCharVar(std::string VarName, int I1, int I2, int I3){
  char NAMEVAR[32];
  int Long = VarName.size();
  if( Long > 32 ){ Long = 32; }
  for(int I=0; I<Long; I++){ NAMEVAR[I] = VarName[I]; }
  int  Lvar;
  char CVAR[32];
  f1getcamdcvar_( Long, NAMEVAR, I1, I2, I3, Lvar, CVAR );
  std::string ToBeReturned=" ";
  ToBeReturned.resize(Lvar);
  for(int I=0; I<Lvar; I++){ ToBeReturned[I] = CVAR[I]; }
  return ToBeReturned;
}
