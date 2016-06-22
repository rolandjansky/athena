/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     conditionsFactory.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//


#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/EtaEtCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/DijetDEtaMassCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/conditionsFactory.h"


Conditions conditionsFactory(const std::vector<double>& etaMins,
			     const std::vector<double>& etaMaxs,
			     const std::vector<double>& thresholds){

  Conditions conditions;
  for (std::size_t i = 0; i != thresholds.size(); ++i){
    
    std::shared_ptr<ICondition> pCondition(new EtaEtCondition(etaMins[i],
							      etaMaxs[i], 
							      thresholds[i]));
    
      conditions.push_back(ConditionBridge(pCondition));
  }
  return conditions;
}


Conditions conditionsFactory(const std::vector<double>& etaMins,
			     const std::vector<double>& etaMaxs,
			     const std::vector<double>& ystarMins,
			     const std::vector<double>& ystarMaxs,
			     const std::vector<double>& massMins,
			     const std::vector<double>& massMaxs){
  
  Conditions conditions;
  for (std::size_t i = 0; i != etaMins.size(); ++i){
    
    std::shared_ptr<ICondition> 
      pCondition(new DijetDEtaMassCondition(etaMins[i],
					    etaMaxs[i], 
					    ystarMins[i],
					    ystarMaxs[i],
					    massMins[i],
					    massMaxs[i]));
    
    conditions.push_back(ConditionBridge(pCondition));
  }
  return conditions;
}



