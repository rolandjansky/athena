/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSEnergyParametrization_h
#define TFCSEnergyParametrization_h

#include "ISF_FastCaloSimParametrization/TFCSParametrization.h"

class TFCSEnergyParametrization:public TFCSParametrization {
public:
  TFCSEnergyParametrization(const char* name=0, const char* title=0);

  virtual bool is_match_Ekin_bin(int /*Ekin_bin*/) const {return true;};
  virtual bool is_match_calosample(int /*calosample*/) const {return true;};
  
  // return number of energy parametrization bins
  virtual int n_bins() {return 0;};
  
private:

  ClassDef(TFCSEnergyParametrization,1)  //TFCSEnergyParametrization
};

#if defined(__MAKECINT__)
#pragma link C++ class TFCSEnergyParametrization;
#endif

#endif
