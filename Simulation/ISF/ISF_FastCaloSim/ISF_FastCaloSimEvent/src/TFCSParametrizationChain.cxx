/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSParametrizationChain.h"
#include <algorithm>
#include <iterator>

//=============================================
//======= TFCSParametrizationChain =========
//=============================================

void TFCSParametrizationChain::recalc_pdgid_intersect()
{
  set_pdgid(m_chain[0]->pdgid());
  
  for(auto param: m_chain) {
    std::set< int > tmp;
 
    std::set_intersection(pdgid().begin(), pdgid().end(),
                          param->pdgid().begin(), param->pdgid().end(),
                          std::inserter(tmp,tmp.begin()));  
    set_pdgid(tmp);
  }
}

void TFCSParametrizationChain::recalc_pdgid_union()
{
  set_pdgid(chain()[0]->pdgid());
  
  for(auto param: chain()) {
    std::set< int > tmp;
 
    std::set_union(pdgid().begin(), pdgid().end(),
                   param->pdgid().begin(), param->pdgid().end(),
                   std::inserter(tmp,tmp.begin()));  
    set_pdgid(tmp);
  }
}

void TFCSParametrizationChain::recalc_Ekin_intersect()
{
  set_Ekin(*m_chain[0]);
  
  for(auto param: m_chain) {
    if(param->Ekin_min()>Ekin_min()) set_Ekin_min(param->Ekin_min());
    if(param->Ekin_max()<Ekin_max()) set_Ekin_max(param->Ekin_max());
    if(Ekin_nominal()<Ekin_min() || Ekin_nominal()>Ekin_max()) set_Ekin_nominal(param->Ekin_nominal());
  }

  if(Ekin_nominal()<Ekin_min() || Ekin_nominal()>Ekin_max()) set_Ekin_nominal(0.5*(Ekin_min()+Ekin_max()));
}

void TFCSParametrizationChain::recalc_eta_intersect()
{
  set_eta(*m_chain[0]);
  
  for(auto param: m_chain) {
    if(param->eta_min()>eta_min()) set_eta_min(param->eta_min());
    if(param->eta_max()<eta_max()) set_eta_max(param->eta_max());
    if(eta_nominal()<eta_min() || eta_nominal()>eta_max()) set_eta_nominal(param->eta_nominal());
  }

  if(eta_nominal()<eta_min() || eta_nominal()>eta_max()) set_eta_nominal(0.5*(eta_min()+eta_max()));
}

void TFCSParametrizationChain::recalc_Ekin_eta_intersect()
{
  recalc_Ekin_intersect();
  recalc_eta_intersect();
}

void TFCSParametrizationChain::recalc_Ekin_union()
{
  set_Ekin(*m_chain[0]);
  
  for(auto param: m_chain) {
    if(param->Ekin_min()<Ekin_min()) set_Ekin_min(param->Ekin_min());
    if(param->Ekin_max()>Ekin_max()) set_Ekin_max(param->Ekin_max());
    if(Ekin_nominal()<Ekin_min() || Ekin_nominal()>Ekin_max()) set_Ekin_nominal(param->Ekin_nominal());
  }

  if(Ekin_nominal()<Ekin_min() || Ekin_nominal()>Ekin_max()) set_Ekin_nominal(0.5*(Ekin_min()+Ekin_max()));
}

void TFCSParametrizationChain::recalc_eta_union()
{
  set_eta(*m_chain[0]);
  
  for(auto param: m_chain) {
    if(param->eta_min()<eta_min()) set_eta_min(param->eta_min());
    if(param->eta_max()>eta_max()) set_eta_max(param->eta_max());
    if(eta_nominal()<eta_min() || eta_nominal()>eta_max()) set_eta_nominal(param->eta_nominal());
  }

  if(eta_nominal()<eta_min() || eta_nominal()>eta_max()) set_eta_nominal(0.5*(eta_min()+eta_max()));
}

void TFCSParametrizationChain::recalc_Ekin_eta_union()
{
  recalc_Ekin_union();
  recalc_eta_union();
}

void TFCSParametrizationChain::recalc()
{
  clear();
  if(m_chain.size()==0) return;
  
  recalc_pdgid_intersect();
  recalc_Ekin_eta_intersect();
  
  m_chain.shrink_to_fit();
}

bool TFCSParametrizationChain::is_match_Ekin_bin(int Ekin_bin) const
{
  for(auto param : m_chain) if(!param->is_match_Ekin_bin(Ekin_bin)) return false;
  return true;
}

bool TFCSParametrizationChain::is_match_calosample(int calosample) const
{
  for(auto param : m_chain) if(!param->is_match_calosample(calosample)) return false;
  return true;
}

void TFCSParametrizationChain::simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol)
{
  for(auto param: m_chain) {
    param->simulate(simulstate,truth,extrapol);
  }
}

void TFCSParametrizationChain::Print(Option_t *option) const
{
  TFCSParametrization::Print(option);
  TString opt(option);
  //bool shortprint=opt.Index("short")>=0;
  //bool longprint=msgLvl(MSG::DEBUG) || (msgLvl(MSG::INFO) && !shortprint);
  opt=opt+"- ";

  for(auto param: m_chain) {
    param->Print(opt);
  }
}
