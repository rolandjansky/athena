/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration                                                      
*/
#ifndef GENERATORFILTERS_XAODSAMEPARTICLEHARDSCATTERINGFILTER_H
#define GENERATORFILTERS_XAODSAMEPARTICLEHARDSCATTERINGFILTER_H

#include "GeneratorModules/GenFilter.h"

#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticle.h"

/// Allows the user to search for any given production Parent1 + Parent 2 -> Child1

/// @author Krystsina Petukhova, Oct 2020
class xAODSameParticleHardScatteringFilter : public GenFilter {
public:

  xAODSameParticleHardScatteringFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

private:

	std::vector<int> m_PDGParent;
	std::vector<int> m_PDGChild;

};

#endif

