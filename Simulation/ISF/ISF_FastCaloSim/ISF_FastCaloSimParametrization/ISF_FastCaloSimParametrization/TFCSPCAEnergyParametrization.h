/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSPCAEnergyParametrization_h
#define TFCSPCAEnergyParametrization_h

#include "ISF_FastCaloSimParametrization/TFCSEnergyParametrization.h"

class TFCSPCAEnergyParametrization:public TFCSEnergyParametrization {
public:
  TFCSPCAEnergyParametrization(const char* name=0, const char* title=0);

  // energies in calo layers should be returned in simulstate
  virtual void simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);

  int n_bins() {return 0;};
  // CHANGE: return number of energy parametrization bins
  
private:
  // PCA Matrix and NN mapping information should be stored as private member variables here

  ClassDef(TFCSPCAEnergyParametrization,1)  //TFCSPCAEnergyParametrization
};

#if defined(__MAKECINT__)
#pragma link C++ class TFCSPCAEnergyParametrization;
#endif

#endif
