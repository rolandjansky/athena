/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef FLIP_TAG_ENUMS_HH
#define FLIP_TAG_ENUMS_HH

#include <string>

namespace FlavorTagDiscriminants {
  // note that all the "non-standard" ones here use the default flip
  // config for SV1, JF, and IPxD. The variants are just for the RNN.
  enum class FlipTagConfig {
    STANDARD,                   // use all tracks
    NEGATIVE_IP_ONLY,           // use only negative IP, flip sign
    FLIP_SIGN,                  // just flip the sign, use all tracks
  };
  FlipTagConfig flipTagConfigFromString(const std::string&);
}

#endif
