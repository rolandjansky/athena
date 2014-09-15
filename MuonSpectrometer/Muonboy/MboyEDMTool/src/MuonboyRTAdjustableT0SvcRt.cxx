/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyEDMTool/MuonboyRTAdjustableT0SvcRt.h"
#include "MboyEDMTool/AdjustableT0Tool.h"

MuonboyRTAdjustableT0SvcRt::MuonboyRTAdjustableT0SvcRt():MuonboyRTAdjustableT0(){
 p_IAdjustableT0Tool = 0 ;
}
 
MuonboyRTAdjustableT0SvcRt::~MuonboyRTAdjustableT0SvcRt(){}
 
void MuonboyRTAdjustableT0SvcRt::RfT(int& TheRimRTSvc,
                         int& Ident, 
			 double& XXXDCA,double& YYYDCA,double& ZZZDCA,
			 double& XXXDCd,double& YYYDCd,double& ZZZDCd,
			 double& XWiCen,double& YWiCen,double& ZWiCen,
			 double& XWidir,double& YWidir,double& ZWidir,
			 double& RADIUS,
                         int& MboyMdtStatus) {

  p_MuonboyRTAbstract->RfT(TheRimRTSvc,
                           Ident, 
                           XXXDCA,YYYDCA,ZZZDCA,
                           XXXDCd,YYYDCd,ZZZDCd,
                           XWiCen,YWiCen,ZWiCen,
                           XWidir,YWidir,ZWidir,
                            RADIUS,
                           MboyMdtStatus ) ;

}
///////////////////////////////////////////////////////////////////////////////
void  MuonboyRTAdjustableT0SvcRt::DumpT0Shift(){
  p_IAdjustableT0Tool->DumpT0Shift();
}

void  MuonboyRTAdjustableT0SvcRt::SetVerboset0shift(){
  p_IAdjustableT0Tool->SetVerboset0shift();
}

void  MuonboyRTAdjustableT0SvcRt::SetDefaultTShift(double Tshift){
  p_IAdjustableT0Tool->SetDefaultTShift(Tshift);
}

void MuonboyRTAdjustableT0SvcRt::ResetSetTShift(double Tshift){
  p_IAdjustableT0Tool->ResetSetTShift(Tshift);
}

void MuonboyRTAdjustableT0SvcRt::SetStatTShift( std::string stationNameString, int stationEta, int stationPhi, double TShift ){
  p_IAdjustableT0Tool->SetStatTShift(stationNameString,stationEta,stationPhi,TShift);
}

void MuonboyRTAdjustableT0SvcRt::SetEnable(){
  AdjT0::AdjustableT0Tool* pAdjustableT0Tool = dynamic_cast<AdjT0::AdjustableT0Tool*>(p_IAdjustableT0Tool) ;
  if (pAdjustableT0Tool) pAdjustableT0Tool->SetEnable();
}
void MuonboyRTAdjustableT0SvcRt::SetDisable(){
  AdjT0::AdjustableT0Tool* pAdjustableT0Tool = dynamic_cast<AdjT0::AdjustableT0Tool*>(p_IAdjustableT0Tool) ;
  if (pAdjustableT0Tool) pAdjustableT0Tool->SetDisable();
}


