/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSParametrizationBase.h"
#include <iostream>

//=============================================
//======= TFCSParametrizationBase =========
//=============================================

std::set< int > TFCSParametrizationBase::m_no_pdgid;

TFCSParametrizationBase::TFCSParametrizationBase(const char* name, const char* title):TNamed(name,title)
{
}

void TFCSParametrizationBase::simulate(TFCSSimulationState& /*simulstate*/,const TFCSTruthState* /*truth*/, const TFCSExtrapolationState* /*extrapol*/)
{
}

void TFCSParametrizationBase::Print(Option_t *option) const
{
  TNamed::Print(option);

  std::cout <<"  PDGID: ";
  for (std::set<int>::iterator it=pdgid().begin(); it!=pdgid().end(); ++it) std::cout << *it << ", ";
  std::cout << std::endl;
  
  std::cout <<"  Ekin="<<Ekin_nominal()<<" MeV, range "<<Ekin_min()<<" MeV < Ekin < "<<Ekin_max()<<" MeV"<<std::endl;
  std::cout <<"  eta="<<eta_nominal()<<", range "<<eta_min()<<" < eta < "<<eta_max()<<std::endl;
}

