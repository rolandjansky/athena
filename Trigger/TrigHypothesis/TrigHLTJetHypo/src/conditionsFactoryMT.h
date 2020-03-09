/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHLTHYPO_CONDITIONSFACTORYMT_H
#define TRIGJETHLTHYPO_CONDITIONSFACTORYMT_H

// ********************************************************************
//
// NAME:     conditionsFactoryMT.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//


#include "./ConditionsDefsMT.h"

ConditionsMT conditionsFactoryAcceptAllMT(std::size_t capacity,
					  unsigned int conditionID=0);

ConditionsMT conditionsFactoryEtaEtMT(const std::vector<double>& etasMin,
                                      const std::vector<double>& etasMax,
                                      const std::vector<double>& thresholds,
                                      const std::vector<int>& asymmetricEtas);


ConditionsMT conditionsFactoryDijetMT(const std::vector<double>& massMins,
                                      const std::vector<double>& massMaxs,
                                      const std::vector<double>& detaMins,
                                      const std::vector<double>& detaMaxs,
                                      const std::vector<double>& dphiMins,
                                      const std::vector<double>& dphiMax);


ConditionsMT conditionsFactoryTLAMT(const std::vector<double>& etasMin,
                                    const std::vector<double>& etasMax,
                                    const std::vector<double>& ystarsMin,
                                    const std::vector<double>& ystarsMax,
                                    const std::vector<double>& massesMin,
                                    const std::vector<double>& massesMax);

ConditionsMT conditionsFactoryHTMT(double htMin,
				   double etmin,
				   double absetamin,
				   double absetamax
				   );



#endif
