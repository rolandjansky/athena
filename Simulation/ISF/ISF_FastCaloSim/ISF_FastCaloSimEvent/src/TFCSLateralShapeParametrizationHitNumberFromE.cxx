/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitNumberFromE.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"

#include "TMath.h"

#include <iostream>

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
  /*
  if(cs >= 0 && cs<= 7) {
    stochastic=0.101;     //LAr      10.1%/sqrt(E)
    constant=0.002;
  }

  if(cs >= 8 && cs <= 11) {
    stochastic=0.214;     //HadEC 21.4%/sqrt(E)
    constant=0.0;
  }

  if(cs >= 12 && cs <= 20) {
    stochastic=0.564;     //TileCal 56.4%/sqrt(E)
    constant=0.055;
  }

  if(cs >= 21 && cs <= 23) {
    stochastic=0.285;     //FCAL    28.5%/sqrt(E)
    constant=0.035;
  }
  */

  double sigma_stochastic=m_stochastic/sqrt(energy/1000.0);
  int hits = 1.0 / (sigma_stochastic*sigma_stochastic + m_constant*m_constant);

  return gRandom->Poisson(hits);
}

void TFCSLateralShapeParametrizationHitNumberFromE::Print(Option_t *option) const
{
  TString opt(option);
  if(!opt.IsWhitespace()) opt="";
  TFCSLateralShapeParametrizationHitBase::Print(option);

  std::cout << opt <<"  stochastic="<<m_stochastic<<" constant="<<m_constant<<std::endl;
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCSLateralShapeParametrizationHitNumberFromE)
