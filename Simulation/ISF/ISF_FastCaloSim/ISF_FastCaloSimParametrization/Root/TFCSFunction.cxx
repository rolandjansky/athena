/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimParametrization/TFCSFunction.h"
#include "ISF_FastCaloSimParametrization/TFCS1DFunctionRegression.h"
#include "ISF_FastCaloSimParametrization/TFCS1DFunctionHistogram.h"

//=============================================
//======= TFCSFunction =========
//=============================================

TFCS1DFunction* TFCSFunction::Create(TH1* hist)
{
// This function is called by the user when he wants a histogram to be transformed into a space efficient variant for the parametrization.
// All code that decides whether a histogram should be transformed into a TFCS1DFunctionRegression or TFCS1DFunctionHistogram
// should go here. 

// For now dummy implementation always using TFCS1DFunctionHistogram

  TFCS1DFunction* fct=new TFCS1DFunctionHistogram(hist);

  return fct;
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCSFunction)

