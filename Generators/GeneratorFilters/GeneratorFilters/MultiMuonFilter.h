/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_MULTIMUONFILTER_H
#define GENERATORFILTERS_MULTIMUONFILTER_H

#include "GeneratorModules/GenFilter.h"

/// @author G. Stavropoulos, September 2006
class MultiMuonFilter : public GenFilter {
public:

  MultiMuonFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

private:

  double m_Ptmin;
  double m_EtaRange;
  int m_NMuons;

};

#endif
