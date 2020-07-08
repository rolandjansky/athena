/*
 *   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGL0GEPPERF_CLUSTERTIMING_H
#define TRIGL0GEPPERF_CLUSTERTIMING_H 1
#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"  

class ClusterTiming: public ::AthAnalysisAlgorithm { 
 public: 
  ClusterTiming( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~ClusterTiming(); 

  ///uncomment and implement methods as required

  std::string m_outputClusterName; 
  std::string m_inputClusterName;
  float m_lambdaCalDivide; 
  float m_qualityCut; 
  float m_timeCutLargeQ; 
  float m_timeCutSmallQ; 
  float m_etaCut; 
                                        //IS EXECUTED:
  virtual StatusCode  initialize();     //once, before any input is loaded
  virtual StatusCode  execute();        //per event
  virtual StatusCode  finalize();       //once, after all events processed
 private: 


}; 

#endif //> !TRIGL0GEPPERF_CLUSTERTIMING_H
