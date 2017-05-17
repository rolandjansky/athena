/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSFunction_h
#define TFCSFunction_h

#include "ISF_FastCaloSimEvent/TFCS1DFunction.h"

class TFCSFunction
{
  public:
    TFCSFunction() {};
    //virtual ~TFCSFunction() {};
    ~TFCSFunction() {};
    
    static TFCS1DFunction* Create(TH1* hist,int,int,int,double,double,int);
  
  private:

  ClassDef(TFCSFunction,1)
 
};

#if defined(__MAKECINT__)
#pragma link C++ class TFCSFunction+;
#endif

#endif
