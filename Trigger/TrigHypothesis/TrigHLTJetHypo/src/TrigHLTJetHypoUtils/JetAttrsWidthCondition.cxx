/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/JetAttrsWidthCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/NonexistantVariable.h" //error handling
#include <sstream>
#include <stdexcept>
#include <iostream>


JetAttrsWidthCondition::JetAttrsWidthCondition(
				double limitMin,
				double limitMax
                              ){
  m_limitMin = limitMin;
  m_limitMax = limitMax;
}

bool JetAttrsWidthCondition::isSatisfied(const HypoJetVector& ips) const{
  if(ips.size() != 1){
    std::stringstream ss;
    ss << "JetAttrs::isSatisfied must see exactly 1 particles, but received "
       << ips.size()
       << '\n';
    
    throw std::runtime_error(ss.str());
  }

  auto jet = ips[0];
  float widthVar;
  Jet jetStruct;

  if (jet -> getAttribute("Width",widthVar)){
    jetStruct.validCondition=true;
    jetStruct.widthVal=widthVar;
    }
  else{
    jetStruct.validCondition=false;
    throw NonexistantVariable("JetAttrs condition cannot retrieve variable 'width' - does not exist");
    }

  return (passWidthCut(jetStruct)); 

}


bool JetAttrsWidthCondition::passWidthCut(const Jet& jet) const{
  bool result;
  if(not jet.validCondition){return false;}
  (jet.widthVal >= m_limitMin and jet.widthVal<= m_limitMax) ? result=true : result=false;  

  return result;
}


std::string JetAttrsWidthCondition::toString() const noexcept {
  std::stringstream ss;
  ss << "JetAttrsWidthCondition: "

     << "limit min: "
     << m_limitMin
     << " limit max: "
     << m_limitMax
     <<'\n';

  return ss.str();
}


double JetAttrsWidthCondition::orderingParameter() const noexcept {
  return m_limitMin;
}
