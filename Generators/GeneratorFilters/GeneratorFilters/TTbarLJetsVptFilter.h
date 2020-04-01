/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_TTBARLJETSVPTFILTER_H
#define GENERATORFILTERS_TTBARLJETSVPTFILTER_H

#include "GeneratorModules/GenFilter.h"

/// Require the event to contain at least one charged lepton (from W decay, which should come from top) with pt at or above Ptcut.
///
/// Events that do not contain t AND t CLHEP::bar quarks are rejected.
/// Only tops decaying to W X are analyzed and counted in this algorithm.
/// @author Gia Khoriauli, June 2008
/// @author Andy Buckley, extension to specific lepton multiplicities, April 2012
class TTbarLJetsVptFilter : public GenFilter {
public:

  TTbarLJetsVptFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

private:

  double m_Ptmin;
  double m_Etamax;
  int m_numLeptons;
  bool m_followTaus;

  // FLS
  double m_VptMin;
  double m_VptMax;
  // ---

};

#endif
