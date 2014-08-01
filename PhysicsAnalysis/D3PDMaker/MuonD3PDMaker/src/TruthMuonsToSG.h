/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/TruthMuonsToSG.h
 * @author Srivas Prasad <srivas.prasad@cern.ch>
 * @date March 2010
 * @brief Create collection of true muons in StoreGate, based on analogous code in TauD3PDMaker
 */

#ifndef TRUTHMMUONSTOSG_H
#define TRUTHMMUONSTOSG_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "D3PDMakerUtils/SGKeyResolver.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>


namespace D3PD {


class TruthMuonsToSG: public AthAlgorithm {
 public:
  TruthMuonsToSG(const std::string& name, ISvcLocator* pSvcLocator);
  
  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

 private:
  std::string m_truthContainerKey;
  std::string m_truthMuonContainerKey;

  /// Helper to resolve SG key for input collection.
  SGKeyResolver m_resolver;
  ToolHandle<IMCTruthClassifier> m_classifier;
};


} // namespace D3PD


#endif // TRUTHMUONSTOSG_H
