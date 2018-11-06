/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/JetAttrsKtDRCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/NonexistantVariable.h" //error handling
#include <sstream>
#include <stdexcept>
#include <iostream>


JetAttrsKtDRCondition::JetAttrsKtDRCondition(
				double limitMin,
				double limitMax
                              ){
  m_limitMin = limitMin;
  m_limitMax = limitMax;
}


bool JetAttrsKtDRCondition::isSatisfied(const HypoJetVector& ips) const{
  if(ips.size() != 1){
    std::stringstream ss;
    ss << "JetAttrs::isSatisfied must see exactly 1 particle, but received "
       << ips.size()
       << '\n';
    
    throw std::runtime_error(ss.str());
  }

  auto jet = ips[0];
  float ktdrVar;
  Jet jetStruct;

  if (jet -> getAttribute("KtDR",ktdrVar)){
    jetStruct.validCondition=true;
    jetStruct.ktdrVal=ktdrVar;
    }
  else{
    jetStruct.validCondition=false;
    throw NonexistantVariable("JetAttrs condition cannot retrieve variable 'KtDR' - does not exist");
    }

  return (passKtDRCut(jetStruct));
 
}


bool JetAttrsKtDRCondition::passKtDRCut(const Jet& jet) const{
  bool result;
  if(not jet.validCondition){return false;}
  (jet.ktdrVal >= m_limitMin and jet.ktdrVal<= m_limitMax) ? result=true : result=false;

  return result;
}


std::string JetAttrsKtDRCondition::toString() const noexcept {
  std::stringstream ss;
  ss << "JetAttrsKtDRCondition: "

     << "limit min: "
     << m_limitMin
     << " limit max: "
     << m_limitMax
     <<'\n';

  return ss.str();
}


double JetAttrsKtDRCondition::orderingParameter() const noexcept {
  return m_limitMin;
}
