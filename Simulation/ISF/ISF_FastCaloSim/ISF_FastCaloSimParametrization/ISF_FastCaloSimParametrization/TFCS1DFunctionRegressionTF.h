/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCS1DFunctionRegressionTF_h
#define TFCS1DFunctionRegressionTF_h

#include "ISF_FastCaloSimParametrization/TFCS1DFunctionRegression.h"
#include "TH1.h"
#include <vector>

class TFCS1DFunctionRegressionTF:public TFCS1DFunctionRegression
{
  public:
  	
    TFCS1DFunctionRegressionTF();
    void storeRegression(string, float, float);

    virtual double rnd_to_fct(double rnd);
    double retransform(double value);
    
  private:
    
    vector<vector<double> > m_fWeightMatrix0to1;
    vector<vector<double> > m_fWeightMatrix1to2;
    float m_rangeval;
    float m_startval;
		
  ClassDef(TFCS1DFunctionRegressionTF,1)
  
};

#endif
