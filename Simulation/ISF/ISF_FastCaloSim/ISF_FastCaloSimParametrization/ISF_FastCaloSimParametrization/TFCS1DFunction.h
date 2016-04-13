/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCS1DFunction_h
#define TFCS1DFunction_h

#include "TH1.h"
#include "TTree.h"

class TFCS1DFunction:public TObject
{
  public:
    TFCS1DFunction();
    TFCS1DFunction(TH1* hist);
    
    virtual void Initialize(TH1* hist);
    int testHisto(TH1* hist);
    
    virtual double rnd_to_fct(double rnd);

    TH1* transform(TH1*);
    double get_maxdev(TH1* h_input, TH1* h_approx);
    void tmvaregression_training(int, TTree *regTree, std::string);
    double get_range_low(TH1* hist);
    double tmvaregression_application(double, std::string);
    TH1* get_cumul(TH1* hist);

  private:
 
 ClassDef(TFCS1DFunction,1)  //TFCS1DFunction
};


#if defined(__MAKECINT__)
#pragma link C++ class TFCS1DFunction;
#endif

#endif
