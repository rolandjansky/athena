/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSLateralShapeParametrizationFluctChain_h
#define TFCSLateralShapeParametrizationFluctChain_h

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitChain.h"

class TFCSLateralShapeParametrizationFluctChain:public TFCSLateralShapeParametrizationHitChain {
public:
  TFCSLateralShapeParametrizationFluctChain(const char* name=nullptr, const char* title=nullptr);
  TFCSLateralShapeParametrizationFluctChain(TFCSLateralShapeParametrizationHitBase* hitsim);

  virtual FCSReturnCode simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) override;

  void Print(Option_t *option) const override;
private:
  float m_RMS=1.0;
  
  ClassDefOverride(TFCSLateralShapeParametrizationFluctChain,1)  //TFCSLateralShapeParametrizationFluctChain
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSLateralShapeParametrizationFluctChain+;
#endif

#endif
