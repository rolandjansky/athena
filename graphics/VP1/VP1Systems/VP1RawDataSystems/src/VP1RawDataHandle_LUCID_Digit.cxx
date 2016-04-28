/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1RawDataHandle_LUCID_Digit      //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2009                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1RawDataSystems/VP1RawDataHandle_LUCID_Digit.h"

//#include "LUCID_RawEvent/LUCID_RawEvent/LUCID_Digit.h" // CMT compilation
#include "LUCID_RawEvent/LUCID_Digit.h"   // new CMake compilation



//____________________________________________________________________
VP1RawDataHandle_LUCID_Digit::VP1RawDataHandle_LUCID_Digit(VP1RawDataCollBase* coll,const LUCID_Digit *data)
  : VP1RawDataHandle_LUCID(coll),m_data(data)
{
}

//____________________________________________________________________
VP1RawDataHandle_LUCID_Digit::~VP1RawDataHandle_LUCID_Digit()
{
}

//____________________________________________________________________
QStringList VP1RawDataHandle_LUCID_Digit::clicked(bool verbose) const
{
  QStringList l;
  l <<" ====> LUCID_Digit:";
  l += commonClicked("   ");
  if (verbose)
    l << "   (no extra verbose information available)";
  return l;
}

unsigned short VP1RawDataHandle_LUCID_Digit::getTubeID() const { return m_data->getTubeID(); }
unsigned short VP1RawDataHandle_LUCID_Digit::getQDC() const { return m_data->getQDC(); }
unsigned short VP1RawDataHandle_LUCID_Digit::getTDC() const { return m_data->getTDC(); }
bool VP1RawDataHandle_LUCID_Digit::isSideA() const { return const_cast<LUCID_Digit *>(m_data)->isSideA(); }//const_cast due to const-incorrectness in LUCID_Digit.h
bool VP1RawDataHandle_LUCID_Digit::isSideC() const { return const_cast<LUCID_Digit *>(m_data)->isSideC(); }//const_cast due to const-incorrectness in LUCID_Digit.h
