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

  std::cout << "amanda - in JetAttrsKtDRCondition checking isSatisfied with passed double limit values \n";

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

  std::cout << "amanda - look at one jet and pull width value \n";
  if (jet -> getAttribute("KtDR",ktdrVar)){
    jetStruct.validCondition=true;
    jetStruct.ktdrVal=ktdrVar;
    std::cout << "amanda - KtDR variable exists, value " << ktdrVar << "\n";
    }
  else{
    jetStruct.validCondition=false;
    throw NonexistantVariable("JetAttrs condition cannot retrieve variable 'KtDR' - does not exist");
    }

  std::cout << "amanda - calling function to see if cut passed. Returning " << passKtDRCut(jetStruct) << "\n";
  return (passKtDRCut(jetStruct));
 
}


bool JetAttrsKtDRCondition::passKtDRCut(const Jet& jet) const{
  bool result;
  if(not jet.validCondition){return false;}
  (jet.ktdrVal >= m_limitMin and jet.ktdrVal<= m_limitMax) ? result=true : result=false;

  return result;
}


std::string JetAttrsKtDRCondition::toString() const noexcept {

    std::cout << "amanda - checking if ktdr passes cuts \n";
    std::cout << "amanda - jet values: ktdr =  " << jet.widthVal << ", validCondition = "<< jet.validCondition << "\n";
    std::cout << "amanda - limits: limitMin = " << m_limitMin << ", limitMax = "<< m_limitMax << "\n";

  std::stringstream ss;
  ss << "JetAttrsKtDRCondition: "

     << "limit min: "
     << m_limitMin
     << " limit max: "
     << m_limitMax
     <<'\n';

  std::cout << "amanda - Returning result " << result << "\n";

  return ss.str();
}


double JetAttrsKtDRCondition::orderingParameter() const noexcept {
  return m_limitMin;
}
