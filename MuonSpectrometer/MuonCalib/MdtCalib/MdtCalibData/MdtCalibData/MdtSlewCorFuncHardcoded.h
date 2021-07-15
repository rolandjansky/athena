/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_MdtSlewCorFuncHardcoded_H
#define MUONCALIB_MdtSlewCorFuncHardcoded_H

#include "MdtCalibData/IMdtSlewCorFunc.h"
#include <vector>

namespace MuonCalib {

/** An implementation of a time slew correction function for AMT digitization chip */
class MdtSlewCorFuncHardcoded : public IMdtSlewCorFunc {
 public:
  explicit MdtSlewCorFuncHardcoded( const CalibFunc::ParVec& vec ) : IMdtSlewCorFunc(vec){}
  inline virtual std::string name() const { return "MdtSlewCorFuncHardCoded"; }
  static unsigned int nUsedPar() { return 0; }
  virtual double correction(double t, double adc) const;
  static double calibrated_p(const double &adc);
  static const std::vector<double> initialize_LUT();
 
 private:
  // s_LUT is vector of timeslew correction values [ns] using ADC ticks as index
  // It is static since all calibration regions use the same correction function.
  static const std::vector<double> s_LUT;

};

}  //namespace MuonCalib

#endif
