/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimParametrization/TFCS1DFunctionHistogram.h"

//=============================================
//======= TFCSFunctionRegression =========
//=============================================

TFCS1DFunctionHistogram::TFCS1DFunctionHistogram()
{
}

TFCS1DFunctionHistogram::TFCS1DFunctionHistogram(TH1* hist)
{
  Initialize(hist);
}

void TFCS1DFunctionHistogram::Initialize(TH1* /*hist*/)
{
}

double TFCS1DFunctionHistogram::rnd_to_fct(double /*rnd*/)
{
  return 0;
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCS1DFunctionHistogram)

