/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimParametrization/TFCSLateralShapeParametrizationHitBase.h"
#include "ISF_FastCaloSimEvent/FastCaloSim_CaloCell_ID.h"

#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"

#include <iostream>

//=============================================
//======= TFCSLateralShapeParametrization =========
//=============================================

TFCSLateralShapeParametrizationHitBase::TFCSLateralShapeParametrizationHitBase(const char* name, const char* title):TFCSParametrization(name,title),m_Ekin_bin(-1),m_calosample(-1)
{
}

void TFCSLateralShapeParametrizationHitBase::set_Ekin_bin(int bin)
{
  m_Ekin_bin=bin;
}

void TFCSLateralShapeParametrizationHitBase::set_calosample(int cs)
{
  m_calosample=cs;
}

int TFCSLateralShapeParametrizationHitBase::get_number_of_hits(TFCSSimulationState& /*simulstate*/,const TFCSTruthState* /*truth*/, const TFCSExtrapolationState* /*extrapol*/) const
{
  return -1;
}

void TFCSLateralShapeParametrizationHitBase::simulate_hit(t_hit& hit,TFCSSimulationState& simulstate,const TFCSTruthState* /*truth*/, const TFCSExtrapolationState* extrapol)
{
  int cs=calosample();
  hit.eta()=0.5*( extrapol->eta(cs, CaloSubPos::SUBPOS_ENT) + extrapol->eta(cs, CaloSubPos::SUBPOS_EXT) );
  hit.phi()=0.5*( extrapol->phi(cs, CaloSubPos::SUBPOS_ENT) + extrapol->phi(cs, CaloSubPos::SUBPOS_EXT) );
  hit.E()*=1;
}

void TFCSLateralShapeParametrizationHitBase::Print(Option_t *option) const
{
  TString opt(option);
  if(!opt.IsWhitespace()) opt="";
  TFCSParametrization::Print(option);

  std::cout << opt <<"  Ekin_bin="<<Ekin_bin()<<std::endl;
  std::cout << opt <<"  calosample="<<calosample()<<std::endl;
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCSLateralShapeParametrizationHitBase)

