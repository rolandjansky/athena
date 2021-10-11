/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_XAODMULTIMUONFILTER_H
#define GENERATORFILTERS_XAODMULTIMUONFILTER_H

#include "GeneratorModules/GenFilter.h"

/// Filters and looks for muons
/// @author I Hinchliffe,  December 2001
/// @author G. Watts, Sep 2006
class xAODMultiMuonFilter : public GenFilter {
public:

  xAODMultiMuonFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

private:

	double m_Ptmin;
	double m_EtaRange;
    int m_NMuons;

};

#endif