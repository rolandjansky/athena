/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/JSSTaggerBase.h"

JSSTaggerBase::JSSTaggerBase(const std::string &name) :
  asg::AsgTool(name),
  m_jetPtMin(200000.),
  m_jetPtMax(3000000.),
  m_jetEtaMax(2.0),
  m_decorate(true),
  m_calibarea(""){}

void JSSTaggerBase::showCuts() const{
  int nCuts = m_accept.getNCuts();
  for(int iCut=0; iCut<nCuts; iCut++){
    std::string cut_string = "";
    cut_string += "  (";
    cut_string += std::to_string(iCut);
    cut_string += ")  ";
    cut_string += m_accept.getCutName(iCut).Data();
    cut_string += " : ";
    cut_string += m_accept.getCutDescription(iCut).Data();
    ATH_MSG_INFO( cut_string );
  }
}
