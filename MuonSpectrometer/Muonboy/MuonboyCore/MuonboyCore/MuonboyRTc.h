/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyRTc_H
#define MuonboyRTc_H


#include "MuonboyCore/MuonboyCoreMisc.h"

extern "C"
void muonboyrft_(int& TheRimRTSvc,
                 int& Ident, 
                 double& XXXDCA,double& YYYDCA,double& ZZZDCA,
                 double& XXXDCd,double& YYYDCd,double& ZZZDCd,
                 double& XWiCen,double& YWiCen,double& ZWiCen,
                 double& XWidir,double& YWidir,double& ZWidir,
                 double& RADIUS);

extern "C"
void dumpt0shift_();

void setverboset0shift_();

extern "C"
void muonboytftdc_(int&  Ident,int& TDCcount,double& TimeFromTDC);

extern "C"
void resetsettshift_(double& TShift );

extern "C"
void incsetstattshift_( char*  stationNamechar, int& stationEta, int& stationPhi, double& TShift );

extern "C"
void setenabletomap_();

extern "C"
void setdisabletomap_();

#endif

