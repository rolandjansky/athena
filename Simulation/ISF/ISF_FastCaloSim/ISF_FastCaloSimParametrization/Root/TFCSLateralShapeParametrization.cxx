/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimParametrization/TFCSLateralShapeParametrization.h"
#include "ISF_FastCaloSimParametrization/FastCaloSim_CaloCell_ID.h"
#include <iostream>

//=============================================
//======= TFCSLateralShapeParametrization =========
//=============================================

TFCSLateralShapeParametrization::TFCSLateralShapeParametrization(const char* name, const char* title):TFCSParametrization(name,title),m_Ekin_bin(-1),m_calosample(-1)
{
}

void TFCSLateralShapeParametrization::set_Ekin_bin(int bin)
{
  m_Ekin_bin=bin;
}

void TFCSLateralShapeParametrization::set_calosample(int cs)
{
  m_calosample=cs;
}

void TFCSLateralShapeParametrization::simulate(TFCSSimulationState& simulstate,const TFCSTruthState* /*truth*/, const TFCSExtrapolationState* extrapol)
{
  int cs=calosample();
  double hit_eta=0.5*( extrapol->eta(cs,TFCSExtrapolationState::SUBPOS_ENT) + extrapol->eta(cs,TFCSExtrapolationState::SUBPOS_EXT) );
  double hit_phi=0.5*( extrapol->phi(cs,TFCSExtrapolationState::SUBPOS_ENT) + extrapol->phi(cs,TFCSExtrapolationState::SUBPOS_EXT) );
  double hit_weight=1;

  simulstate.deposit_HIT(cs,hit_eta,hit_phi,hit_weight);
}

void TFCSLateralShapeParametrization::Print(Option_t *option) const
{
  TFCSParametrization::Print(option);

  std::cout <<"  Ekin_bin="<<Ekin_bin()<<std::endl;
  std::cout <<"  calosample="<<calosample()<<std::endl;
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCSLateralShapeParametrization)

