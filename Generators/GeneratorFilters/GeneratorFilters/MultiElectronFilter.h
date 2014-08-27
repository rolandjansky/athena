/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_MULTIELECTRONFILTER_H
#define GENERATORFILTERS_MULTIELECTRONFILTER_H

#include "GeneratorModules/GenFilter.h"

class MultiElectronFilter : public GenFilter {
public:

  MultiElectronFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

private:

  double m_Ptmin;
  double m_EtaRange;
  int m_NElectrons;

};

#endif
