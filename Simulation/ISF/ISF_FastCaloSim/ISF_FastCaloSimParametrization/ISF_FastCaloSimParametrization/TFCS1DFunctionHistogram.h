/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCS1DFunctionHistogram_h
#define TFCS1DFunctionHistogram_h

#include "ISF_FastCaloSimParametrization/TFCS1DFunction.h"

class TFCS1DFunctionHistogram:public TFCS1DFunction {
  public:
    TFCS1DFunctionHistogram();
    TFCS1DFunctionHistogram(TH1* hist);
    
    virtual void Initialize(TH1* hist);
    
    virtual double rnd_to_fct(double rnd);
  private:

  ClassDef(TFCS1DFunctionHistogram,1)  //TFCS1DFunctionHistogram
};

#if defined(__MAKECINT__)
#pragma link C++ class TFCS1DFunction;
#pragma link C++ class TFCS1DFunctionHistogram;
#endif

#endif
