/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_MULTILEPTONFILTER_H
#define GENERATORFILTERS_MULTILEPTONFILTER_H

#include "GeneratorModules/GenFilter.h"

class MultiLeptonFilter : public GenFilter {
public:

  MultiLeptonFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

private:

  double m_Ptmin;
  double m_EtaRange;
  int m_NLeptons;

};

#endif
