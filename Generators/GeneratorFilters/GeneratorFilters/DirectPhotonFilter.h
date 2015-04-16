/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_DIRECTPHOTONFILTER_H
#define GENERATORFILTERS_DIRECTPHOTONFILTER_H

#include "GeneratorModules/GenFilter.h"

/// Filters and looks for photons from brem or hadr proc
/// @author L. Carminati, June 2010
class DirectPhotonFilter : public GenFilter {
public:

  DirectPhotonFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

private:

  double m_Ptmin;
  double m_Ptmax;
  double m_EtaRange;
  int m_NPhotons;
  bool m_AllowSUSYDecay;

};

#endif
