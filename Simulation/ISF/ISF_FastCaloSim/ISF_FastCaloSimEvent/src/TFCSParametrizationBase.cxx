/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSParametrizationBase.h"
#include <iostream>
#include "TClass.h"

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
  TString opt(option);
  if(!opt.IsWhitespace()) opt="";
  std::cout << opt<<"OBJ " << IsA()->GetName() << " : " << GetName() << " "<<'"'<< GetTitle() <<'"'<<" ("<<this<<")"<< std::endl;

  std::cout << opt <<"  PDGID: ";
  for (std::set<int>::iterator it=pdgid().begin(); it!=pdgid().end(); ++it) std::cout << *it << ", ";
  std::cout << std::endl;
  
  std::cout << opt <<"  Ekin="<<Ekin_nominal()<<" MeV, range "<<Ekin_min()<<" MeV < Ekin < "<<Ekin_max()<<" MeV"<<std::endl;
  std::cout << opt <<"  eta="<<eta_nominal()<<", range "<<eta_min()<<" < eta < "<<eta_max()<<std::endl;
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

//ClassImp(TFCSParametrizationBase)
