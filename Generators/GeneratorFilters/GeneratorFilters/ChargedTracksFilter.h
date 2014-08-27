/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_CHARGEDTRACKSFILTER_H
#define GENERATORFILTERS_CHARGEDTRACKSFILTER_H

#include "GeneratorModules/GenFilter.h"

/// Filter events based on presence of charged tracks
class ChargedTracksFilter : public GenFilter {
public:

  ChargedTracksFilter(const std::string& name, ISvcLocator* pSvcLocator);
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
