/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSParametrizationPDGIDSelectChain.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/TFCSTruthState.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"
#include <iostream>

//=============================================
//======= TFCSParametrizationPDGIDSelectChain =========
//=============================================

void TFCSParametrizationPDGIDSelectChain::recalc()
{
  clear();
  if(size()==0) return;
  
  recalc_pdgid_union();
  recalc_Ekin_eta_intersect();
  
  chain().shrink_to_fit();
}

void TFCSParametrizationPDGIDSelectChain::simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol)
{
  for(auto param: chain()) {
    if(param->is_match_pdgid(truth->pdgid())) {
      ATH_MSG_DEBUG("pdgid="<<truth->pdgid()<<", now run: "<<param->GetName());
      param->simulate(simulstate,truth,extrapol);
    }  
  }
}

void TFCSParametrizationPDGIDSelectChain::unit_test(TFCSSimulationState* simulstate,TFCSTruthState* truth,TFCSExtrapolationState* extrapol)
{
  if(!simulstate) simulstate=new TFCSSimulationState();
  if(!truth) truth=new TFCSTruthState();
  if(!extrapol) extrapol=new TFCSExtrapolationState();

  TFCSParametrizationPDGIDSelectChain chain("chain","chain");
  chain.setLevel(MSG::DEBUG);

  TFCSParametrization* param;
  param=new TFCSParametrization("A begin all","A begin all");
  param->setLevel(MSG::DEBUG);
  param->set_pdgid(0);
  chain.push_back(param);
  param=new TFCSParametrization("A end all","A end all");
  param->setLevel(MSG::DEBUG);
  param->set_pdgid(0);
  chain.push_back(param);

  for(int i=0;i<3;++i) {
    param=new TFCSParametrization(Form("A%d",i),Form("A %d",i));
    param->setLevel(MSG::DEBUG);
    param->set_pdgid(i);
    chain.push_back(param);
  }

  for(int i=3;i>0;--i) {
    param=new TFCSParametrization(Form("B%d",i),Form("B %d",i));
    param->setLevel(MSG::DEBUG);
    param->set_pdgid(i);
    chain.push_back(param);
  }
  param=new TFCSParametrization("B end all","B end all");
  param->setLevel(MSG::DEBUG);
  param->set_pdgid(1);
  chain.push_back(param);
  param=new TFCSParametrization("B begin all","B begin all");
  param->setLevel(MSG::DEBUG);
  param->set_pdgid(1);
  chain.push_back(param);
  
  std::cout<<"====         Chain setup       ===="<<std::endl;
  chain.Print();
  std::cout<<"==== Simulate with pdgid=0      ===="<<std::endl;
  truth->set_pdgid(0);
  chain.simulate(*simulstate,truth,extrapol);
  std::cout<<"==== Simulate with pdgid=1      ===="<<std::endl;
  truth->set_pdgid(1);
  chain.simulate(*simulstate,truth,extrapol);
  std::cout<<"==== Simulate with pdgid=2      ===="<<std::endl;
  truth->set_pdgid(2);
  chain.simulate(*simulstate,truth,extrapol);
  std::cout<<"==================================="<<std::endl<<std::endl;
}

