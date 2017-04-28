/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ELECTRONCHARGECORRECTION_ELECHARGEALG_H
#define ELECTRONCHARGECORRECTION_ELECHARGEALG_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
// #include "xAODTruth/TruthParticle.h"

#include "TTree.h"
#include "TLorentzVector.h"

#include <vector>

// #include "MCTruthClassifier/IMCTruthClassifier.h"
#include "PATInterfaces/SystematicSet.h"

namespace CP{
  // class IElectronChargeEfficiencyCorrectionTool;
  class IEfficiencyScaleFactorTool;
}

class EleChargeAlg: public ::AthAlgorithm {
 public:
  EleChargeAlg( const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~EleChargeAlg();

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();


 private:
  /// The systematics set that will contain all the systematic variations
  CP::SystematicSet m_syst;

  /// The handle to the charge-misid scale-factor tool
  ToolHandle<CP::IEfficiencyScaleFactorTool> m_eccTool;

  /// The name of the input electron container
  StringProperty m_eleContName;
};

#endif //> !ELECTRONCHARGECORRECTION_ELECHARGEALG_H
