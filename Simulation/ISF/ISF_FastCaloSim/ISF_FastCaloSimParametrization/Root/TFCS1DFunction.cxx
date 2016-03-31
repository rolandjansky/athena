/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimParametrization/TFCS1DFunction.h"

//=============================================
//======= TFCS1DFunction =========
//=============================================

TFCS1DFunction::TFCS1DFunction()
{
}

TFCS1DFunction::TFCS1DFunction(TH1* hist)
{
  Initialize(hist);
}

void TFCS1DFunction::Initialize(TH1* /*hist*/)
{
}

double TFCS1DFunction::rnd_to_fct(double /*rnd*/)
{
  return 0;
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCS1DFunction)

