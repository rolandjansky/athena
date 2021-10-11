/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_XAODPHOTONFILTER_H
#define GENERATORFILTERS_XAODPHOTONFILTER_H

#include "GeneratorModules/GenFilter.h"

/// Filters and looks for photons
/// @author I Hinchliffe, May 2004
/// @author A Buckley, May 2012
class xAODPhotonFilter : public GenFilter {
public:

  xAODPhotonFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

private:

  double m_Ptmin;
  double m_Ptmax;
  double m_EtaRange;
  int m_NPhotons;

};

#endif
