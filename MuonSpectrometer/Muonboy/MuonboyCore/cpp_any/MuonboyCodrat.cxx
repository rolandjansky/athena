/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyCodrat.h"

#include "MuonboyCore/GetSetMuonboyVariable.h"

MuonboyCodrat::MuonboyCodrat(){}

MuonboyCodrat::~MuonboyCodrat(){}

int    MuonboyCodrat::GetNMATMA()                         {std::string Name="NMATMA" ; return GetMuboIntVar (Name,NotDef,NotDef,NotDef);}
int    MuonboyCodrat::GetNBDRAT(                 int& ISC){std::string Name="NBDRAT" ; return GetMuboIntVar (Name,ISC   ,NotDef,NotDef);}
double MuonboyCodrat::GetCCDRAT(int& IDU,        int& ISC){std::string Name="CCDRAT" ; return GetMuboRealVar(Name,IDU   ,ISC   ,NotDef);}
double MuonboyCodrat::GetFFDRAT(int& IDU,        int& ISC){std::string Name="FFDRAT" ; return GetMuboRealVar(Name,IDU   ,ISC   ,NotDef);}
double MuonboyCodrat::GetGGDRAT(int& IDU,        int& ISC){std::string Name="GGDRAT" ; return GetMuboRealVar(Name,IDU   ,ISC   ,NotDef);}
double MuonboyCodrat::GetEZDRAT(int& IDU,        int& ISC){std::string Name="EZDRAT" ; return GetMuboRealVar(Name,IDU   ,ISC   ,NotDef);}
double MuonboyCodrat::GetSSDRAT(int& IDU,int& J, int& ISC){std::string Name="SSDRAT" ; return GetMuboRealVar(Name,IDU   ,J     ,ISC   );}
double MuonboyCodrat::GetZZDRAT(int& IDU,int& J, int& ISC){std::string Name="ZZDRAT" ; return GetMuboRealVar(Name,IDU   ,J     ,ISC   );}
double MuonboyCodrat::GetTTDRAT(int& IDU,int& J, int& ISC){std::string Name="TTDRAT" ; return GetMuboRealVar(Name,IDU   ,J     ,ISC   );}
int    MuonboyCodrat::GetIWDRAT(int& IDU,int& J, int& ISC){std::string Name="IWDRAT" ; return GetMuboIntVar (Name,IDU   ,J     ,ISC   );}
int    MuonboyCodrat::GetJFDRAT(int& IDU,        int& ISC){std::string Name="JFDRAT" ; return GetMuboIntVar (Name,IDU   ,ISC   ,NotDef);}

