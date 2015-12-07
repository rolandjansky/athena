/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimParametrization/TFCS1DFunctionRegression.h"

//=============================================
//======= TFCS1DFunctionRegression =========
//=============================================

TFCS1DFunctionRegression::TFCS1DFunctionRegression()
{
}

TFCS1DFunctionRegression::TFCS1DFunctionRegression(TH1* hist)
{
  Initialize(hist);
}

void TFCS1DFunctionRegression::Initialize(TH1* /*hist*/)
{
}

double TFCS1DFunctionRegression::rnd_to_fct(double /*rnd*/)
{
  return 0;
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCS1DFunctionRegression)

