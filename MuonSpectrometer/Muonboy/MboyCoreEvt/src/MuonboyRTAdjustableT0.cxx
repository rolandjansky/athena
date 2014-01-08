/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MuonboyRTAdjustableT0.h"
 
MuonboyRTAdjustableT0::MuonboyRTAdjustableT0():MuonboyRTAbstract(){
 p_MuonboyRTAbstract = 0 ;
}
 
MuonboyRTAdjustableT0::~MuonboyRTAdjustableT0(){}
 
void MuonboyRTAdjustableT0::TfTDC(int&  Ident,int& TDCcount,double& TimeFromTDC) {

  p_MuonboyRTAbstract->TfTDC(Ident,TDCcount,TimeFromTDC);

}

std::string MuonboyRTAdjustableT0::PrintRsLine(int Ident, int WireSag, double AdMaxima) {

  return p_MuonboyRTAbstract->PrintRsLine(Ident,WireSag,AdMaxima) ;

}

