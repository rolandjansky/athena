/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSFunction_h
#define TFCSFunction_h

#include "ISF_FastCaloSimParametrization/TFCS1DFunction.h"

class TFCSFunction {
  public:
    static TFCS1DFunction* Create(TH1* hist);
    virtual ~TFCSFunction() {}
  private:

  ClassDef(TFCSFunction,1)  //TFCSFunction
};

#if defined(__MAKECINT__)
#pragma link C++ class TFCSFunction;
#endif

#endif
