/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimParametrization/TFCSLateralShapeParametrizationHitChain.h"
#include "ISF_FastCaloSimParametrization/TFCSLateralShapeParametrizationHitBase.h"
#include "ISF_FastCaloSimEvent/FastCaloSim_CaloCell_ID.h"
#include <iostream>

//=============================================
//======= TFCSLateralShapeParametrization =========
//=============================================

TFCSLateralShapeParametrizationHitChain::TFCSLateralShapeParametrizationHitChain(const char* name, const char* title):TFCSLateralShapeParametrization(name,title)
{
}

void TFCSLateralShapeParametrizationHitChain::set_geometry(CaloGeometry* geo)
{
  for(TFCSLateralShapeParametrizationHitBase* hitsim : m_chain) hitsim->set_geometry(geo);
}

int TFCSLateralShapeParametrizationHitChain::get_number_of_hits(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const
{
  for(TFCSLateralShapeParametrizationHitBase* hitsim : m_chain) {
    int n=hitsim->get_number_of_hits(simulstate,truth,extrapol);
    std::cout<<"  "<<hitsim->ClassName()<<":"<<hitsim->GetName()<<" #hits="<<n<<std::endl;
    if(n>=0) return n;
  } 
  return -1;
}

void TFCSLateralShapeParametrizationHitChain::simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol)
{
  TFCSLateralShapeParametrizationHitBase::t_hit hit;
  for(TFCSLateralShapeParametrizationHitBase* hitsim : m_chain) {
    hitsim->simulate_hit(hit,simulstate,truth,extrapol);
  } 
}

void TFCSLateralShapeParametrizationHitChain::Print(Option_t *option) const
{
  TFCSLateralShapeParametrization::Print(option);
  for(TFCSLateralShapeParametrizationHitBase* hitsim : m_chain) {
    std::cout<<"  ";
    hitsim->Print();
  } 
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCSLateralShapeParametrizationHitChain)

