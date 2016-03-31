/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCS1DFunction_h
#define TFCS1DFunction_h

#include "TH1.h"

class TFCS1DFunction:public TObject {
  public:
    TFCS1DFunction();
    TFCS1DFunction(TH1* hist);
    
    virtual void Initialize(TH1* hist);
    
    virtual double rnd_to_fct(double rnd);
  private:

  ClassDef(TFCS1DFunction,1)  //TFCS1DFunction
};


#if defined(__MAKECINT__)
#pragma link C++ class TFCS1DFunction;
#endif

#endif
