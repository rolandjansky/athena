/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibData/MdtSlewCorFuncHardcoded.h"
#include "iostream"
#include "cmath"

namespace MuonCalib {

std::map<short, float> MdtSlewCorFuncHardcoded::m_LUT=std::map<short, float>();

double MdtSlewCorFuncHardcoded::correction(double /*t*/, double adc) const {
  if(adc>500 || adc<0) return 0;
  if(m_LUT.size()==0) {
    float last_val(9e9);
    for(short i=0;i<500; i++) {
      float new_val=109/calibrated_p(i);
      if(last_val - new_val > 0.2) {
	last_val=new_val;
	m_LUT[i]=new_val;
      }
      if (new_val<0.1) break;
    }
  }
  return m_LUT.lower_bound(static_cast<short>(adc))->second;
}

double MdtSlewCorFuncHardcoded::calibrated_p(const double &adc) const {
  double w = adc*25.0/32.0;
  return std::exp(1.11925e+00 + 2.08708e-02*w);
}

}  //namespace MuonCalib
