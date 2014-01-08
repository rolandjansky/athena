/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyComusc.h"

#include "MuonboyCore/GetSetMuonboyVariable.h"

MuonboyComusc::MuonboyComusc(){}

MuonboyComusc::~MuonboyComusc(){}

double MuonboyComusc::RRMUSC(int Index) {std::string Name="RRMUSC"   ; return GetMuboRealVar(Name,Index ,NotDef,NotDef);}
double MuonboyComusc::RRMASC(int Index) {std::string Name="RRMASC"   ; return GetMuboRealVar(Name,Index ,NotDef,NotDef);}
double MuonboyComusc::X0MASC(int Index) {std::string Name="X0MASC"   ; return GetMuboRealVar(Name,Index ,NotDef,NotDef);}
double MuonboyComusc::S1MASC(int Index) {std::string Name="S1MASC"   ; return GetMuboRealVar(Name,Index ,NotDef,NotDef);}
double MuonboyComusc::S2MASC(int Index) {std::string Name="S2MASC"   ; return GetMuboRealVar(Name,Index ,NotDef,NotDef);}
double MuonboyComusc::THMASC(int Index) {std::string Name="THMASC"   ; return GetMuboRealVar(Name,Index ,NotDef,NotDef);}
double MuonboyComusc::ALMASC(int Index) {std::string Name="ALMASC"   ; return GetMuboRealVar(Name,Index ,NotDef,NotDef);}
double MuonboyComusc::BLMASC(int Index) {std::string Name="BLMASC"   ; return GetMuboRealVar(Name,Index ,NotDef,NotDef);}
double MuonboyComusc::VinVouMASC(int Ivinvouparam,int Index) {std::string Name="VinVouMASC"   ; return GetMuboRealVar(Name,Ivinvouparam ,Index,NotDef);}
double MuonboyComusc::ENMASC()          {std::string Name="ENMASC"   ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyComusc::SIGENMASC()       {std::string Name="SIGENMASC"; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyComusc::RRMESU(int Index) {std::string Name="RRMESU"   ; return GetMuboRealVar(Name,Index ,NotDef,NotDef);}
double MuonboyComusc::DDMESU(int Index) {std::string Name="DDMESU"   ; return GetMuboRealVar(Name,Index ,NotDef,NotDef);}
double MuonboyComusc::FFMESU(int Index) {std::string Name="FFMESU"   ; return GetMuboRealVar(Name,Index ,NotDef,NotDef);}
int    MuonboyComusc::NBMUSC()          {std::string Name="NBMUSC"   ; return GetMuboIntVar (Name,NotDef,NotDef,NotDef);}
int    MuonboyComusc::JVMUSC(int Index) {std::string Name="JVMUSC"   ; return GetMuboIntVar (Name,Index ,NotDef,NotDef);}
int    MuonboyComusc::NBMASC()          {std::string Name="NBMASC"   ; return GetMuboIntVar (Name,NotDef,NotDef,NotDef);}
int    MuonboyComusc::JVMASC(int Index) {std::string Name="JVMASC"   ; return GetMuboIntVar (Name,Index ,NotDef,NotDef);}
int    MuonboyComusc::LLMASC(int Index) {std::string Name="LLMASC"   ; return GetMuboIntVar (Name,Index ,NotDef,NotDef);}
int    MuonboyComusc::IDMASC(int Index) {std::string Name="IDMASC"   ; return GetMuboIntVar (Name,Index ,NotDef,NotDef);}
int    MuonboyComusc::NBMESU()          {std::string Name="NBMESU"   ; return GetMuboIntVar (Name,NotDef,NotDef,NotDef);}

double MuonboyComusc::XposMASC(int Index) {std::string Name="XposMASC"   ; return GetMuboRealVar(Name,Index ,NotDef,NotDef);}
double MuonboyComusc::YposMASC(int Index) {std::string Name="YposMASC"   ; return GetMuboRealVar(Name,Index ,NotDef,NotDef);}
double MuonboyComusc::ZposMASC(int Index) {std::string Name="ZposMASC"   ; return GetMuboRealVar(Name,Index ,NotDef,NotDef);}
double MuonboyComusc::XdirMASC(int Index) {std::string Name="XdirMASC"   ; return GetMuboRealVar(Name,Index ,NotDef,NotDef);}
double MuonboyComusc::YdirMASC(int Index) {std::string Name="YdirMASC"   ; return GetMuboRealVar(Name,Index ,NotDef,NotDef);}
double MuonboyComusc::ZdirMASC(int Index) {std::string Name="ZdirMASC"   ; return GetMuboRealVar(Name,Index ,NotDef,NotDef);}
double MuonboyComusc::PmomMASC(int Index) {std::string Name="PmomMASC"   ; return GetMuboRealVar(Name,Index ,NotDef,NotDef);}
double MuonboyComusc::ChgeMASC(int Index) {std::string Name="ChgeMASC"   ; return GetMuboRealVar(Name,Index ,NotDef,NotDef);}
int    MuonboyComusc::IValMASC(int Index) {std::string Name="IValMASC"   ; return GetMuboIntVar (Name,Index ,NotDef,NotDef);}

void MuonboyComusc::Print1() 
{
  std::cout << "NBMASC " << NBMASC()
            << "ENMASC " << ENMASC()
            << std::endl ;
  int IndexMax = NBMASC();
  for (int Index= 0 ; Index <IndexMax ; Index++){
   std::cout 
            << " Index " << Index
            << " rrmasc " << RRMASC(Index)
            << " llmasc " << LLMASC(Index)
            << " x0masc " << X0MASC(Index)
            << " s1masc " << S1MASC(Index)
            << " s2masc " << S2MASC(Index)
            << " thmasc " << THMASC(Index)
            << " almasc " << ALMASC(Index)
            << " blmasc " << BLMASC(Index)
            << std::endl ;
  }    
}
void MuonboyComusc::Print2() 
{
  std::cout << "NBMASC " << NBMASC()
            << "ENMASC " << ENMASC()
            << std::endl ;
  int IndexMax = NBMASC();
  for (int Index= 0 ; Index <IndexMax ; Index++){
   std::cout 
            << " IValMASC " << IValMASC(Index)
            << " XposMASC " << XposMASC(Index)
            << " YposMASC " << YposMASC(Index)
            << " ZposMASC " << ZposMASC(Index)
            << " Index " << Index
            << " rrmasc " << RRMASC(Index)
            << " llmasc " << LLMASC(Index)
            << " x0masc " << X0MASC(Index)
            << " s1masc " << S1MASC(Index)
            << " s2masc " << S2MASC(Index)
            << " thmasc " << THMASC(Index)
            << " almasc " << ALMASC(Index)
            << " blmasc " << BLMASC(Index)
            << std::endl ;
  }    
}

void MuonboyComusc::ResetMBCOMUSC() {
  SetNBMUSC(0) ;
  SetNBMASC(0) ;
  SetNBMESU(0) ;
}
void MuonboyComusc::SetNBMUSC(int    NBMASCi) {std::string Name="NBMUSC" ; SetMuboIntVar (Name,NotDef,NotDef,NotDef,NBMASCi);}
void MuonboyComusc::SetNBMASC(int    NBMASCi) {std::string Name="NBMASC" ; SetMuboIntVar (Name,NotDef,NotDef,NotDef,NBMASCi);}
void MuonboyComusc::SetNBMESU(int    NBMASCi) {std::string Name="NBMESU" ; SetMuboIntVar (Name,NotDef,NotDef,NotDef,NBMASCi);}

