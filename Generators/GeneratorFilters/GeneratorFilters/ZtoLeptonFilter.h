/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_ZTOLEPTONFILTER_H
#define GENERATORFILTERS_ZTOLEPTONFILTER_H

#include "GeneratorModules/GenFilter.h"

/// Filter and look for Z's decaying to leptons
/// @author I Hinchliffe, Jan 2001
class ZtoLeptonFilter : public GenFilter {
public:

  ZtoLeptonFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

private:

  double m_Ptmin;
  double m_EtaRange;

};

#endif
