/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCS1DFunctionRegression_h
#define TFCS1DFunctionRegression_h

#include "ISF_FastCaloSimEvent/TFCS1DFunction.h"
//#include "ISF_FastCaloSimParametrization/TFCS1DFunction.h"
#include "TTree.h"
#include "TH1.h"
#include <vector>

class TFCS1DFunctionRegression:public TFCS1DFunction
{
  public:
  	
    TFCS1DFunctionRegression();
    TFCS1DFunctionRegression(TH1* hist);
    
    virtual void Initialize(TH1* hist);
    void storeRegression(string);
    virtual double rnd_to_fct(double rnd);
    
  //ACH private:
    
    vector<vector<double> > m_fWeightMatrix0to1;
    vector<vector<double> > m_fWeightMatrix1to2;
    
    double regression_value(double uniform);
    void get_weights(std::string);
    void validate(int,std::string);
		  
  ClassDef(TFCS1DFunctionRegression,1)  //TFCS1DFunctionRegression
  
};

#endif
