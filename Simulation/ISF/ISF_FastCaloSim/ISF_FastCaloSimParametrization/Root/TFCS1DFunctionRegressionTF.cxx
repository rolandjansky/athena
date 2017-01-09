/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

using namespace std;
#include "ISF_FastCaloSimParametrization/TFCS1DFunctionRegressionTF.h"
#include "TFile.h"
#include "TString.h"
#include "TMath.h"


//=============================================
//======= TFCS1DFunctionRegressionTF =========
//=============================================

using namespace std;

TFCS1DFunctionRegressionTF::TFCS1DFunctionRegressionTF()
{
  m_rangeval = 0;
  m_startval = 0;
}

void TFCS1DFunctionRegressionTF::storeRegression(string weightfilename, float rangeval, float startval)
{
  
  m_rangeval=rangeval;
  m_startval=startval;
  
	get_weights(weightfilename);
 
}

double TFCS1DFunctionRegressionTF::retransform(double value)
{
 
 return (value*m_rangeval+m_startval);
 
}

double TFCS1DFunctionRegressionTF::rnd_to_fct(double rnd)
{
  
  double value=regression_value(rnd); 
  if(m_rangeval>0)
   value=retransform(value);
  return value;
  
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

//ClassImp(TFCS1DFunctionRegressionTF)

