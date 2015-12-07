/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSSimpleLateralShapeParametrization_h
#define TFCSSimpleLateralShapeParametrization_h

#include "ISF_FastCaloSimParametrization/TFCSLateralShapeParametrization.h"

class TFCSSimpleLateralShapeParametrization:public TFCSLateralShapeParametrization {
public:
  TFCSSimpleLateralShapeParametrization(const char* name=0, const char* title=0);

  // simulated one hit position with weight that should be put into simulstate
  // sometime later all hit weights should be resacled such that their final sum is simulstate->E(sample)
  // someone also needs to map all hits into cells
  virtual void simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);
private:
  // simple shape information should be stored as private member variables here

  ClassDef(TFCSSimpleLateralShapeParametrization,1)  //TFCSSimpleLateralShapeParametrization
};

#if defined(__MAKECINT__)
#pragma link C++ class TFCSSimpleLateralShapeParametrization;
#endif

#endif
