/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "NSWCalibData/VMMTimePar.h"
namespace Muon{

void VMMTimePar::setParameters(double slope, double intercept, double t0){
  m_slope=slope;
  m_intercept=intercept;
  m_t0=t0;
}

void VMMTimePar::getParameters(double& slope, double& intercept, double& t0){
  slope=m_slope;
  intercept=m_intercept;
  t0=m_t0;
}
}//end Muon namespace
