/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitNumberFromE.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"

#include "TMath.h"

//=============================================
//======= TFCSHistoLateralShapeParametrization =========
//=============================================

TFCSLateralShapeParametrizationHitNumberFromE::TFCSLateralShapeParametrizationHitNumberFromE(const char* name, const char* title,double stochastic,double constant) :
  TFCSLateralShapeParametrizationHitBase(name,title),m_stochastic(stochastic),m_constant(constant)
{
}

int TFCSLateralShapeParametrizationHitNumberFromE::get_number_of_hits(TFCSSimulationState& simulstate,const TFCSTruthState* /*truth*/, const TFCSExtrapolationState* /*extrapol*/) const
{
  int cs=calosample();
  double energy=simulstate.E(cs);

  double sigma_stochastic=m_stochastic/sqrt(energy/1000.0);
  int hits = 1.0 / (sigma_stochastic*sigma_stochastic + m_constant*m_constant);

  return gRandom->Poisson(hits);
}

void TFCSLateralShapeParametrizationHitNumberFromE::Print(Option_t *option) const
{
  TString opt(option);
  if(!opt.IsWhitespace()) opt="";
  TFCSLateralShapeParametrizationHitBase::Print(option);

  ATH_MSG_INFO(opt <<"  stochastic="<<m_stochastic<<" constant="<<m_constant);
}
