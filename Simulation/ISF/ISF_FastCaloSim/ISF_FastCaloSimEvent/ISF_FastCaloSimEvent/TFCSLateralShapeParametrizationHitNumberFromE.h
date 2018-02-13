/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSLateralShapeParametrizationHitNumberFromE_h
#define TFCSLateralShapeParametrizationHitNumberFromE_h

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitBase.h"

#include "TH2.h"
#include "TRandom3.h"


class TFCSLateralShapeParametrizationHitNumberFromE:public TFCSLateralShapeParametrizationHitBase {
public:
  TFCSLateralShapeParametrizationHitNumberFromE(const char* name=0, const char* title=0,double stochastic=0.1,double constant=0);

  int get_number_of_hits(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const;

  void Print(Option_t *option = "") const;
private:
  // simple shape information should be stored as private member variables here
  double m_stochastic;
  double m_constant;

  ClassDef(TFCSLateralShapeParametrizationHitNumberFromE,1)  //TFCSLateralShapeParametrizationHitNumberFromE
};

#if defined(__MAKECINT__)
#pragma link C++ class TFCSLateralShapeParametrizationHitNumberFromE+;
#endif

#endif
