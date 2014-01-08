/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyCoreMisc.h"


// Fortran routines -------------------------------------------------------------

extern "C" void f1getrmubovar_( int& Long, char* NAMEVAR, int&I1, int& I2, int& I3, double& Rvar );
extern "C" void f1getimubovar_( int& Long, char* NAMEVAR, int&I1, int& I2, int& I3, int& Ivar );
extern "C" void f1getcmubovar_( int& Long, char* NAMEVAR, int&I1, int& I2, int& I3, int& Lvar,  char* CVAR );
extern "C" void f1setrmubovar_( int& Long, char* NAMEVAR, int&I1, int& I2, int& I3, double& Rvar );
extern "C" void f1setimubovar_( int& Long, char* NAMEVAR, int&I1, int& I2, int& I3, int& Ivar );

extern "C" void f1getimubovarbaz_( int& Istop, int& Istatus , int& Long, char* NAMEVAR, int&I1, int& I2, int& I3, int& Ivar );
extern "C" void f1getrmubovarbaz_( int& Istop, int& Istatus , int& Long, char* NAMEVAR, int&I1, int& I2, int& I3, double& Rvar );
extern "C" void f1setrmubovarbaz_( int& Istop, int& Istatus , int& Long, char* NAMEVAR, int&I1, int& I2, int& I3, double& Rvar );
extern "C" void f1setimubovarbaz_( int& Istop, int& Istatus , int& Long, char* NAMEVAR, int&I1, int& I2, int& I3, int& Ivar );

// Get Muonboy REAL (double) Variable -------------------------------------------

double       GetMuboRealVar(std::string VarName, int I1, int I2, int I3){
  char NAMEVAR[32];
  int Long = VarName.size();
  if( Long > 32 ){ Long = 32; }
  for(int I=0; I<Long; I++){ NAMEVAR[I] = VarName[I]; }
  int I3p = I3 + 1;
  int I2p = I2 + 1;
  int I1p = I1 + 1;
  double ToBeReturned=0.;
  f1getrmubovar_( Long, NAMEVAR, I1p, I2p, I3p, ToBeReturned );
  return ToBeReturned;
}

// Get Muonboy INTEGER (int) Variable -------------------------------------------

int          GetMuboIntVar(std::string VarName, int I1, int I2, int I3){
  char NAMEVAR[32];
  int Long = VarName.size();
  if( Long > 32 ){ Long = 32; }
  for(int I=0; I<Long; I++){ NAMEVAR[I] = VarName[I]; }
  int I3p = I3 + 1;
  int I2p = I2 + 1;
  int I1p = I1 + 1;
  int ToBeReturned=0;
  f1getimubovar_( Long, NAMEVAR, I1p, I2p, I3p, ToBeReturned );
  return ToBeReturned;
}

// Get Muonboy CHARACTER (std::string) Variable ---------------------------------

std::string  GetMuboCharVar(std::string VarName, int I1, int I2, int I3){
  char NAMEVAR[32];
  int Long = VarName.size();
  if( Long > 32 ){ Long = 32; }
  for(int I=0; I<Long; I++){ NAMEVAR[I] = VarName[I]; }
  int I3p = I3 + 1;
  int I2p = I2 + 1;
  int I1p = I1 + 1;
  int  Lvar;
  char CVAR[32];
  f1getcmubovar_( Long, NAMEVAR, I1p, I2p, I3p, Lvar, CVAR );
  std::string ToBeReturned=" ";
  ToBeReturned.resize(Lvar);
  for(int I=0; I<Lvar; I++){ ToBeReturned[I] = CVAR[I]; }
  return ToBeReturned;
}

// Set Muonboy REAL (double) Variable -------------------------------------------

void   SetMuboRealVar( std::string VarName, int I1, int I2, int I3, double Rvar ){
  char NAMEVAR[32];
  int Long = VarName.size();
  if( Long > 32 ){ Long = 32; }
  for(int I=0; I<Long; I++){ NAMEVAR[I] = VarName[I]; }
  int I3p = I3 + 1;
  int I2p = I2 + 1;
  int I1p = I1 + 1;
  f1setrmubovar_( Long, NAMEVAR, I1p, I2p, I3p, Rvar );
}

// Set Muonboy INTEGER (int) Variable -------------------------------------------

void   SetMuboIntVar( std::string VarName, int I1, int I2, int I3, int Ivar ){
  char NAMEVAR[32];
  int Long = VarName.size();
  if( Long > 32 ){ Long = 32; }
  for(int I=0; I<Long; I++){ NAMEVAR[I] = VarName[I]; }
  int I3p = I3 + 1;
  int I2p = I2 + 1;
  int I1p = I1 + 1;
  f1setimubovar_( Long, NAMEVAR, I1p, I2p, I3p, Ivar );
}

// Get Muonboy REAL (double) Variable -------------------------------------------

int GetMuboRealVarBaz (std::string VarName, int I1, int I2, int I3, double& value){
  char NAMEVAR[32];
  int Long = VarName.size();
  if( Long > 32 ){ Long = 32; }
  for(int I=0; I<Long; I++){ NAMEVAR[I] = VarName[I]; }
  int I3p = I3 + 1;
  int I2p = I2 + 1;
  int I1p = I1 + 1;
  int Istop   = 0 ;
  int Istatus = 0 ;
  f1getrmubovarbaz_( Istop, Istatus , Long, NAMEVAR, I1p, I2p, I3p, value );
  return Istatus;
}

// Get Muonboy INTEGER (int) Variable -------------------------------------------

int GetMuboIntVarBaz (std::string VarName, int I1, int I2, int I3, int& value){
  char NAMEVAR[32];
  int Long = VarName.size();
  if( Long > 32 ){ Long = 32; }
  for(int I=0; I<Long; I++){ NAMEVAR[I] = VarName[I]; }
  int I3p = I3 + 1;
  int I2p = I2 + 1;
  int I1p = I1 + 1;
  int Istop   = 0 ;
  int Istatus = 0 ;
  f1getimubovarbaz_( Istop, Istatus , Long, NAMEVAR, I1p, I2p, I3p, value );
  return Istatus;
}

// Set Muonboy REAL (double) Variable -------------------------------------------

int   SetMuboRealVarBaz( std::string VarName, int I1, int I2, int I3, double Rvar ){
  char NAMEVAR[32];
  int Long = VarName.size();
  if( Long > 32 ){ Long = 32; }
  for(int I=0; I<Long; I++){ NAMEVAR[I] = VarName[I]; }
  int I3p = I3 + 1;
  int I2p = I2 + 1;
  int I1p = I1 + 1;
  int Istop   = 0 ;
  int Istatus = 0 ;
  f1setrmubovarbaz_( Istop, Istatus , Long, NAMEVAR, I1p, I2p, I3p, Rvar );
  return Istatus;
}

// Set Muonboy INTEGER (int) Variable -------------------------------------------

int   SetMuboIntVarBaz( std::string VarName, int I1, int I2, int I3, int Ivar ){
  char NAMEVAR[32];
  int Long = VarName.size();
  if( Long > 32 ){ Long = 32; }
  for(int I=0; I<Long; I++){ NAMEVAR[I] = VarName[I]; }
  int I3p = I3 + 1;
  int I2p = I2 + 1;
  int I1p = I1 + 1;
  int Istop   = 0 ;
  int Istatus = 0 ;
  f1setimubovarbaz_( Istop, Istatus , Long, NAMEVAR, I1p, I2p, I3p, Ivar );
  return Istatus;
}

