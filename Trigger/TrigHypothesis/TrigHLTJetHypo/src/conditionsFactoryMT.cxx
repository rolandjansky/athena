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


#include "./AcceptAllConditionMT.h"
#include "./EtaEtConditionMT.h"
#include "./EtaEtAsymmetricConditionMT.h"
#include "./DijetConditionMT.h"
#include "./HTConditionMT.h"
#include "./TLAConditionMT.h"

#include "./conditionsFactoryMT.h"

ConditionsMT conditionsFactoryAcceptAllMT(std::size_t capacity){

  ConditionsMT conditions;
  conditions.push_back
    ( std::move(std::make_unique<AcceptAllConditionMT>(capacity)));
  return conditions;
}

ConditionsMT conditionsFactoryEtaEtMT(const std::vector<double>& etaMins,
                                    const std::vector<double>& etaMaxs,
                                    const std::vector<double>& thresholds,
                                    const std::vector<int>& asymmetricEtas){

  ConditionsMT conditions;
  for (std::size_t i = 0; i != thresholds.size(); ++i){
    
    if (asymmetricEtas[i] != 0){
      conditions.push_back
	(std::move(
		   (std::make_unique<EtaEtAsymmetricConditionMT>(etaMins[i],
								 etaMaxs[i],
								 thresholds[i]))));
      
    } else {
      conditions.push_back
	(std::move(
		   (std::make_unique<EtaEtConditionMT>(etaMins[i],
						       etaMaxs[i],thresholds[i]))));
    }
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
    conditions.push_back
      (std::move(
		 std::make_unique<DijetConditionMT>(massMins[i],
						    massMaxs[i],
						    detaMins[i],
						    detaMaxs[i],
						    dphiMins[i],
						    dphiMaxs[i])));
    
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
  conditions.push_back
    (std::move(
	       std::make_unique<TLAConditionMT>(etaMins,
						etaMaxs, 
						ystarMins,
						ystarMaxs,
						massMins,
						massMaxs)));
  
  return conditions;
}



ConditionsMT conditionsFactoryHTMT(double htMin){
  
  ConditionsMT conditions;
  conditions.push_back
    (std::move(
	       std::make_unique<HTConditionMT>(htMin)));
  return conditions;
  
}



