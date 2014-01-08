/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyCofito.h"

#include "MuonboyCore/GetSetMuonboyVariable.h"

MuonboyCofito::MuonboyCofito(){}

MuonboyCofito::~MuonboyCofito(){}


int    MuonboyCofito::GetNDRAMA()        {std::string Name="NDRAMA"    ; return GetMuboIntVar (Name,NotDef,NotDef,NotDef);}
int    MuonboyCofito::GetNMATMA()        {std::string Name="NMATMA"    ; return GetMuboIntVar (Name,NotDef,NotDef,NotDef);}
int    MuonboyCofito::GetNBDRAT(int ISC) {std::string Name="NBDRAT"    ; return GetMuboIntVar (Name,ISC,NotDef,NotDef);}

int    MuonboyCofito::GetNBFIXE()       {std::string Name="NBFIXE"     ; return GetMuboIntVar (Name,NotDef,NotDef,NotDef);}
int    MuonboyCofito::GetIDFIXE(int NN) {std::string Name="IDFIXE"     ; return GetMuboIntVar (Name,NN,NotDef,NotDef);}
int    MuonboyCofito::GetISCFIXE(int NN){std::string Name="ISCFIXE"    ; return GetMuboIntVar (Name,NN,NotDef,NotDef);}

double MuonboyCofito::GetSSTRAKDO(int ID,int J,int ISC){std::string Name="SSTRAKDO"    ; return GetMuboRealVar(Name,ID,J,ISC);}
double MuonboyCofito::GetZZTRAKDO(int ID,int J,int ISC){std::string Name="ZZTRAKDO"    ; return GetMuboRealVar(Name,ID,J,ISC);}
double MuonboyCofito::GetTTTRAKDO(int ID,int J,int ISC){std::string Name="TTTRAKDO"    ; return GetMuboRealVar(Name,ID,J,ISC);}

int    MuonboyCofito::GetKOUNTERDRA(int ID,int ISC) {std::string Name="KOUNTERDRA"     ; return GetMuboIntVar (Name,ID,ISC,NotDef);}

double MuonboyCofito::GetSSDRAT(int ID,int J,int ISC){std::string Name="SSDRAT"    ; return GetMuboRealVar(Name,ID,J,ISC);}
double MuonboyCofito::GetZZDRAT(int ID,int J,int ISC){std::string Name="ZZDRAT"    ; return GetMuboRealVar(Name,ID,J,ISC);}
double MuonboyCofito::GetTTDRAT(int ID,int J,int ISC){std::string Name="TTDRAT"    ; return GetMuboRealVar(Name,ID,J,ISC);}

double MuonboyCofito::GetCCDRAT(int ID,int ISC){std::string Name="CCDRAT"    ; return GetMuboRealVar(Name,ID,ISC,NotDef);}
double MuonboyCofito::GetFFDRAT(int ID,int ISC){std::string Name="FFDRAT"    ; return GetMuboRealVar(Name,ID,ISC,NotDef);}
double MuonboyCofito::GetGGDRAT(int ID,int ISC){std::string Name="GGDRAT"    ; return GetMuboRealVar(Name,ID,ISC,NotDef);}
double MuonboyCofito::GetEZDRAT(int ID,int ISC){std::string Name="EZDRAT"    ; return GetMuboRealVar(Name,ID,ISC,NotDef);}
int    MuonboyCofito::GetJFDRAT(int ID,int ISC){std::string Name="JFDRAT"    ; return GetMuboIntVar (Name,ID,ISC,NotDef);}

int    MuonboyCofito::GetIWDRAT(int ID,int J,int ISC){std::string Name="IWDRAT"    ; return GetMuboIntVar(Name,ID,J,ISC);}
