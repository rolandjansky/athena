/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyCoreMisc.h"

#include "MuonboyCore/GetSetMuonboyVariable.h"

// Fortran routines -------------------------------------------------------------

extern "C" void decoidentamdcmdt_(
       int&  Ident,
       char* STANAME,
       int&  Jtyp,int& Jff,int& Jzz,int& Job,
       int& Jsl,int& Jtube,
       int&  StationEta,int& StationPhi,
       int& Multilayer,int& TubeLayer,int& Tube
       );
extern "C" void decoidentamdcrpc_(
       int&  Ident,
       char* STANAME,
       int&  Jtyp,int& Jff,int& Jzz,int& Job,
       int& JSPLI,int& JSL,int& JSZ,int& JSTRI,
       int&  StationEta,int& StationPhi,
       int& DoubletR,int& DoubletZ,int& DoubletPhi,int& GasGap,int& MeasuresPhi,int& Strip
       );
extern "C" void decoidentamdctgc_(
       int&  Ident,
       char* STANAME,
       int&  Jtyp,int& Jff,int& Jzz,int& Job,
       int& JSPLI,int& JSL,int& JSZ,int& JSTRI,
       int&  StationEta,int& StationPhi,
       int& GasGap,int& IsStrip,int& Channel
       );
extern "C" void decoidentamdccsc_(
       int&  Ident,
       char* STANAME,
       int&  Jtyp,int& Jff,int& Jzz,int& Job,
       int& Jsl,int& Jtube,double& SZflag,
       int&  StationEta,int& StationPhi,
       int& ChamberLayer,int& WireLayer,int& MeasuresPhi,int& Strip
       );
extern "C" void translatetoamdcmdt_(
       char* STANAME,
       int&  Jtyp,int& Jff,int& Jzz,int& Job,
       int& Jsl,int& Jtube,
       int&  StationEta,int& StationPhi,
       int& Multilayer,int& TubeLayer,int& Tube
       );
extern "C" void translatetoamdcrpc_(
       char* STANAME,
       int&  Jtyp,int& Jff,int& Jzz,int& Job,
       int& JSPLI,int& JSL,int& JSZ,int& JSTRI,
       int&  StationEta,int& StationPhi,
       int& DoubletR,int& DoubletZ,int& DoubletPhi,int& GasGap,int& MeasuresPhi,int& Strip
       );
extern "C" void translatetoamdctgc_(
       char* STANAME,
       int&  Jtyp,int& Jff,int& Jzz,int& Job,
       int& JSPLI,int& JSL,int& JSZ,int& JSTRI,
       int&  StationEta,int& StationPhi,
       int& GasGap,int& IsStrip,int& Channel
       );
extern "C" void translatetoamdccsc_(
       char* STANAME,
       int&  Jtyp,int& Jff,int& Jzz,int& Job,
       int& Jsl,int& Jtube,double& SZflag,
       int&  StationEta,int& StationPhi,
       int& ChamberLayer,int& WireLayer,int& MeasuresPhi,int& Strip
       );
extern "C" void translatetoamdccsc_(
       char* STANAME,
       int&  Jtyp,int& Jff,int& Jzz,int& Job,
       int& Jsl,int& Jtube,double& SZflag,
       int&  StationEta,int& StationPhi,
       int& ChamberLayer,int& WireLayer,int& MeasuresPhi,int& Strip
       );
extern "C" void getind_(
       int& Jtyp,int& Jff,int& Jzz,int& Job,
       int& IC,int& INOF,int& INOZ, int& ICT
       );
                                 
//---------------------------------------------------------------
int GetIDFIXE( 
              int& icur
){
  std::string VarName="IDFIXE"   ; 
  int I1 = icur    - 1 ;
  int I2 = 0           ;
  int I3 = 0           ;
  int value            ; 
  /*int Status =*/  GetMuboIntVarBaz (VarName, I1, I2, I3, value) ;
  return value ;
}
int GetISCFIXE( 
              int& icur
){
  std::string VarName="ISCFIXE"   ; 
  int I1 = icur    - 1 ;
  int I2 = 0           ;
  int I3 = 0           ;
  int value            ; 
  /*int Status =*/  GetMuboIntVarBaz (VarName, I1, I2, I3, value) ;
  return value ;
}
int GetNBFIXE(){
  std::string VarName="NBFIXE"   ; 
  int I1 = 0 ;
  int I2 = 0 ;
  int I3 = 0 ;
  int value         ; 
  /*int Status =*/  GetMuboIntVarBaz (VarName, I1, I2, I3, value) ;
  return value ;
}
int GetNMATTR(){
  std::string VarName="NMATTR"   ; 
  int I1 = 0 ;
  int I2 = 0 ;
  int I3 = 0 ;
  int value         ; 
  /*int Status =*/  GetMuboIntVarBaz (VarName, I1, I2, I3, value) ;
  return value ;
}
int GetNCOUBAS2(){
  std::string VarName="NCOUBAS2"   ; 
  int I1 = 0 ;
  int I2 = 0 ;
  int I3 = 0 ;
  int value         ; 
  /*int Status =*/  GetMuboIntVarBaz (VarName, I1, I2, I3, value) ;
  return value ;
}
double GetPASTUB( 
              int& IMDT
){
  std::string VarName="PASTUB"   ; 
  int I1 = IMDT   - 1  ;
  int I2 = 0           ;
  int I3 = 0           ;
  double value         ; 
  /*int Status =*/  GetMuboRealVarBaz (VarName, I1, I2, I3, value) ;
  return value ;
}
double GetRESTUB( 
              int& IMDT
){
  std::string VarName="RESTUB"   ; 
  int I1 = IMDT   - 1  ;
  int I2 = 0           ;
  int I3 = 0           ;
  double value         ; 
  /*int Status =*/  GetMuboRealVarBaz (VarName, I1, I2, I3, value) ;
  return value ;
}
int GetISTMDTV( 
              int& IGEOM
){
  std::string VarName="ISTMDTV"   ; 
  int I1 = IGEOM   - 1 ;
  int I2 = 0           ;
  int I3 = 0           ;
  int value            ; 
  /*int Status =*/  GetMuboIntVarBaz (VarName, I1, I2, I3, value) ;
  return value ;
}
double GetGCECHAV( 
              int& IGEOM
){
  std::string VarName="GCECHAV"   ; 
  int I1 = IGEOM   - 1 ;
  int I2 = 0           ;
  int I3 = 0           ;
  double value         ; 
  /*int Status =*/  GetMuboRealVarBaz (VarName, I1, I2, I3, value) ;
  return value ;
}
double GetTCECHAV( 
              int& IGEOM
){
  std::string VarName="TCECHAV"   ; 
  int I1 = IGEOM   - 1 ;
  int I2 = 0           ;
  int I3 = 0           ;
  double value         ; 
  /*int Status =*/  GetMuboRealVarBaz (VarName, I1, I2, I3, value) ;
  return value ;
}
double GetFCECHAV( 
              int& IGEOM
){
  std::string VarName="FCECHAV"   ; 
  int I1 = IGEOM   - 1 ;
  int I2 = 0           ;
  int I3 = 0           ;
  double value         ; 
  /*int Status =*/  GetMuboRealVarBaz (VarName, I1, I2, I3, value) ;
  return value ;
}
int GetIGEOMA( 
              int& IC,int& INOF,int& INOZ 
){
  std::string VarName="IGEOMA"   ; 
  int I1 = IC   - 1 ;
  int I2 = INOF - 1 ;
  int I3 = INOZ - 1 ;
  int value         ; 
  /*int Status =*/  GetMuboIntVarBaz (VarName, I1, I2, I3, value) ;
  return value ;
}
                                 
int GetIWCHA( 
              int& IC,int& INOF,int& INOZ 
){
  std::string VarName="IWCHA"   ; 
  int I1 = IC   - 1 ;
  int I2 = INOF - 1 ;
  int I3 = INOZ - 1 ;
  int value         ; 
  /*int Status =*/  GetMuboIntVarBaz (VarName, I1, I2, I3, value) ;
  return value ;
}
                                 
int GetISC( 
              int& Jtyp 
){
  std::string VarName="ISCISC"   ; 
  int I1 = Jtyp   - 1 ;
  int I2 = 0          ;
  int I3 = 0          ;
  int value           ; 
  /*int Status =*/  GetMuboIntVarBaz (VarName, I1, I2, I3, value) ;
  return value ;
}

//---------------------------------------------------------------
void GetICINOFINOZICT( 
                         int&  Jtyp,int& Jff,int& Jzz,int& Job,
                         int& IC,int& INOF,int& INOZ, int& ICT
){
 getind_(Jtyp,Jff,Jzz,Job,
         IC,INOF,INOZ, ICT
        );
}
                                 
//---------------------------------------------------------------
void GetDecoIdentAMDCMDT(int& Ident, 
                         std::string& StationName ,
                         int&  Jtyp,int& Jff,int& Jzz,int& Job,
                         int& Jsl,int& Jtube,
                         int&  StationEta,int& StationPhi,
                         int& Multilayer,int& TubeLayer,int& Tube
                        ){
 char STANAME[3];
 int  Lvar = 3 ;
 decoidentamdcmdt_( 
       Ident,
       STANAME,
       Jtyp,Jff,Jzz,Job,
       Jsl,Jtube,
       StationEta, StationPhi, 
       Multilayer,TubeLayer,Tube
       );
 StationName.resize(Lvar);
 for (int i= 0; i <Lvar; i++){StationName[i]=STANAME[i];}
                                 
}
void GetDecoIdentAMDCRPC(int& Ident, 
                         std::string& StationName ,
                         int&  Jtyp,int& Jff,int& Jzz,int& Job,
                         int& JSPLI,int& JSL,int& JSZ,int& JSTRI,
                         int&  StationEta,int& StationPhi,
                         int& DoubletR,int& DoubletZ,int& DoubletPhi,int& GasGap,int& MeasuresPhi,int& Strip
                        ){
 char STANAME[3];
 int  Lvar = 3 ;
 decoidentamdcrpc_( 
       Ident,
       STANAME,
       Jtyp,Jff,Jzz,Job,
       JSPLI, JSL, JSZ, JSTRI,
       StationEta, StationPhi, 
       DoubletR, DoubletZ, DoubletPhi, GasGap, MeasuresPhi, Strip
       );
 StationName.resize(Lvar);
 for (int i= 0; i <Lvar; i++){StationName[i]=STANAME[i];}
                                 
}
void GetDecoIdentAMDCTGC(int& Ident, 
                         std::string& StationName ,
                         int&  Jtyp,int& Jff,int& Jzz,int& Job,
                         int& JSPLI,int& JSL,int& JSZ,int& JSTRI,
                         int&  StationEta,int& StationPhi,
                         int& GasGap,int& IsStrip,int& Channel
                        ){
 char STANAME[3];
 int  Lvar = 3 ;
 decoidentamdctgc_( 
       Ident,
       STANAME,
       Jtyp,Jff,Jzz,Job,
       JSPLI, JSL, JSZ, JSTRI,
       StationEta, StationPhi, 
       GasGap,IsStrip,Channel 
       );
 StationName.resize(Lvar);
 for (int i= 0; i <Lvar; i++){StationName[i]=STANAME[i];}
                                 
}
void GetDecoIdentAMDCCSC(int& Ident, 
                         std::string& StationName ,
                         int&  Jtyp,int& Jff,int& Jzz,int& Job,
                         int& Jsl,int& Jtube,double& SZflag,
                         int&  StationEta,int& StationPhi,
                         int& ChamberLayer,int& WireLayer,int& MeasuresPhi,int& Strip
                        ){
 char STANAME[3];
 int  Lvar = 3 ;
 decoidentamdccsc_( 
       Ident, 
       STANAME,
       Jtyp,Jff,Jzz,Job,
       Jsl,Jtube,SZflag,
       StationEta,StationPhi,
       ChamberLayer,WireLayer,MeasuresPhi,Strip
       );
 StationName.resize(Lvar);
 for (int i= 0; i <Lvar; i++){StationName[i]=STANAME[i];}
                                 
}
//---------------------------------------------------------------
void TranslateToAmdcMDT( 
                         std::string& StationName ,
                         int&  StationEta,int& StationPhi,
                         int& Multilayer,int& TubeLayer,int& Tube,
                         int&  Jtyp,int& Jff,int& Jzz,int& Job,
                         int& Jsl,int& Jtube
                        ){
 char STANAME[3];
 int  Lvar = 3 ;
 for (int i= 0; i <Lvar; i++){STANAME[i]=StationName[i];}
 translatetoamdcmdt_( 
       STANAME,
       Jtyp,Jff,Jzz,Job,
       Jsl,Jtube,
       StationEta, StationPhi, 
       Multilayer,TubeLayer,Tube
       );
                                 
}
void TranslateToAmdcRPC( 
                         std::string& StationName ,
                         int&  StationEta,int& StationPhi,
                         int& DoubletR,int& DoubletZ,int& DoubletPhi,int& GasGap,int& MeasuresPhi,int& Strip,
                         int&  Jtyp,int& Jff,int& Jzz,int& Job,
                         int& JSPLI,int& JSL,int& JSZ,int& JSTRI
                        ){
 char STANAME[3];
 int  Lvar = 3 ;
 for (int i= 0; i <Lvar; i++){STANAME[i]=StationName[i];}
 translatetoamdcrpc_( 
       STANAME,
       Jtyp,Jff,Jzz,Job,
       JSPLI, JSL, JSZ, JSTRI,
       StationEta, StationPhi, 
       DoubletR, DoubletZ, DoubletPhi, GasGap, MeasuresPhi, Strip
       );
                                 
}
void TranslateToAmdcTGC( 
                         std::string& StationName ,
                         int&  StationEta,int& StationPhi,
                         int& GasGap,int& IsStrip,int& Channel,
                         int&  Jtyp,int& Jff,int& Jzz,int& Job,
                         int& JSPLI,int& JSL,int& JSZ,int& JSTRI
                        ){
 char STANAME[3];
 int  Lvar = 3 ;
 for (int i= 0; i <Lvar; i++){STANAME[i]=StationName[i];}
 translatetoamdctgc_( 
       STANAME,
       Jtyp,Jff,Jzz,Job,
       JSPLI, JSL, JSZ, JSTRI,
       StationEta, StationPhi, 
       GasGap,IsStrip,Channel 
       );
                                 
}
void TranslateToAmdcCSC( 
                         std::string& StationName ,
                         int&  StationEta,int& StationPhi,
                         int& ChamberLayer,int& WireLayer,int& MeasuresPhi,int& Strip,
                         int&  Jtyp,int& Jff,int& Jzz,int& Job,
                         int& Jsl,int& Jtube,double& SZflag
                        ){
 char STANAME[3];
 int  Lvar = 3 ;
 for (int i= 0; i <Lvar; i++){STANAME[i]=StationName[i];}
 translatetoamdccsc_( 
       STANAME,
       Jtyp,Jff,Jzz,Job,
       Jsl,Jtube,SZflag,
       StationEta,StationPhi,
       ChamberLayer,WireLayer,MeasuresPhi,Strip
       );
                                 
}
