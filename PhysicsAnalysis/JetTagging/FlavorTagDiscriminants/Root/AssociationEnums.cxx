/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "FlavorTagDiscriminants/AssociationEnums.h"

#include <stdexcept>

namespace FlavorTagDiscriminants{

#define RETURN_CONFIG(cfg) \
  if (name == std::string(#cfg)) return TrackLinkType::cfg

  TrackLinkType trackLinkTypeFromString(const std::string& name) {
    RETURN_CONFIG(TRACK_PARTICLE);
    RETURN_CONFIG(IPARTICLE);
    throw std::logic_error("DL2 association scheme '" + name + "' unknown");
  }

#undef RETURN_CONFIG

}
