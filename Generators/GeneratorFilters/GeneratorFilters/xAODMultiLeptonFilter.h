/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_XAODMULTILEPTONFILTER_H
#define GENERATORFILTERS_XAODMULTILEPTONFILTER_H

#include "GeneratorModules/GenFilter.h"

class xAODMultiLeptonFilter : public GenFilter {
public:

  xAODMultiLeptonFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

private:

  double m_Ptmin;
  double m_EtaRange;
  int m_NLeptons;

};

#endif
