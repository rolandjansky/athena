/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSParametrizationChain.h"
#include <iostream>
#include <algorithm>
#include <iterator>

//=============================================
//======= TFCSParametrizationChain =========
//=============================================

void TFCSParametrizationChain::recalc()
{
  clear();
  if(m_chain.size()==0) return;

  set_pdgid(m_chain[0]->pdgid());
  set_Ekin_min(m_chain[0]->Ekin_min());
  set_Ekin_max(m_chain[0]->Ekin_max());
  set_eta_min(m_chain[0]->eta_min());
  set_eta_max(m_chain[0]->eta_max());
  
  for(auto param: m_chain) {
    std::set< int > tmp;
 
    std::set_intersection(pdgid().begin(), pdgid().end(),
                          param->pdgid().begin(), param->pdgid().end(),
                          std::inserter(tmp,tmp.begin()));  
    set_pdgid(tmp);
    if(param->Ekin_min()>Ekin_min()) set_Ekin_min(param->Ekin_min());
    if(param->Ekin_max()<Ekin_max()) set_Ekin_max(param->Ekin_max());
    if(param->eta_min()>eta_min()) set_eta_min(param->eta_min());
    if(param->eta_max()<eta_max()) set_eta_max(param->eta_max());
  }

  set_Ekin_nominal(0.5*(Ekin_min()+Ekin_max()));
  set_eta_nominal(0.5*(eta_min()+eta_max()));
}

void TFCSParametrizationChain::set_geometry(ICaloGeometry* geo)
{
  for(auto param : m_chain) param->set_geometry(geo);
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
  if(!opt.IsWhitespace()) opt=option; else opt=opt+"  ";

  for(auto param: m_chain) {
    param->Print(opt);
  }
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

//ClassImp(TFCSParametrizationChain)
