/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimParametrization/TFCSNNLateralShapeParametrization.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"
#include "ISF_FastCaloSimEvent/FastCaloSim_CaloCell_ID.h"

//=============================================
//======= TFCSLateralShapeParametrization =========
//=============================================

TFCSNNLateralShapeParametrization::TFCSNNLateralShapeParametrization(const char* name, const char* title):TFCSLateralShapeParametrizationHitBase(name,title)
{
}

void TFCSNNLateralShapeParametrization::simulate_hit(t_hit& hit,TFCSSimulationState& /*simulstate*/,const TFCSTruthState* /*truth*/, const TFCSExtrapolationState* extrapol)
{
  int cs=calosample();
  hit.eta()=0.5*( extrapol->eta(cs, CaloSubPos::SUBPOS_ENT) + extrapol->eta(cs, CaloSubPos::SUBPOS_EXT) );
  hit.phi()=0.5*( extrapol->phi(cs, CaloSubPos::SUBPOS_ENT) + extrapol->phi(cs, CaloSubPos::SUBPOS_EXT) );
  hit.E()*=1;

  //simulstate.deposit_HIT(cs,hit_eta,hit_phi,hit_weight);
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCSNNLateralShapeParametrization)

