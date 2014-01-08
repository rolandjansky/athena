/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GetDecoIdentAMDC_H
#define GetDecoIdentAMDC_H

#include "MuonboyCore/MuonboyCoreMisc.h"

//---------------------------------------------------------------
int GetIDFIXE( 
              int& icur
                        );
int GetISCFIXE( 
              int& icur
                        );
int GetNBFIXE();

int GetNMATTR();

int GetNCOUBAS2();

double GetPASTUB( 
              int& IMDT
                        );
double GetRESTUB( 
              int& IMDT
                        );
int GetISTMDTV( 
              int& IGEOM
                        );
double GetGCECHAV( 
              int& IGEOM
                        );
double GetTCECHAV( 
              int& IGEOM
                        );
double GetFCECHAV( 
              int& IGEOM
                        );
int GetIGEOMA( 
              int& IC,int& INOF,int& INOZ 
                        );
int GetIWCHA( 
              int& IC,int& INOF,int& INOZ 
                        );
int GetISC( 
              int& Jtyp 
                        );

//---------------------------------------------------------------
void GetICINOFINOZICT( 
                         int&  Jtyp,int& Jff,int& Jzz,int& Job,
                         int& IC,int& INOF,int& INOZ, int& ICT
                        );

//---------------------------------------------------------------
void GetDecoIdentAMDCMDT(int& Ident, 
                         std::string& StationName ,
                         int&  Jtyp,int& Jff,int& Jzz,int& Job,
                         int& Jsl,int& Jtube,
                         int&  StationEta,int& StationPhi,
                         int& Multilayer,int& TubeLayer,int& Tube
                        );

void GetDecoIdentAMDCRPC(int& Ident, 
                         std::string& StationName ,
                         int&  Jtyp,int& Jff,int& Jzz,int& Job,
                         int& JSPLI,int& JSL,int& JSZ,int& JSTRI,
                         int&  StationEta,int& StationPhi,
                         int& DoubletR,int& DoubletZ,int& DoubletPhi,int& GasGap,int& MeasuresPhi,int& Strip
                        );

void GetDecoIdentAMDCTGC(int& Ident, 
                         std::string& StationName ,
                         int&  Jtyp,int& Jff,int& Jzz,int& Job,
                         int& JSPLI,int& JSL,int& JSZ,int& JSTRI,
                         int&  StationEta,int& StationPhi,
                         int& GasGap,int& IsStrip,int& Channel
                        );
void GetDecoIdentAMDCCSC(int& Ident, 
                         std::string& StationName ,
                         int&  Jtyp,int& Jff,int& Jzz,int& Job,
                         int& Jsl,int& Jtube,double& SZflag,
                         int&  StationEta,int& StationPhi,
                         int& ChamberLayer,int& WireLayer,int& MeasuresPhi,int& Strip
                        );

//---------------------------------------------------------------
void TranslateToAmdcMDT(
                         std::string& StationName ,
                         int&  StationEta,int& StationPhi,
                         int& Multilayer,int& TubeLayer,int& Tube,
                         int&  Jtyp,int& Jff,int& Jzz,int& Job,
                         int& Jsl,int& Jtube
                        );

void TranslateToAmdcRPC(
                         std::string& StationName ,
                         int&  StationEta,int& StationPhi,
                         int& DoubletR,int& DoubletZ,int& DoubletPhi,int& GasGap,int& MeasuresPhi,int& Strip,
                         int&  Jtyp,int& Jff,int& Jzz,int& Job,
                         int& JSPLI,int& JSL,int& JSZ,int& JSTRI
                        );

void TranslateToAmdcTGC(
                         std::string& StationName ,
                         int&  StationEta,int& StationPhi,
                         int& GasGap,int& IsStrip,int& Channel,
                         int&  Jtyp,int& Jff,int& Jzz,int& Job,
                         int& JSPLI,int& JSL,int& JSZ,int& JSTRI
                        );
void TranslateToAmdcCSC(
                         std::string& StationName ,
                         int&  StationEta,int& StationPhi,
                         int& ChamberLayer,int& WireLayer,int& MeasuresPhi,int& Strip,
                         int&  Jtyp,int& Jff,int& Jzz,int& Job,
                         int& Jsl,int& Jtube,double& SZflag
                        );

#endif
