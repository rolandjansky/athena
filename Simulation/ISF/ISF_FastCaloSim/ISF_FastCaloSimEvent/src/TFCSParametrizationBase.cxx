/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSParametrizationBase.h"
#include "TClass.h"

//=============================================
//======= TFCSParametrizationBase =========
//=============================================

std::set< int > TFCSParametrizationBase::m_no_pdgid;

#ifndef __FastCaloSimStandAlone__
//Initialize only in constructor to make sure the needed services are ready
Athena::MsgStreamMember* TFCSParametrizationBase::s_msg(nullptr); 
#endif

#if defined(__FastCaloSimStandAlone__)
TFCSParametrizationBase::TFCSParametrizationBase(const char* name, const char* title):TNamed(name,title),m_msg(&std::cout),m_level(MSG::INFO)
{
}
#else
TFCSParametrizationBase::TFCSParametrizationBase(const char* name, const char* title):TNamed(name,title)
{
  if(s_msg==nullptr) s_msg=new Athena::MsgStreamMember("FastCaloSimParametrization");
}
#endif

void TFCSParametrizationBase::simulate(TFCSSimulationState& /*simulstate*/,const TFCSTruthState* /*truth*/, const TFCSExtrapolationState* /*extrapol*/)
{
}

void TFCSParametrizationBase::Print(Option_t *option) const
{
  TString opt(option);
  if(!opt.IsWhitespace()) opt="";
  ATH_MSG_INFO(opt<<'"'<< GetTitle() <<'"'<<" ("<<IsA()->GetName()<<"*)"<<this);
  
  if(msgLvl(MSG::INFO)) {
    ATH_MSG(INFO) << opt <<"  PDGID: ";
    for (std::set<int>::iterator it=pdgid().begin(); it!=pdgid().end(); ++it) {
      if(it!=pdgid().begin()) msg() << ", ";
      msg() << *it;
    }  
    msg() << endmsg;
  }
  
  ATH_MSG_INFO(opt <<"  Ekin="<<Ekin_nominal()<<" MeV, range "<<Ekin_min()<<" MeV < Ekin < "<<Ekin_max()<<" MeV");
  ATH_MSG_INFO(opt <<"  eta="<<eta_nominal()<<", range "<<eta_min()<<" < eta < "<<eta_max());
}

