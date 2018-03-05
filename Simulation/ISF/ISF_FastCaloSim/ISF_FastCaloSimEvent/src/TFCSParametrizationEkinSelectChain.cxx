/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSParametrizationEkinSelectChain.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/TFCSTruthState.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"
#include <iostream>

//=============================================
//======= TFCSParametrizationEkinSelectChain =========
//=============================================

void TFCSParametrizationEkinSelectChain::recalc()
{
  clear();
  if(size()==0) return;
  
  recalc_pdgid_intersect();
  recalc_Ekin_union();
  recalc_eta_intersect();
  
  chain().shrink_to_fit();
}

void TFCSParametrizationEkinSelectChain::push_back_in_bin(TFCSParametrizationBase* param) 
{
  push_back_in_bin(param,param->Ekin_min(),param->Ekin_max());
}

int TFCSParametrizationEkinSelectChain::get_bin(TFCSSimulationState&,const TFCSTruthState* truth, const TFCSExtrapolationState*) const
{
  return val_to_bin(truth->Ekin());
}

const std::string TFCSParametrizationEkinSelectChain::get_variable_text(TFCSSimulationState&,const TFCSTruthState* truth, const TFCSExtrapolationState*) const
{
  return std::string(Form("Ekin=%1.1f",truth->Ekin()));
}

const std::string TFCSParametrizationEkinSelectChain::get_bin_text(int bin) const
{
  if(bin==-1 || bin>=(int)get_number_of_bins()) {
    return std::string(Form("bin=%d not in [%1.1f<=Ekin<%1.1f)",bin,m_bin_low_edge[0],m_bin_low_edge[get_number_of_bins()]));
  }  
  return std::string(Form("bin=%d, %1.1f<=Ekin<%1.1f",bin,m_bin_low_edge[bin],m_bin_low_edge[bin+1]));
}

void TFCSParametrizationEkinSelectChain::unit_test(TFCSSimulationState* simulstate,TFCSTruthState* truth, const TFCSExtrapolationState* extrapol)
{
  if(!simulstate) simulstate=new TFCSSimulationState();
  if(!truth) truth=new TFCSTruthState();
  if(!extrapol) extrapol=new TFCSExtrapolationState();

  TFCSParametrizationEkinSelectChain chain("chain","chain");
  chain.setLevel(MSG::DEBUG);

  TFCSParametrization* param;
  param=new TFCSParametrization("A begin all","A begin all");
  param->setLevel(MSG::DEBUG);
  param->set_Ekin_nominal(2);
  param->set_Ekin_min(2);
  param->set_Ekin_max(5);
  chain.push_before_first_bin(param);
  param=new TFCSParametrization("A end all","A end all");
  param->setLevel(MSG::DEBUG);
  param->set_Ekin_nominal(2);
  param->set_Ekin_min(2);
  param->set_Ekin_max(5);
  chain.push_back(param);

  const int n_params=5;
  for(int i=2;i<n_params;++i) {
    param=new TFCSParametrization(Form("A%d",i),Form("A %d",i));
    param->setLevel(MSG::DEBUG);
    param->set_Ekin_nominal(i*i+0.1);
    param->set_Ekin_min(i*i);
    param->set_Ekin_max((i+1)*(i+1));
    chain.push_back_in_bin(param);
  }
  for(int i=n_params;i>=1;--i) {
    param=new TFCSParametrization(Form("B%d",i),Form("B %d",i));
    param->setLevel(MSG::DEBUG);
    param->set_Ekin_nominal(i*i+0.1);
    param->set_Ekin_min(i*i);
    param->set_Ekin_max((i+1)*(i+1));
    chain.push_back_in_bin(param);
  }

  std::cout<<"====         Chain setup       ===="<<std::endl;
  chain.Print();

  param=new TFCSParametrization("B end all","B end all");
  param->setLevel(MSG::DEBUG);
  chain.push_back(param);
  param=new TFCSParametrization("B begin all","B begin all");
  param->setLevel(MSG::DEBUG);
  chain.push_before_first_bin(param);
  
  std::cout<<"====         Chain setup       ===="<<std::endl;
  chain.Print();
  std::cout<<"==== Simulate with E=0.3      ===="<<std::endl;
  truth->SetPtEtaPhiM(0.3,0,0,0);
  chain.simulate(*simulstate,truth,extrapol);
  std::cout<<"==== Simulate with E=1.0      ===="<<std::endl;
  truth->SetPtEtaPhiM(1,0,0,0);
  chain.simulate(*simulstate,truth,extrapol);
  std::cout<<"==== Simulate with E=1.3      ===="<<std::endl;
  truth->SetPtEtaPhiM(1.3,0,0,0);
  chain.simulate(*simulstate,truth,extrapol);
  std::cout<<"==== Simulate with E=4.3      ===="<<std::endl;
  truth->SetPtEtaPhiM(4.3,0,0,0);
  chain.simulate(*simulstate,truth,extrapol);
  std::cout<<"==== Simulate with E=100      ===="<<std::endl;
  truth->SetPtEtaPhiM(100,0,0,0);
  chain.simulate(*simulstate,truth,extrapol);
  std::cout<<"==================================="<<std::endl<<std::endl;
}


