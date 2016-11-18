/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCS1DFunction_h
#define TFCS1DFunction_h

// STL includes
#include <string>

#include "TH1.h"
#include "TTree.h"

class TFCS1DFunction:public TObject
{
  public:
    TFCS1DFunction();
    TFCS1DFunction(TH1* hist);
    
    void Initialize(TH1* hist);
    //int testHisto(TH1* hist, std::string, float&, float&, std::string, int, std::string);
    int testHisto(TH1* hist, std::string, float&, float&, std::string, int, int, int, double, int);
    
    virtual double rnd_to_fct(double rnd);
    
    TH1* transform(TH1*, float&, float&);
    double get_maxdev(TH1* h_input, TH1* h_approx);
    void tmvaregression_training(int, TTree *regTree, std::string, std::string);
    double get_range_low(TH1* hist);
    double tmvaregression_application(double, std::string);
    static TH1* get_cumul(TH1* hist);

  private:
 
 ClassDef(TFCS1DFunction,1)  //TFCS1DFunction
};

//#if defined(__MAKECINT__)
//#pragma link C++ class TFCS1DFunction+;
//#endif

#endif
