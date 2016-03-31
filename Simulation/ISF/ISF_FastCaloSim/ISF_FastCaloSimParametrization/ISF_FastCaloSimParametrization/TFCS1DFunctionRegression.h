/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCS1DFunctionRegression_h
#define TFCS1DFunctionRegression_h

#include "ISF_FastCaloSimParametrization/TFCS1DFunction.h"

class TFCS1DFunctionRegression:public TFCS1DFunction {
  public:
    TFCS1DFunctionRegression();
    TFCS1DFunctionRegression(TH1* hist);
    
    virtual void Initialize(TH1* hist);
    
    virtual double rnd_to_fct(double rnd);
  private:

  ClassDef(TFCS1DFunctionRegression,1)  //TFCS1DFunctionRegression
};

#if defined(__MAKECINT__)
#pragma link C++ class TFCS1DFunction;
#pragma link C++ class TFCS1DFunctionRegression;
#endif

#endif
