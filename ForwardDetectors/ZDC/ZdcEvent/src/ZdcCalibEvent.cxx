/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : ZdcCalibEvent.cxx
//  Author   : Steinberg
//  Created  : August 2009
//
//  DESCRIPTION:
//     Implementation comments only.  Class level comments go in .h file.
//
//  HISTORY:
//
//  BUGS:
//    
//    
//
//*****************************************************************************

#include "ZdcEvent/ZdcCalibEvent.h"

#include <iostream>
#include <sstream>
#include <iomanip>

ZdcCalibEvent::ZdcCalibEvent()
{
  
  m_towerE_A.clear();
  m_towerE_C.clear();
  m_towerT_A.clear();
  m_towerT_C.clear();

  m_EMCellVec_A.clear();
  m_HADCellVec_A.clear();
  m_HADCellVec_C.clear();

}

void ZdcCalibEvent::print() 
{

  return;
}

double ZdcCalibEvent::getTotalEnergy_A()
{
  double e = 0;
  for (int i = 0;i<4;i++){e += m_towerE_A.at(i);}
  return e;
}

double ZdcCalibEvent::getTotalEnergy_C()
{
  double e = 0;
  for (int i = 0;i<4;i++){e += m_towerE_C.at(i);}
  return e;
}

double ZdcCalibEvent::getEMEnergy_A()
{
  return 0;
}

double ZdcCalibEvent::getHADEnergy_A()
{
  return 0;
}

double ZdcCalibEvent::getHADEnergy_C()
{
  return 0;
}

