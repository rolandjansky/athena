/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration                                                      
*/
#ifndef GENERATORFILTERS_SAMEPARTICLEHARDSCATTERINGFILTER_H
#define GENERATORFILTERS_SAMEPARTICLEHARDSCATTERINGFILTER_H

#include "GeneratorModules/GenFilter.h"

/// Allows the user to search for any given production Parent1 + Parent 2 -> Child1

/// @author Krystsina Petukhova, Oct 2020
class SameParticleHardScatteringFilter : public GenFilter {
public:

  SameParticleHardScatteringFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

private:

	std::vector<int> m_PDGParent;
	std::vector<int> m_PDGChild;

};

#endif

