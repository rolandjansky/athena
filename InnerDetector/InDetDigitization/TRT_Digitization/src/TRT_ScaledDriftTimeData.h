/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#ifndef TRT_DIGITIZATION_TRT_SCALEDDRIFTTIMEDATA_H
#define TRT_DIGITIZATION_TRT_SCALEDDRIFTTIMEDATA_H

#include "ITRT_DriftTimeData.h"

#include <vector>

class TRT_ScaledDriftTimeData : public ITRT_DriftTimeData {

public:
  // //NB: Assumes ownership of pOriginalData!!
  TRT_ScaledDriftTimeData( ITRT_DriftTimeData* pOriginalData, const double& scaleFactor );

  ~TRT_ScaledDriftTimeData();

  //Interface implementation
  double MaxTabulatedField() const;
  double DriftTimeAtNoField(const double& distance) const;
  double DriftTimeAtMaxField(const double& distance) const;

private:

  ITRT_DriftTimeData* m_pOriginalData;
  const double m_scaleFactor;

};

#endif
