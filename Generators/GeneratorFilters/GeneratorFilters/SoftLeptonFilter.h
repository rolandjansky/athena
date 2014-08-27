/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_SOFTLEPTONFILTER_H
#define GENERATORFILTERS_SOFTLEPTONFILTER_H

#include "GeneratorModules/GenFilter.h"


/// Filters and looks for leptons from hadron decays.
/// @author Henri Bachacou, Feb 2006
class SoftLeptonFilter : public GenFilter {
public:

  SoftLeptonFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

private:

  double m_Ptmin;
  double m_EtaRange;
  int    m_LeptonType;

};

#endif
