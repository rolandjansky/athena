/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCS2DFunctionRegression_h
#define TFCS2DFunctionRegression_h

#include "ISF_FastCaloSimParametrization/TFCS2DFunction.h"
#include "TTree.h"
#include "TH1.h"
#include <vector>

class TFCS2DFunctionRegression:public TFCS2DFunction
{
  public:
  	
    TFCS2DFunctionRegression();
    TFCS2DFunctionRegression(TH1* hist);
    
    void Initialize(TH1* hist);
    void storeRegression(string,int);
    double rnd_to_fct(std::vector<double>,int);
    
  private:
    
    vector<vector<double> > m_fWeightMatrix0to1;
    vector<vector<double> > m_fWeightMatrix1to2;
    
    double regression_value(std::vector<double> vuniform,int);
    void get_weights(std::string,int);
    void validate(int,std::string,int);
		  
  ClassDef(TFCS2DFunctionRegression,1)  //TFCS1DFunctionRegression
  
};

#endif
