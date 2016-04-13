/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCS1DFunctionRegression_h
#define TFCS1DFunctionRegression_h

#include "ISF_FastCaloSimParametrization/TFCS1DFunction.h"
#include "TTree.h"
#include "TH1.h"
#include <vector>

class TFCS1DFunctionRegression:public TFCS1DFunction
{
  public:
  	
    TFCS1DFunctionRegression();
    TFCS1DFunctionRegression(TH1* hist);
    TFCS1DFunctionRegression(std::string);
    
    virtual void Initialize(TH1* hist);
    virtual void Initialize(std::string);
    virtual double rnd_to_fct(double rnd);

  private:
    
    std::vector<std::vector<double> > m_fWeightMatrix0to1;
    std::vector<std::vector<double> > m_fWeightMatrix1to2;
    
    double regression_value(double uniform);
    void get_weights(std::string);
    void validate(int,std::string);
	  
  ClassDef(TFCS1DFunctionRegression,1)  //TFCS1DFunctionRegression
  
};

#if defined(__MAKECINT__)
#pragma link C++ class TFCS1DFunction;
#pragma link C++ class TFCS1DFunctionRegression;
#endif

#endif
