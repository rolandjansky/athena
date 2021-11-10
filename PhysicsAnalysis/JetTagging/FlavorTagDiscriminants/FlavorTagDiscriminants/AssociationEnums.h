/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ASSOCIATION_ENUMS_HH
#define ASSOCIATION_ENUMS_HH

#include <string>

namespace FlavorTagDiscriminants {
  enum class TrackLinkType {
    TRACK_PARTICLE,
    IPARTICLE
  };
  TrackLinkType trackLinkTypeFromString(const std::string&);
}

#endif
