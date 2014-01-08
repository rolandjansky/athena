/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyRTAccess.h"

#include "MboyCoreEvt/MuonboyRTAdjustableT0.h"

extern "C"
void muonboyrft_(int& TheRimRTSvc,
                 int& Ident,
                 double& XXXDCA,double& YYYDCA,double& ZZZDCA,
                 double& XXXDCd,double& YYYDCd,double& ZZZDCd,
                 double& XWiCen,double& YWiCen,double& ZWiCen,
                 double& XWidir,double& YWidir,double& ZWidir,
                 double& RADIUS,
                 int& MboyMdtStatus){

  (MuonboyRTAccess::GetMuonboyRTAbstract())->RfT(TheRimRTSvc,
                                                 Ident,
                                                 XXXDCA, YYYDCA, ZZZDCA,
                                                 XXXDCd, YYYDCd, ZZZDCd,
                                                 XWiCen, YWiCen, ZWiCen,
                                                 XWidir, YWidir, ZWidir,
                                                 RADIUS,
                                                 MboyMdtStatus);

}

extern "C"
void muonboytftdc_(int&  Ident,int& TDCcount,double& TimeFromTDC){

  (MuonboyRTAccess::GetMuonboyRTAbstract())->TfTDC(Ident,TDCcount,TimeFromTDC);

}

extern "C"
void dumpt0shift_(){

  MuonboyRTAbstract* pMuonboyRTAbstract = MuonboyRTAccess::GetMuonboyRTAbstract() ;
  MuonboyRTAdjustableT0* pMuonboyRTAdjustableT0 = dynamic_cast<MuonboyRTAdjustableT0*>(pMuonboyRTAbstract) ;
  if (pMuonboyRTAdjustableT0) pMuonboyRTAdjustableT0->DumpT0Shift();

}

extern "C"
void setverboset0shift_(){

  MuonboyRTAbstract* pMuonboyRTAbstract = MuonboyRTAccess::GetMuonboyRTAbstract() ;
  MuonboyRTAdjustableT0* pMuonboyRTAdjustableT0 = dynamic_cast<MuonboyRTAdjustableT0*>(pMuonboyRTAbstract) ;
  if (pMuonboyRTAdjustableT0) pMuonboyRTAdjustableT0->SetVerboset0shift();

}

extern "C"
void resetsettshift_(double& Tshift){

  MuonboyRTAbstract* pMuonboyRTAbstract = MuonboyRTAccess::GetMuonboyRTAbstract() ;
  MuonboyRTAdjustableT0* pMuonboyRTAdjustableT0 = dynamic_cast<MuonboyRTAdjustableT0*>(pMuonboyRTAbstract) ;
  if (pMuonboyRTAdjustableT0) pMuonboyRTAdjustableT0->ResetSetTShift(Tshift);

}

extern "C"
void incsetstattshift_( char*  stationNamechar, int& stationEta, int& stationPhi, double& TShift ){

  std::string stationNameString = "XYZ" ;
  for (int i= 0; i <3; i++){stationNameString[i]=stationNamechar[i];}
  MuonboyRTAbstract* pMuonboyRTAbstract = MuonboyRTAccess::GetMuonboyRTAbstract() ;
  MuonboyRTAdjustableT0* pMuonboyRTAdjustableT0 = dynamic_cast<MuonboyRTAdjustableT0*>(pMuonboyRTAbstract) ;
  if (pMuonboyRTAdjustableT0) pMuonboyRTAdjustableT0->SetStatTShift( stationNameString, stationEta, stationPhi, TShift );

}

extern "C"
void setenabletomap_(){
  MuonboyRTAbstract* pMuonboyRTAbstract = MuonboyRTAccess::GetMuonboyRTAbstract() ;
  MuonboyRTAdjustableT0* pMuonboyRTAdjustableT0 = dynamic_cast<MuonboyRTAdjustableT0*>(pMuonboyRTAbstract) ;
  if (pMuonboyRTAdjustableT0) pMuonboyRTAdjustableT0->SetEnable();
}
extern "C"
void setdisabletomap_(){
  MuonboyRTAbstract* pMuonboyRTAbstract = MuonboyRTAccess::GetMuonboyRTAbstract() ;
  MuonboyRTAdjustableT0* pMuonboyRTAdjustableT0 = dynamic_cast<MuonboyRTAdjustableT0*>(pMuonboyRTAbstract) ;
  if (pMuonboyRTAdjustableT0) pMuonboyRTAdjustableT0->SetDisable();
}
