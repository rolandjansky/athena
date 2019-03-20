/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     conditionsFactory2.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//


#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/EtaEtCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SingleJetMassCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/EtaEtAsymmetricCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/DijetDEtaMassCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/DijetDPhiCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/DijetCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HTCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/TLACondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/JetAttrsCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/JetAttrsWidthCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/JetAttrsKtDRCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/JVTCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FalseCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/conditionsFactory2.h"

Conditions conditionsFactoryEtaEt(const std::vector<double>& etaMins,
                                  const std::vector<double>& etaMaxs,
                                  const std::vector<double>& thresholds,
                                  const std::vector<int>& asymmetricEtas){

  Conditions conditions;
  for (std::size_t i = 0; i != thresholds.size(); ++i){
    
    std::shared_ptr<ICondition> pCondition(nullptr);

    if (asymmetricEtas[i] != 0){
      pCondition.reset(new EtaEtAsymmetricCondition(etaMins[i], etaMaxs[i], thresholds[i]));
    } else {
      pCondition.reset(new EtaEtCondition(etaMins[i], etaMaxs[i], thresholds[i]));
    }

    conditions.push_back(ConditionBridge(pCondition));      
  }
  return conditions;
}


Conditions conditionsFactorysinglemass(const std::vector<double>& etaMins,
                                  const std::vector<double>& etaMaxs,
                                  const std::vector<double>& thresholds,
                                  const std::vector<double>& jetMassMin,
                                  const std::vector<double>& jetMassMax){

  Conditions conditions;
  for (std::size_t i = 0; i != thresholds.size(); ++i){
    
    std::shared_ptr<ICondition> pCondition(nullptr);

//    if (asymmetricEtas[i] != 0){
//      pCondition.reset(new EtaEtAsymmetricCondition(etaMins[i], etaMaxs[i], thresholds[i]));
//    } else {
      pCondition.reset(new SingleJetMassCondition(etaMins[i], etaMaxs[i], thresholds[i], jetMassMin[i], jetMassMax[i]));
//    }

    conditions.push_back(ConditionBridge(pCondition));      
  }
  return conditions;
}


Conditions conditionsFactoryDijet(const std::vector<double>& etThresholds1,
                                  const std::vector<double>& etThresholds2,
                                  const std::vector<double>& etaMins1,
                                  const std::vector<double>& etaMaxs1,
                                  const std::vector<double>& etaMins2,
                                  const std::vector<double>& etaMaxs2,
                                  const std::vector<double>& massMins,
                                  const std::vector<double>& massMaxs,
                                  const std::vector<double>& detaMins,
                                  const std::vector<double>& detaMaxs,
                                  const std::vector<double>& dphiMins,
                                  const std::vector<double>& dphiMaxs){
  
  Conditions conditions;

  
  for(std::size_t i = 0; i < etThresholds1.size(); ++i){
    std::shared_ptr<ICondition>
      pCondition(new DijetCondition(etThresholds1[i],
                                    etThresholds2[i],
                                    etaMins1[i],
                                    etaMaxs1[i],
                                    etaMins2[i],
                                    etaMaxs2[i],
                                    massMins[i],
                                    massMaxs[i],
                                    detaMins[i],
                                    detaMaxs[i],
                                    dphiMins[i],
                                    dphiMaxs[i]));
    
    conditions.push_back(ConditionBridge(pCondition));
  }
  return conditions;
}

Conditions conditionsFactoryDijetEtaMass(const std::vector<double>& etaMins,
                                         const std::vector<double>& etaMaxs,
                                         const std::vector<double>& etMins,
                                         const std::vector<double>& dEtaMins,
                                         const std::vector<double>& dEtaMaxs,
                                         const std::vector<double>& massMins,
                                         const std::vector<double>& massMaxs){
  
  Conditions conditions;
  std::shared_ptr<ICondition> 
    pCondition(new DijetDEtaMassCondition(etaMins,
                                          etaMaxs, 
                                          etMins,
                                          dEtaMins,
                                          dEtaMaxs,
                                          massMins,
                                          massMaxs));
  
  conditions.push_back(ConditionBridge(pCondition));
  return conditions;
}


Conditions conditionsFactoryDijetDPhi(const std::vector<double>& dPhiMins,
                                      const std::vector<double>& dPhiMaxs){
  
  Conditions conditions;

  std::shared_ptr<ICondition> pCondition(new DijetDPhiCondition(dPhiMins, 
                                                                dPhiMaxs));
  conditions.push_back(ConditionBridge(pCondition));

  return conditions;
}


Conditions conditionsFactoryTLA(const std::vector<double>& etaMins,
                                const std::vector<double>& etaMaxs,
                                const std::vector<double>& ystarMins,
                                const std::vector<double>& ystarMaxs,
                                const std::vector<double>& massMins,
                                const std::vector<double>& massMaxs){

  Conditions conditions;
  std::shared_ptr<ICondition> 
    pCondition(new TLACondition(etaMins,
                                etaMaxs, 
                                ystarMins,
                                ystarMaxs,
                                massMins,
                                massMaxs));
  
  conditions.push_back(ConditionBridge(pCondition));
  return conditions;
}



Conditions conditionsFactoryHT(double htMin){
  
  Conditions conditions;
  
  std::shared_ptr<ICondition> pCondition(new HTCondition(htMin));
  
  conditions.push_back(ConditionBridge(pCondition));
  return conditions;
}


//added A. Steinhebel, April 2018
Conditions conditionsFactoryJetAttrsWidth(double limitMin,
                      double limitMax){
  Conditions conditions;

  std::shared_ptr<ICondition> 
    pCondition(new JetAttrsWidthCondition(limitMin,
                      limitMax));

  conditions.push_back(ConditionBridge(pCondition));
  return conditions;
}

Conditions conditionsFactoryJetAttrsKtDR(double limitMin,
                        double limitMax){
  Conditions conditions;

  std::shared_ptr<ICondition> 
    pCondition(new JetAttrsKtDRCondition(limitMin,
                      limitMax));

  conditions.push_back(ConditionBridge(pCondition));
  return conditions;
}

Conditions conditionsFactoryJetAttrs(const std::vector<std::string>& jetVars,
                      const std::vector<double>& limitMins,
                      const std::vector<double>& limitMaxs){
  Conditions conditions;

  /*
  for(std::size_t i = 0; i < limitMins.size(); ++i){
      std::shared_ptr<ICondition>
      pCondition(new JetAttrsCondition(jetVars[i],
                                    Es[i],
                                    limitMins[i],
                                    limitMaxs[i]));

    conditions.push_back(ConditionBridge(pCondition));
  }
  */
  std::shared_ptr<ICondition> 
    pCondition(new JetAttrsCondition(jetVars,
                    limitMins,
                    limitMaxs));

  conditions.push_back(ConditionBridge(pCondition));

  return conditions;
}

Conditions conditionsFactoryFalse(double limitMin,
                  double limitMax){
  Conditions conditions;

  std::shared_ptr<ICondition> 
    pCondition(new FalseCondition(limitMin,
                  limitMax));

  conditions.push_back(ConditionBridge(pCondition));
  return conditions;
}

// Jona Bossio, February 2019
Conditions conditionsFactoryJVT(const int& nJets, const double& etThreshold, const double& workingPoint){
  Conditions conditions;
  for(int i = 0; i < nJets; ++i){
    std::shared_ptr<ICondition> pCondition(new JVTCondition(etThreshold, workingPoint));
    conditions.push_back(ConditionBridge(pCondition));
  }
  return conditions;
}
