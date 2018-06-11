/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCSInvisibleParametrization_h
#define ISF_FASTCALOSIMEVENT_TFCSInvisibleParametrization_h

#include "ISF_FastCaloSimEvent/TFCSParametrization.h"

class TFCSInvisibleParametrization:public TFCSParametrization {
public:
  TFCSInvisibleParametrization(const char* name=nullptr, const char* title=nullptr):TFCSParametrization(name,title) {};

  virtual bool is_match_Ekin_bin(int /*Ekin_bin*/) const {return true;};
  virtual bool is_match_calosample(int /*calosample*/) const {return true;};

  void simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);
private:

  ClassDef(TFCSInvisibleParametrization,1)  //TFCSInvisibleParametrization
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSInvisibleParametrization+;
#endif

#endif
