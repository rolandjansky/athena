/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/JetAttrsCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/NonexistantVariable.h" //error handling
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <numeric> //to product vector

JetAttrsCondition::JetAttrsCondition(
                std::vector<std::string> jetVar,
                std::vector<double> limitMin,
                std::vector<double> limitMax
                              ){
  m_jetVar = jetVar;
  m_limitMin = limitMin;
  m_limitMax = limitMax;
}

bool JetAttrsCondition::isSatisfied(const HypoJetVector& ips) const{

  //Consider single jets
  if(ips.size() != 1){
    std::stringstream ss;
    ss << "JetAttrs::isSatisfied must see exactly 1 particles, but received "
       << ips.size()
       << '\n';

    throw std::runtime_error(ss.str());
  }


  auto jet = ips[0];
  auto trigDecision = true; //all chain bits must be satisfied to keep this true. Any failure will make false

  //for debugging
  //std::cout << "in JetAttrsCondition \n";
  //std::cout << "length of condition input arrays (var,E,min,max): "<< m_jetVar.size() << m_limitMin.size() << m_limitMax.size() << "\n";

  //second working try - good for single E
  //For each jet (defined by energy value), consider all appropriate moments and return true/false for each
  for (unsigned int index=0; index < m_jetVar.size(); index++){//for each jet variable
    if(m_jetVar[index].compare("width")==0){trigDecision*=passWidthCut(jet,index);}
    else if(m_jetVar[index].compare("ktdr")==0){trigDecision*=passKtDRCut(jet,index);}
    else if(m_jetVar[index].compare(" ")==0){trigDecision*=true;} //m_has = false so do not consider
    else{trigDecision*=false;} //variable does not yet have passCut method
  }


  //if(trigDecision){return true;} //if full trigger is satisfied, return true (trigDecision==true)
  //return false;

  return trigDecision;

}


bool JetAttrsCondition::passWidthCut(pHypoJet j0, int i) const {

  bool result;
  float widthVar;
  Jet jetStruct;

  //for debugging
  //std::cout << "in JetAttrsCondition passWidthCut \n";
  //std::cout << "limits " << m_limitMin[i] << " and " << m_limitMax[i] << "\n";

  if (j0 -> getAttribute("Width",widthVar)){
    jetStruct.validCondition=true;
    jetStruct.value=widthVar;

    //for debugging
    //std::cout << "Width variable exists, value " << widthVar << "\n";
    }
  else{
    jetStruct.validCondition=false;
    throw NonexistantVariable("JetAttrs condition cannot retrieve variable 'width' - does not exist");
    }

  if(not jetStruct.validCondition){return false;}
  (jetStruct.value >= m_limitMin[i] and jetStruct.value<= m_limitMax[i]) ? result=true : result=false;

  //for debugging
  //std::cout << "Returning width result " << result << "\n";

  return result;

}


bool JetAttrsCondition::passKtDRCut(pHypoJet j0, int i) const {

  bool result;
  float ktdrVar;
  Jet jetStruct;

  //for debugging
  //std::cout << "in JetAttrsCondition passKtDRCut \n";
  if (j0 -> getAttribute("KtDR",ktdrVar)){
    jetStruct.validCondition=true;
    jetStruct.value=ktdrVar;

    //for debugging
    //std::cout << "KtDR variable exists, value " << ktdrVar << "\n";
    }
  else{
    jetStruct.validCondition=false;
    throw NonexistantVariable("JetAttrs condition cannot retrieve variable 'ktdr' - does not exist");
    }

  if(not jetStruct.validCondition){return false;}
  (jetStruct.value >= m_limitMin[i] and jetStruct.value<= m_limitMax[i]) ? result=true : result=false;

  //for debugging
  //std::cout << "Returning ktdr result " << result << "\n";

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
