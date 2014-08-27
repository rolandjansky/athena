/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_TTBARMASSFILTER_H
#define GENERATORFILTERS_TTBARMASSFILTER_H

#include "GeneratorModules/GenFilter.h"

/// Require the invariant mass of a top + anti-top pair to be within a certain range
///
/// Events that do not contain top AND anti-top 1 or 2 pairs are rejected.
/// @author  Gia Khoriauli, October 2010
class TTbarMassFilter : public GenFilter {
public:

  TTbarMassFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

private:

  double m_massRangeLowThr;
  double m_massRangeHighThr;

};

#endif
