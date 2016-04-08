/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSNNLateralShapeParametrization_h
#define TFCSNNLateralShapeParametrization_h

#include "ISF_FastCaloSimParametrization/TFCSLateralShapeParametrization.h"

class TFCSNNLateralShapeParametrization:public TFCSLateralShapeParametrization {
public:
  TFCSNNLateralShapeParametrization(const char* name=0, const char* title=0);

  // simulated one hit position with weight that should be put into simulstate
  // sometime later all hit weights should be resacled such that their final sum is simulstate->E(sample)
  // someone also needs to map all hits into cells
  virtual void simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);
private:
  // NN shape information should be stored as private member variables here

  ClassDef(TFCSNNLateralShapeParametrization,1)  //TFCSNNLateralShapeParametrization
};

#if defined(__MAKECINT__)
#pragma link C++ class TFCSNNLateralShapeParametrization;
#endif

#endif
