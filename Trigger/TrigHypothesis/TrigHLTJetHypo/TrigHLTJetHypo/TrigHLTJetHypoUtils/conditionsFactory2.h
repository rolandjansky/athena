/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHLTHYPO_CONDITIONSFACTORY2_H
#define TRIGJETHLTHYPO_CONDITIONSFACTORY2_H

// ********************************************************************
//
// NAME:     conditionsFactory2.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//


#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

Conditions conditionsFactoryEtaEt(const std::vector<double>& etasMin,
                                  const std::vector<double>& etasMax,
                                  const std::vector<double>& thresholds,
                                  const std::vector<int>& asymmetricEtas);


Conditions conditionsFactorysinglemass(const std::vector<double>& etasMin,
                                  const std::vector<double>& etasMax,
                                  const std::vector<double>& thresholds,
                                  const std::vector<double>& jetMassMin,
                                  const std::vector<double>& jetMassMax);


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
                                  const std::vector<double>& dphiMaxs);


// for conditionsFactory2
Conditions conditionsFactoryDijetEtaMass(const std::vector<double>& etasMin,
                                         const std::vector<double>& etasMax,
                                         const std::vector<double>& etMins,
                                         const std::vector<double>& dEtaMin,
                                         const std::vector<double>& dEtaMax,
                                         const std::vector<double>& massesMin,
                                         const std::vector<double>& massesMax);

// for conditionsFactory
Conditions conditionsFactoryDijetEtaMass(double etasMin,
                                         double etasMax,
                                         double ystarsMin,
                                         double ystarsMax,
                                         double massesMin,
                                         double massesMax);


Conditions conditionsFactoryDijetDPhi(const std::vector<double>& dPhiMins,
                                      const std::vector<double>& dPhiMaxs);


Conditions conditionsFactoryTLA(const std::vector<double>& etasMin,
                                const std::vector<double>& etasMax,
                                const std::vector<double>& ystarsMin,
                                const std::vector<double>& ystarsMax,
                                const std::vector<double>& massesMin,
                                const std::vector<double>& massesMax);

Conditions conditionsFactoryHT(double htMin);

//added A. Steinhebel, April 2018
Conditions conditionsFactoryJetAttrsWidth(double limitMin,
                          double limitMax); 
Conditions conditionsFactoryJetAttrsKtDR(double limitMin,
                          double limitMax); 
Conditions conditionsFactoryFalse(double limitMin,
                          double limitMax); 

Conditions conditionsFactoryJetAttrs(const std::vector<std::string>& jetVars,
                      const std::vector<double>& limitMins,
                      const std::vector<double>& limitMaxs);

// Jona Bossio, February 2019
Conditions conditionsFactoryJVT(const int& nJets, const double& etThreshold, const double& workingPoint);

#endif
