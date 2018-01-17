/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 ////////////////////////////////////
//                                  //
//  Class: TRT_BarrelDriftTimeData  //
//                                  //
//  Author: Thomas Kittelmann       //
//                                  //
//  First Version: January 2006     //
//                                  //
 ////////////////////////////////////

#ifndef TRT_BARRELDRIFTTIMEDATA_H
#define TRT_BARRELDRIFTTIMEDATA_H

#include "TRT_Digitization/ITRT_DriftTimeData.h"

#include <vector>

class TRT_BarrelDriftTimeData : public ITRT_DriftTimeData {

public:
  TRT_BarrelDriftTimeData(unsigned int digversion, int strawGasType);

  //Interface implementation:

  double MaxTabulatedField() const;
  double DriftTimeAtNoField(const double& distance) const;
  double DriftTimeAtMaxField(const double& distance) const;

private:

  int m_strawGas;

  std::vector<double> m_tabdists_nofield;
  std::vector<double> m_tabdrifttime_nofield;
  std::vector<double> m_tabdists_maxfield;
  std::vector<double> m_tabdrifttime_maxfield;

};

#endif
