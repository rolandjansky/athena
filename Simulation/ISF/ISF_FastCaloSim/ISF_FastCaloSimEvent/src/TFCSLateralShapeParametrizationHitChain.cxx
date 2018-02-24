/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitChain.h"
#include "ISF_FastCaloSimEvent/FastCaloSim_CaloCell_ID.h"

#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"

//=============================================
//======= TFCSLateralShapeParametrization =========
//=============================================

TFCSLateralShapeParametrizationHitChain::TFCSLateralShapeParametrizationHitChain(const char* name, const char* title):TFCSLateralShapeParametrization(name,title),m_number_of_hits_simul(nullptr)
{
}

TFCSLateralShapeParametrizationHitChain::TFCSLateralShapeParametrizationHitChain(TFCSLateralShapeParametrizationHitBase* hitsim):TFCSLateralShapeParametrization(TString("hit_chain_")+hitsim->GetName(),TString("hit chain for ")+hitsim->GetTitle()),m_number_of_hits_simul(nullptr)
{
  set_pdgid_Ekin_eta_Ekin_bin_calosample(*hitsim);
  
  m_chain.push_back(hitsim);
}

void TFCSLateralShapeParametrizationHitChain::set_geometry(ICaloGeometry* geo)
{
  TFCSLateralShapeParametrization::set_geometry(geo);
  if(m_number_of_hits_simul) m_number_of_hits_simul->set_geometry(geo);
}

int TFCSLateralShapeParametrizationHitChain::get_number_of_hits(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const
{
  if(m_number_of_hits_simul) {
    int n=m_number_of_hits_simul->get_number_of_hits(simulstate,truth,extrapol);
    if(n<1) n=1;
    return n;
  }
  for(TFCSLateralShapeParametrizationHitBase* hitsim : m_chain) {
    int n=hitsim->get_number_of_hits(simulstate,truth,extrapol);
    //std::cout<<"  "<<hitsim->ClassName()<<":"<<hitsim->GetName()<<" #hits="<<n<<std::endl;
    if(n>0) return n;
  } 
  return 1;
}

void TFCSLateralShapeParametrizationHitChain::simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol)
{
  // Call get_number_of_hits() only once, as it could contain a random number
  int nhit=get_number_of_hits(simulstate,truth,extrapol);
  float Ehit=simulstate.E(calosample())/nhit;
  
  if(msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG("E("<<calosample()<<")="<<simulstate.E(calosample())<<" #hits="<<nhit);
    for(int i=0;i<nhit;++i) {
      TFCSLateralShapeParametrizationHitBase::Hit hit; 
      hit.E()=Ehit;
      for(TFCSLateralShapeParametrizationHitBase* hitsim : m_chain) {
        if(i<2) hitsim->setLevel(MSG::DEBUG);
         else hitsim->setLevel(MSG::INFO);
        //std::cout<<"  do "<<hitsim->ClassName()<<":"<<hitsim->GetName()<<" hit="<<i<<std::endl;
        hitsim->simulate_hit(hit,simulstate,truth,extrapol);
      } 
    }  
  } else {
    for(int i=0;i<nhit;++i) {
      TFCSLateralShapeParametrizationHitBase::Hit hit; 
      hit.E()=Ehit;
      for(TFCSLateralShapeParametrizationHitBase* hitsim : m_chain) {
        //std::cout<<"  do "<<hitsim->ClassName()<<":"<<hitsim->GetName()<<" hit="<<i<<std::endl;
        hitsim->simulate_hit(hit,simulstate,truth,extrapol);
      } 
    }  
  }  
}

void TFCSLateralShapeParametrizationHitChain::Print(Option_t *option) const
{
  TFCSLateralShapeParametrization::Print(option);
  TString opt(option);
  bool shortprint=opt.Index("short")>=0;
  bool longprint=msgLvl(MSG::DEBUG) || (msgLvl(MSG::INFO) && !shortprint);
  TString optprint=opt;optprint.ReplaceAll("short","");

  if(m_number_of_hits_simul) {
    if(longprint) ATH_MSG_INFO(optprint <<"#:Number of hits simulation:");
    m_number_of_hits_simul->Print(opt+"#:");
  }
  if(longprint) ATH_MSG_INFO(optprint <<"- Simulation chain:");
  for(TFCSLateralShapeParametrizationHitBase* hitsim : m_chain) {
    hitsim->Print(opt+"- ");
  } 
}
