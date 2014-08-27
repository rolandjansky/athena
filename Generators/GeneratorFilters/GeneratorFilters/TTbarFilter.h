/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_TTBARFILTER_H
#define GENERATORFILTERS_TTBARFILTER_H

#include "GeneratorModules/GenFilter.h"

/// Require the event to contain at least one t (or tbar) quark with pt at or above Ptcut.
///
/// Events that do not contain t AND t CLHEP::bar quarks are rejected.
/// Only tops decaying to W X are analyzed and counted in this algorithm.
/// @author V. Savinov, Dec 2005
class TTbarFilter : public GenFilter {
public:

  TTbarFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

private:

  double m_Ptmin;

};

#endif
