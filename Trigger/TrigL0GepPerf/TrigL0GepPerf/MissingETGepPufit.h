/*
 *   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGL0GEPPERF_MISSINGETGEPPUFIT_H
#define TRIGL0GEPPERF_MISSINGETGEPPUFIT_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"

namespace PUfitVar{
  constexpr float maxEta = 4.8;
  constexpr float caloResSqrtTerm = 15.81;
  constexpr float caloResFloor = 50;
  constexpr float nSigma = 5.0;
  constexpr float constraintWeight = 1.;
  constexpr float trimFactor = 0.9;
  constexpr std::size_t nEtaBins = 14;
  constexpr std::size_t nPhiBins = 8;
  constexpr std::size_t nTowers = nEtaBins*nPhiBins;
}


class MissingETGepPufit: public ::AthAnalysisAlgorithm {
 public: 
  MissingETGepPufit( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~MissingETGepPufit();
  std::string m_inCluster;
  std::string m_outMET;
  virtual StatusCode  initialize();     //once, before any input is loaded
  virtual StatusCode  execute();        //per event
  virtual StatusCode PufitMET(std::string clusterContainerName, std::string metName, float inputSigma);
  virtual StatusCode  finalize();       //once, after all events processed

 private: 
}; 

#endif //> !TRIGL0GEPPERF_MISSINGETGEPPUFIT_H
