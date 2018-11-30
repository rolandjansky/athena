/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/JetAttrsCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/NonexistantVariable.h" //error handling
#include <sstream>
#include <stdexcept>
#include <iostream>


JetAttrsCondition::JetAttrsCondition(
                std::vector<std::string> jetVar,
                std::vector<double> E,
                std::vector<double> limitMin,
                std::vector<double> limitMax
                              ){
  m_jetVar = jetVar;
  m_E = E;
  m_limitMin = limitMin;
  m_limitMax = limitMax;
}

bool JetAttrsCondition::isSatisfied(const HypoJetVector& ips) const{

  if(ips.size() != 1){
    std::stringstream ss;
    ss << "JetAttrs::isSatisfied must see exactly 1 particles, but received "
       << ips.size()
       << '\n';

    throw std::runtime_error(ss.str());
  }

  auto jet = ips[0];
  auto trigDecision = true; //all chain bits must be satisfied to keep this true. Any failure will make false

  std::cout << "amanda - in JetAttrsCondition \n";
  std::cout << "amanda - length of condition input arrays (var,E,min,max): "<< m_jetVar.size() << m_E.size() << m_limitMin.size() << m_limitMax.size() << "\n";

  for (unsigned int index=0; index < m_jetVar.size(); index++){
      if(m_jetVar[index].compare("width")==0){trigDecision*=passWidthCut(jet,index);}
      else if(m_jetVar[index].compare("ktdr")==0){trigDecision*=passKtDRCut(jet,index);}
      else if(m_jetVar[index].compare(" ")==0){trigDecision*=true;} //m_has = false so do not consider
      else{trigDecision*=false;} //variable does not yet have passCut method
      }


  return trigDecision;
}


bool JetAttrsCondition::passWidthCut(pHypoJet j0, int i) const {

  bool result;
  float widthVar;
  Jet jetStruct;

  std::cout << "amanda - in JetAttrsCondition passWidthCut \n";
  if (j0 -> getAttribute("Width",widthVar)){
    jetStruct.validCondition=true;
    jetStruct.value=widthVar;
    std::cout << "amanda - Width variable exists, value " << widthVar << "\n";
    }
  else{
    jetStruct.validCondition=false;
    throw NonexistantVariable("JetAttrs condition cannot retrieve variable 'width' - does not exist");
    }

  if(not jetStruct.validCondition){return false;}
  (jetStruct.value >= m_limitMin[i] and jetStruct.value<= m_limitMax[i]) ? result=true : result=false;

  std::cout << "amanda - Returning width result " << result << "\n";

  return result;

}

bool JetAttrsCondition::passKtDRCut(pHypoJet j0, int i) const {

  bool result;
  float ktdrVar;
  Jet jetStruct;

  std::cout << "amanda - in JetAttrsCondition passKtDRCut \n";
  if (j0 -> getAttribute("KtDR",ktdrVar)){
    jetStruct.validCondition=true;
    jetStruct.value=ktdrVar;
    std::cout << "amanda - KtDR variable exists, value " << ktdrVar << "\n";
    }
  else{
    jetStruct.validCondition=false;
    throw NonexistantVariable("JetAttrs condition cannot retrieve variable 'ktdr' - does not exist");
    }

  if(not jetStruct.validCondition){return false;}
  (jetStruct.value >= m_limitMin[i] and jetStruct.value<= m_limitMax[i]) ? result=true : result=false;

  std::cout << "amanda - Returning ktdr result " << result << "\n";

  return result;

}

std::string JetAttrsCondition::toString() const noexcept {
  std::stringstream ss;

  
for (unsigned int index=0; index < m_jetVar.size(); index++){
  ss << "JetAttrsCondition: "
     << "jet moment: "
     << m_jetVar[index]
     << "limit min: "
     << m_limitMin[index]
     << " limit max: "
     << m_limitMax[index]
     <<'\n';
}

  return ss.str();
}


double JetAttrsCondition::orderingParameter() const noexcept {
  return m_limitMin[0];
}
