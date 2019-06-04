/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef MUONVMMCHARGEPAR_H
#define MUONVMMCHARGEPAR_H

#include "NSWCalibData/VMMPar.h"
namespace Muon{
class VMMChargePar: public VMMPar{

public:
  VMMChargePar(){};
  ~VMMChargePar(){};
  void  setParameters(double slope, double intercept, double saturation) ;
  void  getParameters(double& slope, double& intercept, double& saturation) ;

private:
 double m_slope, m_intercept,  m_saturation; //slope and intercept from the vmm charge calibration and the saturation cut off in units of charge;  
  

}; 
}
#endif 
