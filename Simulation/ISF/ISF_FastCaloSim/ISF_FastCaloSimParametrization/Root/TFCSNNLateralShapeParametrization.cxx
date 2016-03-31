/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimParametrization/TFCSNNLateralShapeParametrization.h"
#include "ISF_FastCaloSimParametrization/FastCaloSim_CaloCell_ID.h"

//=============================================
//======= TFCSLateralShapeParametrization =========
//=============================================

TFCSNNLateralShapeParametrization::TFCSNNLateralShapeParametrization(const char* name, const char* title):TFCSLateralShapeParametrization(name,title)
{
}

void TFCSNNLateralShapeParametrization::simulate(TFCSSimulationState& simulstate,const TFCSTruthState* /*truth*/, const TFCSExtrapolationState* extrapol)
{
  int cs=calosample();
  double hit_eta=0.5*( extrapol->eta(cs,TFCSExtrapolationState::SUBPOS_ENT) + extrapol->eta(cs,TFCSExtrapolationState::SUBPOS_EXT) );
  double hit_phi=0.5*( extrapol->phi(cs,TFCSExtrapolationState::SUBPOS_ENT) + extrapol->phi(cs,TFCSExtrapolationState::SUBPOS_EXT) );
  double hit_weight=1;

  simulstate.deposit_HIT(cs,hit_eta,hit_phi,hit_weight);
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCSNNLateralShapeParametrization)

