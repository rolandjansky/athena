/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration 
*/
#ifndef GENERATORFILTERS_DIRECTPHOTONFILTER_H
#define GENERATORFILTERS_DIRECTPHOTONFILTER_H

#include "GeneratorModules/GenFilter.h"

/// Filters and looks for photons from brem or hadr proc
/// @author L. Carminati, June 2010
class DirectPhotonFilter : public GenFilter {
public:

  DirectPhotonFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

private:

  std::vector<double> m_Ptmin;
  std::vector<double> m_Ptmax;
  double m_EtaRange;
  size_t m_NPhotons;
  bool m_AllowSUSYDecay;
  bool m_OrderPhotons;

};

#endif
