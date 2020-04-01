/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef MUONVMMTIMEPAR_H
#define MUONVMMTIMEPAR_H

#include "NSWCalibData/VMMPar.h"
namespace Muon{
class VMMTimePar: public VMMPar{

public:
  VMMTimePar(){};
  ~VMMTimePar()=default;
  void  setParameters(double slope, double intercept, double t0);
  void  getParameters(double& slope, double& intercept, double& t0);

private:
 double m_slope,m_intercept,m_t0; //slope and intercept from the vmm time calibration 

};
}
#endif 
