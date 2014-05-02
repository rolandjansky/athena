/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArRawConditions/LArCaliPulseParamsVsCalib.h" 
#include <iostream>

LArCaliPulseParamsVsCalib::LArCaliPulseParamsVsCalib() {
  //  std::cout << "LArCaliPulseParamsVsCalib: in constructor" << std::endl ;
  initialize() ;
}

LArCaliPulseParamsVsCalib::~LArCaliPulseParamsVsCalib() {}

void LArCaliPulseParamsVsCalib::set(const HWIdentifier& ChID, 
				    float tcal, float fstep ) {
  //   std::cout << "==>> LArCaliPulseParamsVsCalib::set(const HWIdentifier&,float,float) called!" 
  // 	    << std::endl ;
  initialize() ; // does nothing if already initialized
  LArCaliPulseParamsP t ; 
  t.m_Tcal         = tcal ;
  t.m_Fstep        = fstep ;
  setPdata(ChID,t) ;
}
void LArCaliPulseParamsVsCalib::set(const HWIdentifier& ChID, 
				    float tcal, float fstep, float offset, float dtcal ) {
  //   std::cout << "==>> LArCaliPulseParamsVsCalib::set(const HWIdentifier&,float,float) called!" 
  // 	    << std::endl ;
  initialize() ; // does nothing if already initialized
  LArCaliPulseParamsP t ; 
  t.m_Tcal         = tcal ;
  t.m_Fstep        = fstep ;
  t.m_Offset       = offset ;
  t.m_dTimeCal     = dtcal ;
  setPdata(ChID,t) ;
}
void LArCaliPulseParamsVsCalib::set(const HWIdentifier& ChID, 
				    float tcal, float fstep, float offset, float dtcal, short ncb ) {
  //   std::cout << "==>> LArCaliPulseParamsVsCalib::set(const HWIdentifier&,float,float) called!" 
  // 	    << std::endl ;
  initialize() ; // does nothing if already initialized
  LArCaliPulseParamsP t ; 
  t.m_Tcal         = tcal ;
  t.m_Fstep        = fstep ;
  t.m_Offset       = offset ;
  t.m_dTimeCal     = dtcal ;
  t.m_nCB          = ncb ;
  setPdata(ChID,t) ;
}

