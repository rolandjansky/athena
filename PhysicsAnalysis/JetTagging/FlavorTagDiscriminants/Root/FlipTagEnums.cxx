/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "FlavorTagDiscriminants/FlipTagEnums.h"

#include <stdexcept>

namespace FlavorTagDiscriminants{

#define RETURN_CONFIG(cfg) \
  if (name == std::string(#cfg)) return FlipTagConfig::cfg

  FlipTagConfig flipTagConfigFromString(const std::string& name) {
    RETURN_CONFIG(STANDARD);
    RETURN_CONFIG(NEGATIVE_IP_ONLY);
    RETURN_CONFIG(FLIP_SIGN);
    throw std::logic_error("b-tagging flip config '" + name + "' unknown");
  }

#undef RETURN_CONFIG

}
