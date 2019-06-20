/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef FLIP_TAG_ENUMS_HH
#define FLIP_TAG_ENUMS_HH

#include <string>

namespace FlavorTagDiscriminants {
  enum class FlipTagConfig {
    STANDARD,                   // use all tracks
    NEGATIVE_IP_ONLY,           // use only negative IP, flip sign
  };
  FlipTagConfig flipTagConfigFromString(const std::string&);
}

#endif
