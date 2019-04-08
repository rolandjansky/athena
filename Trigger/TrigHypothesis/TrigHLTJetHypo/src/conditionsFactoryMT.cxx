/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     conditionsFactoryMT.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//


#include "./EtaEtConditionMT.h"
#include "./EtaEtAsymmetricConditionMT.h"
#include "./DijetConditionMT.h"
#include "./HTConditionMT.h"
#include "./TLAConditionMT.h"

#include "./conditionsFactoryMT.h"

ConditionsMT conditionsFactoryEtaEtMT(const std::vector<double>& etaMins,
                                    const std::vector<double>& etaMaxs,
                                    const std::vector<double>& thresholds,
                                    const std::vector<int>& asymmetricEtas){

  ConditionsMT conditions;
  for (std::size_t i = 0; i != thresholds.size(); ++i){
    
    std::shared_ptr<IConditionMT> pCondition(nullptr);

    if (asymmetricEtas[i] != 0){
      pCondition.reset(new EtaEtAsymmetricConditionMT(etaMins[i],
                                                      etaMaxs[i],
                                                      thresholds[i]));
    } else {
      pCondition.reset(new EtaEtConditionMT(etaMins[i],
                                            etaMaxs[i],thresholds[i]));
    }

    conditions.push_back(ConditionBridgeMT(pCondition));      
  }
  return conditions;
}



ConditionsMT conditionsFactoryDijetMT(const std::vector<double>& massMins,
                                    const std::vector<double>& massMaxs,
                                    const std::vector<double>& detaMins,
                                    const std::vector<double>& detaMaxs,
                                    const std::vector<double>& dphiMins,
                                    const std::vector<double>& dphiMaxs){  
  ConditionsMT conditions;
  
  for(std::size_t i = 0; i < massMins.size(); ++i){
    std::shared_ptr<IConditionMT>
      pCondition(new DijetConditionMT(massMins[i],
                                      massMaxs[i],
                                      detaMins[i],
                                      detaMaxs[i],
                                      dphiMins[i],
                                      dphiMaxs[i]));
    
    conditions.push_back(ConditionBridgeMT(pCondition));
  }
  return conditions;
}


ConditionsMT conditionsFactoryTLAMT(const std::vector<double>& etaMins,
                                    const std::vector<double>& etaMaxs,
                                    const std::vector<double>& ystarMins,
                                    const std::vector<double>& ystarMaxs,
                                    const std::vector<double>& massMins,
                                    const std::vector<double>& massMaxs){

  ConditionsMT conditions;
  std::shared_ptr<IConditionMT> 
    pCondition(new TLAConditionMT(etaMins,
                                  etaMaxs, 
                                  ystarMins,
                                  ystarMaxs,
                                  massMins,
                                  massMaxs));
  
  conditions.push_back(ConditionBridgeMT(pCondition));
  return conditions;
}



ConditionsMT conditionsFactoryHTMT(double htMin){
  
  ConditionsMT conditions;
  
  std::shared_ptr<IConditionMT> pCondition(new HTConditionMT(htMin));
  
  conditions.push_back(ConditionBridgeMT(pCondition));
  return conditions;
}



