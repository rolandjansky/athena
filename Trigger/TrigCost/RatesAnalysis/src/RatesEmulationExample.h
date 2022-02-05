/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RATESANALYSIS_RATESEMULATIONEXAMPLE_H
#define RATESANALYSIS_RATESEMULATIONEXAMPLE_H 1

#include "RatesAnalysis/RatesAnalysisAlg.h"

class RatesEmulationExample: public ::RatesAnalysisAlg { 
 public: 
  RatesEmulationExample( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~RatesEmulationExample(); 

  virtual StatusCode  ratesInitialize() override;
  virtual StatusCode  ratesExecute() override;
  virtual StatusCode  ratesFinalize() override;

 private:

  Gaudi::Property<float> m_lumi{this, "TargetLuminosity", 2e34, "Targer inst. luminosity, assuming full ring."};

}; 

#endif //> !RATESANALYSIS_RATESEMULATIONEXAMPLE_H
