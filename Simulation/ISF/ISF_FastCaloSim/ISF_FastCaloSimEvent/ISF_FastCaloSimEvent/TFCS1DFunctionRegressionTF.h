/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCS1DFunctionRegressionTF_h
#define ISF_FASTCALOSIMEVENT_TFCS1DFunctionRegressionTF_h

#include "ISF_FastCaloSimEvent/TFCS1DFunctionRegression.h"
#include "TH1.h"
#include <vector>

class TFCS1DFunctionRegressionTF:public TFCS1DFunctionRegression
{
  public:

    TFCS1DFunctionRegressionTF() {};
    TFCS1DFunctionRegressionTF(float, float);
    ~TFCS1DFunctionRegressionTF() {};

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
