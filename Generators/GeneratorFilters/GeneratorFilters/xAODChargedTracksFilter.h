/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_XAODCHARGEDTRACKSFILTER_H
#define GENERATORFILTERS_XAODCHARGEDTRACKSFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"

/// Filter events based on presence of charged tracks
class xAODChargedTracksFilter : public GenFilter {
public:

  xAODChargedTracksFilter(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode filterEvent();

private:

  // Minimum pT for a track to count
  double m_Ptmin;

  // Maximum |pseudorapidity| for a track to count
  double m_EtaRange;

  // Minimum number of tracks
  double m_NTracks;

};

#endif
