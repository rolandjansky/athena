/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSNNLateralShapeParametrization_h
#define TFCSNNLateralShapeParametrization_h

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitBase.h"

class TFCSNNLateralShapeParametrization:public TFCSLateralShapeParametrizationHitBase {
public:
  TFCSNNLateralShapeParametrization(const char* name=nullptr, const char* title=nullptr);

  // simulated one hit position with weight that should be put into simulstate
  // sometime later all hit weights should be resacled such that their final sum is simulstate->E(sample)
  // someone also needs to map all hits into cells
  virtual void simulate_hit(Hit& hit,TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);
private:
  // NN shape information should be stored as private member variables here

  ClassDef(TFCSNNLateralShapeParametrization,1)  //TFCSNNLateralShapeParametrization
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSNNLateralShapeParametrization+;
#endif

#endif
