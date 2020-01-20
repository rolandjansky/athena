/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RATESANALYSIS_FULLMENU_H
#define RATESANALYSIS_FULLMENU_H 1

#include "RatesAnalysis/RatesAnalysisAlg.h"

class FullMenu: public ::RatesAnalysisAlg { 
 public: 
  FullMenu( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~FullMenu(); 

  virtual StatusCode  ratesInitialize() override;
  virtual StatusCode  ratesExecute() override;
  virtual StatusCode  ratesFinalize() override;

 private:

  Gaudi::Property<float> m_lumi{this, "TargetLuminosity", 2e34, "Targer inst. luminosity, assuming full ring."};

}; 

#endif //> !RATESANALYSIS_FULLMENU_H
