/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimParametrization/TFCSLateralShapeParametrizationHitChain.h"
#include "ISF_FastCaloSimParametrization/TFCSLateralShapeParametrizationHitBase.h"
#include "ISF_FastCaloSimEvent/FastCaloSim_CaloCell_ID.h"
#include <iostream>

#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"

//=============================================
//======= TFCSLateralShapeParametrization =========
//=============================================

TFCSLateralShapeParametrizationHitChain::TFCSLateralShapeParametrizationHitChain(const char* name, const char* title):TFCSLateralShapeParametrization(name,title)
{
}

TFCSLateralShapeParametrizationHitChain::TFCSLateralShapeParametrizationHitChain(TFCSLateralShapeParametrizationHitBase* hitsim):TFCSLateralShapeParametrization(TString("hit_chain_")+hitsim->GetName(),TString("hit chain for ")+hitsim->GetTitle())
{
  set_calosample(hitsim->calosample());
  
  set_Ekin_bin(hitsim->Ekin_bin());
  set_Ekin_nominal(hitsim->Ekin_nominal());
  set_Ekin_min(hitsim->Ekin_min());
  set_Ekin_max(hitsim->Ekin_max());
  
  set_eta_nominal(hitsim->eta_nominal());
  set_eta_min(hitsim->eta_min());
  set_eta_max(hitsim->eta_max());

  set_pdgid(hitsim->pdgid());

  m_chain.push_back(hitsim);
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
  // Call get_number_of_hits() only once, as it could contain a random number
  int nhit=get_number_of_hits(simulstate,truth,extrapol);
  float Ehit=simulstate.E(calosample())/nhit;
  for(int i=0;i<nhit;++i) {
    TFCSLateralShapeParametrizationHitBase::t_hit hit; 
    hit.E()=Ehit;
    for(TFCSLateralShapeParametrizationHitBase* hitsim : m_chain) {
      hitsim->simulate_hit(hit,simulstate,truth,extrapol);
    } 
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

