/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration 
*/
#ifndef GENERATORFILTERS_XAODDIRECTPHOTONFILTER_H
#define GENERATORFILTERS_XAODDIRECTPHOTONFILTER_H

#include "GeneratorModules/GenFilter.h"

#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"

#include "xAODTruth/TruthParticle.h"

#include <limits>

/// Filters and looks for photons from brem or hadr proc
/// @author L. Carminati, June 2010
class xAODDirectPhotonFilter : public GenFilter {
public:

  xAODDirectPhotonFilter(const std::string& name, ISvcLocator* pSvcLocator);
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
