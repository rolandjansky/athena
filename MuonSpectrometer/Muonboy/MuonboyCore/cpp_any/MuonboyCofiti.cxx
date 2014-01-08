/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyCofiti.h"

#include "MuonboyCore/GetSetMuonboyVariable.h"

MuonboyCofiti::MuonboyCofiti(){}

MuonboyCofiti::~MuonboyCofiti(){}


int    MuonboyCofiti::GetISCI(){std::string Name="ISCI"    ; return GetMuboIntVar (Name,NotDef,NotDef,NotDef);}
double MuonboyCofiti::GetDZT1(){std::string Name="DZT1"    ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyCofiti::GetDS1 (){std::string Name="DS1"     ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyCofiti::GetDZT2(){std::string Name="DZT2"    ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyCofiti::GetDS2 (){std::string Name="DS2"     ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyCofiti::GetSMU (){std::string Name="SMU"     ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyCofiti::GetCFI (){std::string Name="CFI"     ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyCofiti::GetSFI (){std::string Name="SFI"     ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyCofiti::GetCFA (){std::string Name="CFA"     ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyCofiti::GetSFA (){std::string Name="SFA"     ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyCofiti::GetZ1  (){std::string Name="Z1"      ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyCofiti::GetS1  (){std::string Name="S1"      ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyCofiti::GetT1  (){std::string Name="T1"      ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyCofiti::GetX1  (){std::string Name="X1"      ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyCofiti::GetY1  (){std::string Name="Y1"      ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyCofiti::GetZ2  (){std::string Name="Z2"      ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyCofiti::GetS2  (){std::string Name="S2"      ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyCofiti::GetT2  (){std::string Name="T2"      ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyCofiti::GetX2  (){std::string Name="X2"      ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyCofiti::GetY2  (){std::string Name="Y2"      ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}

void MuonboyCofiti::SetISCI(int ISCIi   ){std::string Name="ISCI"    ; SetMuboIntVar (Name,NotDef,NotDef,NotDef,ISCIi);}
void MuonboyCofiti::SetDZT1(double DZT1i){std::string Name="DZT1"    ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,DZT1i);}
void MuonboyCofiti::SetDS1 (double DS1i ){std::string Name="DS1"     ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,DS1i );}
void MuonboyCofiti::SetDZT2(double DZT2i){std::string Name="DZT2"    ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,DZT2i);}
void MuonboyCofiti::SetDS2 (double DS2i ){std::string Name="DS2"     ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,DS2i );}
void MuonboyCofiti::SetSMU (double SMUi ){std::string Name="SMU"     ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,SMUi );}
void MuonboyCofiti::SetCFI (double CFIi ){std::string Name="CFI"     ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,CFIi );}
void MuonboyCofiti::SetSFI (double SFIi ){std::string Name="SFI"     ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,SFIi );}
void MuonboyCofiti::SetCFA (double CFAi ){std::string Name="CFA"     ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,CFAi );}
void MuonboyCofiti::SetSFA (double SFAi ){std::string Name="SFA"     ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,SFAi );}
void MuonboyCofiti::SetZ1  (double Z1i  ){std::string Name="Z1"      ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,Z1i  );}
void MuonboyCofiti::SetS1  (double S1i  ){std::string Name="S1"      ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,S1i  );}
void MuonboyCofiti::SetT1  (double T1i  ){std::string Name="T1"      ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,T1i  );}
void MuonboyCofiti::SetX1  (double X1i  ){std::string Name="X1"      ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,X1i  );}
void MuonboyCofiti::SetY1  (double Y1i  ){std::string Name="Y1"      ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,Y1i  );}
void MuonboyCofiti::SetZ2  (double Z2i  ){std::string Name="Z2"      ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,Z2i  );}
void MuonboyCofiti::SetS2  (double S2i  ){std::string Name="S2"      ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,S2i  );}
void MuonboyCofiti::SetT2  (double T2i  ){std::string Name="T2"      ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,T2i  );}
void MuonboyCofiti::SetX2  (double X2i  ){std::string Name="X2"      ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,X2i  );}
void MuonboyCofiti::SetY2  (double Y2i  ){std::string Name="Y2"      ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,Y2i  );}


int MuonboyCofiti::GetISENFI(){std::string Name="ISENFI"   ; return GetMuboIntVar (Name,NotDef,NotDef,NotDef);}
int MuonboyCofiti::GetIFAST (){std::string Name="IFAST"    ; return GetMuboIntVar (Name,NotDef,NotDef,NotDef);}

void MuonboyCofiti::SetISENFI(int ISENFIi){std::string Name="ISENFI"   ; SetMuboIntVar (Name,NotDef,NotDef,NotDef,ISENFIi);}
void MuonboyCofiti::SetIFAST (int IFASTi ){std::string Name="IFAST"    ; SetMuboIntVar (Name,NotDef,NotDef,NotDef,IFASTi);}


double MuonboyCofiti::GetRCHAMA(){std::string Name="RCHAMA"   ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyCofiti::GetRCHAMI(){std::string Name="RCHAMI"   ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyCofiti::GetZCHAMA(){std::string Name="ZCHAMA"   ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}
double MuonboyCofiti::GetZCHAMI(){std::string Name="ZCHAMI"   ; return GetMuboRealVar(Name,NotDef,NotDef,NotDef);}

void MuonboyCofiti::SetRCHAMA(double RCHAMAi){std::string Name="RCHAMA"   ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,RCHAMAi);}
void MuonboyCofiti::SetRCHAMI(double RCHAMIi){std::string Name="RCHAMI"   ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,RCHAMIi);}
void MuonboyCofiti::SetZCHAMA(double ZCHAMAi){std::string Name="ZCHAMA"   ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,ZCHAMAi);}
void MuonboyCofiti::SetZCHAMI(double ZCHAMIi){std::string Name="ZCHAMI"   ; SetMuboRealVar(Name,NotDef,NotDef,NotDef,ZCHAMIi);}


double MuonboyCofiti::GetVTHETAPHI(int Ipara)     {std::string Name="VTHETAPHI"; return GetMuboRealVar(Name,Ipara, NotDef,NotDef);}
double MuonboyCofiti::GetVPERIGEE(int Ipara)      {std::string Name="VPERIGEE" ; return GetMuboRealVar(Name,Ipara, NotDef,NotDef);}

void MuonboyCofiti::SetVTHETAPHI(double VTHETAPHIi,int Ipara)     {std::string Name="VTHETAPHI"; SetMuboRealVar(Name,Ipara, NotDef,NotDef,VTHETAPHIi);}
void MuonboyCofiti::SetVPERIGEE (double VPERIGEEi ,int Ipara)     {std::string Name="VPERIGEE" ; SetMuboRealVar(Name,Ipara, NotDef,NotDef,VPERIGEEi);}


double MuonboyCofiti::GetVDEB(int Ivdeb,int Ipara){std::string Name="VDEB"     ; return GetMuboRealVar(Name,Ipara, Ivdeb, NotDef);}

void MuonboyCofiti::SetVDEB(double VDEBi,int Ivdeb,int Ipara){std::string Name="VDEB"     ; SetMuboRealVar(Name,Ipara ,Ivdeb ,NotDef,VDEBi);}


double MuonboyCofiti::GetVEND(int Ipara)          {std::string Name="VEND"     ; return GetMuboRealVar(Name,Ipara, NotDef,NotDef);}
