/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration 
*/
#ifndef GENERATORFILTERS_SPLITPHOTONFILTER_H
#define GENERATORFILTERS_SPLITPHOTONFILTER_H

#include "GeneratorModules/GenFilter.h"

class SplitPhotonFilter : public GenFilter {
public:

  SplitPhotonFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

private:

  double m_Ptmin;
  double m_EtaRange;
  int m_NPhotons;
  std::vector<int> m_dauPdg;

};

#endif
