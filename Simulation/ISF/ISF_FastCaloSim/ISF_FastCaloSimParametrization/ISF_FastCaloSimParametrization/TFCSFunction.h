/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSFunction_h
#define TFCSFunction_h

#include "ISF_FastCaloSimEvent/TFCS1DFunction.h"
//#include "ISF_FastCaloSimParametrization/TFCS1DFunction.h"

class TFCSFunction
{
  public:
    TFCSFunction();
    virtual ~TFCSFunction() {}
    static TFCS1DFunction* Create(TH1* hist,int,int,int,double,double,int);
  private:

  ClassDef(TFCSFunction,1)
};

#endif
