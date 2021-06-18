/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGL0GEPPERF_GEPJETALG_H
#define TRIGL0GEPPERF_GEPJETALG_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigL0GepPerf/CustomTopoCluster.h"
#include "TrigL0GepPerf/CustomJet.h"

#include <string>


class GepJetAlg: public ::AthAnalysisAlgorithm {

 public:
  GepJetAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual StatusCode  execute() override; 

 private:

  std::string m_jetAlg;
  std::string m_topoclLabel;

};

#endif //> !TRIGL0GEPPERF_GEPJETALG_H
