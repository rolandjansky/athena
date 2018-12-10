/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTJet_JetAttrs.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR: A. Steinhebel
//
// ********************************************************************

#include "GaudiKernel/StatusCode.h"
#include "TrigHLTJetHypo/TrigHLTJetHypo_JetAttrs.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/conditionsFactory2.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsSorter.h"
//#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/JetAttrsFactory.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SingleJetGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetAttrsValueInterpreter.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/WidthInterpreter.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/KtDRInterpreter.h"

#include <map>
#include <memory>

TrigHLTJetHypo_JetAttrs::TrigHLTJetHypo_JetAttrs(const std::string& name,
                   ISvcLocator* pSvcLocator):
TrigHLTJetHypoBase(name, pSvcLocator) {

    declareProperty("momentstr",   m_momentstr);
    declareProperty("E",   m_E);
    declareProperty("has",   m_has);
    declareProperty("jetVars",   m_jetVars);
    declareProperty("limit_mins",   m_limitMins);
    declareProperty("limit_maxs",   m_limitMaxs);
}


TrigHLTJetHypo_JetAttrs::~TrigHLTJetHypo_JetAttrs(){
}


HLT::ErrorCode TrigHLTJetHypo_JetAttrs::hltInitialize()
{
  ATH_MSG_INFO("amanda - in initialize()");
  ATH_MSG_INFO("amanda - momentstr = " << m_momentstr);

  //initialize map (m_conversionMap)
  m_conversionMap["width"] = std::make_unique<WidthInterpreter>();
  m_conversionMap.emplace("ktdr", std::make_unique<KtDRInterpreter>());

  return TrigHLTJetHypoBase::hltInitialize();
}


Conditions TrigHLTJetHypo_JetAttrs::getConditions() const {
    
    ATH_MSG_INFO("amanda - in getConditions()");

    //define vectors to be passed to condition    
    std::string match ("yes");
    std::vector<std::string> jetVarVec ;
    std::vector<double> limitMinVec ;
    std::vector<double> limitMaxVec ;

    if(m_E.size() == 0){
      ATH_MSG_INFO("amanda - no defined jet moments, return false");    
      auto conditions = conditionsFactoryFalse(0.0,1.0);
      std::sort(conditions.begin(), conditions.end(),ConditionsSorter());
      return conditions;
    }
    //fill vectors
    else{
       for(unsigned int count=0; count<m_jetVars.size(); count++){
           if(m_has[count].compare(match)==0){
             jetVarVec.push_back(m_jetVars[(count)% m_jetVars.size()]);
             ATH_MSG_INFO("amanda - getting limits for " << m_jetVars[count % m_jetVars.size()]);
             std::pair<double,double> limits = (*m_conversionMap.at(m_jetVars[count % m_jetVars.size()]))(m_limitMins[count], m_limitMaxs[count]);

             ATH_MSG_INFO("amanda - got limits " << limits);
             limitMinVec.push_back(limits.first);
             limitMaxVec.push_back(limits.second);
           }
           else{
             ATH_MSG_INFO("amanda - m_has=false, return true");
             jetVarVec.push_back(" "); // will return true in JetAttrsCondition and essentially be ignored
             limitMinVec.push_back(-10.0); //placeholder values
             limitMaxVec.push_back(-10.0);
           }
      }
    }

   auto conditions = conditionsFactoryJetAttrs(jetVarVec, limitMinVec, limitMaxVec);


   ATH_MSG_INFO("amanda - passing to conditionsFactory " << jetVarVec);

   //print inputs given to condition
   for(auto& c : conditions){ATH_MSG_INFO(c.toString());}

   return conditions;

}


 
std::shared_ptr<IJetGrouper> TrigHLTJetHypo_JetAttrs::getJetGrouper() const {
  return std::make_shared<SingleJetGrouper>();
}


bool TrigHLTJetHypo_JetAttrs::checkVals() const {

  if(m_limitMaxs.size() != m_limitMins.size()) {

    ATH_MSG_ERROR(name()
      << ": mismatch between number of limits (lower and upper): "
      << m_limitMins.size() << " "
      << m_limitMaxs.size() << " "
      );
    return false;
  }
  return true;

}

 
   
std::vector<std::shared_ptr<ICleaner>> TrigHLTJetHypo_JetAttrs::getCleaners() const {
  std::vector<std::shared_ptr<ICleaner>> v;
  return v;
} 


