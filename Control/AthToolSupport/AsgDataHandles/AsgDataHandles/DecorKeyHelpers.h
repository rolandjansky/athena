/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */

#ifndef ASG_DATA_HANDLES_DECOR_KEY_HELPERS_H
#define ASG_DATA_HANDLES_DECOR_KEY_HELPERS_H

#ifndef XAOD_STANDALONE
#include <StoreGate/DecorKeyHelpers.h>
#else

#include <stdexcept>
#include <string>

namespace SG {

inline std::string contKeyFromKey (const std::string& key)
{
  const auto split = key.rfind ('.');
  if (split == std::string::npos)
    throw std::runtime_error ("decor key does not contain a .: " + key);
  return key.substr (0, split);
}

inline std::string decorKeyFromKey (const std::string& key)
{
  const auto split = key.rfind ('.');
  if (split == std::string::npos)
    throw std::runtime_error ("decor key does not contain a .: " + key);
  return key.substr (split + 1);
}
}

#endif

#endif
