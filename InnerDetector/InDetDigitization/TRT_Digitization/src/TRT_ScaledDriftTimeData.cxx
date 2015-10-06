/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 ////////////////////////////////////
//                                  //
//  Class: TRT_ScaledDriftTimeData  //
//                                  //
//  Author: Thomas Kittelmann       //
//                                  //
//  First Version: January 2006     //
//                                  //
 ////////////////////////////////////

#include "TRT_ScaledDriftTimeData.h"

//______________________________________________________________________________
TRT_ScaledDriftTimeData::TRT_ScaledDriftTimeData(ITRT_DriftTimeData* pOriginalData,//Assumes ownership of pOriginalData!!
						 const double& scaleFactor)
  : m_pOriginalData(pOriginalData), m_scaleFactor(scaleFactor) {}

//______________________________________________________________________________
TRT_ScaledDriftTimeData::~TRT_ScaledDriftTimeData() {
  delete m_pOriginalData;
}

//______________________________________________________________________________
double TRT_ScaledDriftTimeData::MaxTabulatedField() const {
  return m_pOriginalData->MaxTabulatedField();
}

//______________________________________________________________________________
double TRT_ScaledDriftTimeData::DriftTimeAtNoField(const double& dist) const {
  return m_pOriginalData->DriftTimeAtNoField(dist) * m_scaleFactor;
}

//______________________________________________________________________________
double TRT_ScaledDriftTimeData::DriftTimeAtMaxField(const double& dist) const {
  return m_pOriginalData->DriftTimeAtMaxField(dist) * m_scaleFactor;
}
