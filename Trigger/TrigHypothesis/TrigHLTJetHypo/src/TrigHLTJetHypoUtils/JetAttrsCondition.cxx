/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

  //Consider single jets
  if(ips.size() != 1){
    std::stringstream ss;
    ss << "JetAttrs::isSatisfied must see exactly 1 particles, but received "
       << ips.size()
       << '\n';

    throw std::runtime_error(ss.str());
  }


  auto jet = ips[0];
//  auto trigDecision = true; //all chain bits must be satisfied to keep this true. Any failure will make false
//  auto nmbVars = m_jetVar.size() / m_E.size(); // number of moments considered at each energy point

  std::cout << "amanda - in JetAttrsCondition \n";
  std::cout << "amanda - length of condition input arrays (var,E,min,max): "<< m_jetVar.size() << m_E.size() << m_limitMin.size() << m_limitMax.size() << "\n";

  //first working try
  /*
  //For each jet (defined by energy value), consider all appropriate moments and return true/false for each
  for (unsigned int jetEn=0; jetEn < m_E.size(); jetEn++){
    bool trigDec=true; //resets for each jet energy
    for (unsigned int index=0; index < nmbVars; index++){
      if(m_jetVar[index+nmbVars*jetEn].compare("width")==0){trigDecision*=passWidthCut(jet,index+nmbVars*jetEn);}
      else if(m_jetVar[index+nmbVars*jetEn].compare("ktdr")==0){trigDecision*=passKtDRCut(jet,index+nmbVars*jetEn);}
      else if(m_jetVar[index+nmbVars*jetEn].compare(" ")==0){trigDecision*=true;} //m_has = false so do not consider
      else{trigDecision*=false;} //variable does not yet have passCut method
    }
    trigDecision*=trigDec;
  }  
  */

  //second working try - good for single E
  /*
  //For each jet (defined by energy value), consider all appropriate moments and return true/false for each
  //for (unsigned int jetEn=0; jetEn < m_E.size(); jetEn++){
  if(m_E.size()==1){    
    for (unsigned int index=0; index < m_jetVar.size(); index++){//for each jet variable
      if(m_jetVar[index].compare("width")==0){trigDecision*=passWidthCut(jet,index);}
      else if(m_jetVar[index].compare("ktdr")==0){trigDecision*=passKtDRCut(jet,index);}
      else if(m_jetVar[index].compare(" ")==0){trigDecision*=true;} //m_has = false so do not consider
      else{trigDecision*=false;} //variable does not yet have passCut method
    }
    if(trigDecision){return true;} //if full trigger is satisfied, return true (trigDecision==true)
  }  


  return false;
  */


  //Create vector of decisions. If/when every element is "true" then condition is fully satisfied
  //If has=false, fill decision vector with "true" to ignore. Fill other elements with false. The "false" entries must now be checked with passCut methods below
  std::vector<bool> trigDecision;

  //to keep track of which limits to use
  for(auto var : m_jetVar){
    if(var.compare(" ")==0){
      trigDecision.push_back(true);
    }
    else{trigDecision.push_back(false);}
  } 

  for(unsigned int index=0;index<m_jetVar.size();index++){
    if(!trigDecision[index]){
      if(m_jetVar[index].compare("width")==0){
        trigDecision[index]=passWidthCut(jet,index);
      }
      else if(m_jetVar[index].compare("ktdr")==0){
        trigDecision[index]=passKtDRCut(jet,index);
      }
    }
  }
  


  //if(checkDecision(trigDecision)){return true;}
  //return false;
  return checkDecision(trigDecision);

}


bool JetAttrsCondition::checkDecision(std::vector<bool> dec) const {
  //if vector is full of "true" statments, the product will also be true and trigger satisfied
  //Check multiplication here

  auto nmbVars = m_jetVar.size()/m_E.size();
  bool result=true;
   
  for (unsigned int i=0; i<m_E.size(); i++){
    auto start_itr = std::next(dec.cbegin(),i*nmbVars);
    auto end_itr = std::next(dec.cbegin(), i*nmbVars + nmbVars);
    bool resultE = std::accumulate(start_itr, end_itr, 1, std::multiplies<bool>());
    result*=resultE;
  }

  std::cout << "amanda - In check decision \n";
  std::cout << "amanda - Returning event decision " << result << "\n";

  return result;

}



bool JetAttrsCondition::passWidthCut(pHypoJet j0, int i) const {

  bool result;
  float widthVar;
  Jet jetStruct;

  std::cout << "amanda - in JetAttrsCondition passWidthCut \n";
  std::cout << "amanda - limits " << m_limitMin[i] << " and " << m_limitMax[i] << "\n";

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
