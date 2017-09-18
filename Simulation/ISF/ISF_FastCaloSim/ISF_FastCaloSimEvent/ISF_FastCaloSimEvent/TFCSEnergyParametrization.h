/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCSEnergyParametrization_h
#define ISF_FASTCALOSIMEVENT_TFCSEnergyParametrization_h

#include "ISF_FastCaloSimEvent/TFCSParametrization.h"

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

#endif
