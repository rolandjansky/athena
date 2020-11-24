/*
 *   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGL0GEPPERF_MISSINGETGEPCL_H
#define TRIGL0GEPPERF_MISSINGETGEPCL_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"

class MissingETGepCl: public ::AthAnalysisAlgorithm {
 public: 
  MissingETGepCl( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~MissingETGepCl();

  virtual StatusCode  initialize();     //once, before any input is loaded
  virtual StatusCode  execute();        //per event
  virtual StatusCode  ComputeClusterMET(std::string clusterContainerName, std::string metName );
  virtual StatusCode  finalize();       //once, after all events processed
  std::string m_inCluster; 
  std::string m_outMET; 
}; 

#endif //> !TRIGL0GEPPERF_MISSINGETGEP_H
