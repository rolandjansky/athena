/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "NSWCalibData/VMMChargePar.h"
namespace Muon{


void VMMChargePar::setParameters(double slope, double intercept, double saturation){
  m_slope=slope;
  m_intercept=intercept;
  m_saturation=saturation;
}

void VMMChargePar::getParameters(double& slope, double& intercept, double& saturation){
  slope=m_slope;
  intercept=m_intercept;
  saturation=m_saturation;
}
}//end Muon namespace
