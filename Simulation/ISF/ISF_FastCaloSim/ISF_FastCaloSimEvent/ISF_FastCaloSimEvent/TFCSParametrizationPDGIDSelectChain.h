/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCSParametrizationPDGIDSelectChain_h
#define ISF_FASTCALOSIMEVENT_TFCSParametrizationPDGIDSelectChain_h

#include "ISF_FastCaloSimEvent/TFCSParametrizationChain.h"

class TFCSParametrizationPDGIDSelectChain:public TFCSParametrizationChain {
public:
  TFCSParametrizationPDGIDSelectChain(const char* name=nullptr, const char* title=nullptr):TFCSParametrizationChain(name,title) {};
  TFCSParametrizationPDGIDSelectChain(const TFCSParametrizationPDGIDSelectChain& ref):TFCSParametrizationChain(ref) {};

  virtual void simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);

  static void unit_test(TFCSSimulationState* simulstate=nullptr,TFCSTruthState* truth=nullptr,TFCSExtrapolationState* extrapol=nullptr);
protected:
  virtual void recalc();

private:

  ClassDef(TFCSParametrizationPDGIDSelectChain,1)  //TFCSParametrizationPDGIDSelectChain
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSParametrizationPDGIDSelectChain+;
#endif

#endif
