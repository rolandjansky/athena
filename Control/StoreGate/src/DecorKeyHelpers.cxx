/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file StoreGate/src/DecorKeyHelpers.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2017
 * @brief Some common helper functions used by decoration handles.
 */


#include "StoreGate/tools/DecorKeyHelpers.h"


namespace SG {


/**
 * @brief Extract the container part of key.
 * @param key The decoration handle key.
 *
 * Given a key of the form CCC.DDD, returns the container part, CCC.
 */
std::string contKeyFromKey (const std::string& key)
{
  std::string::size_type ipos = key.find ('.');
  if (ipos == std::string::npos)
    return key;
  return key.substr (0, ipos);
}


/**
 * @brief Extract the decoration part of key.
 * @param key The decoration handle key.
 *
 * Given a key of the form CCC.DDD, returns the decoration part, DDD.
 */
std::string decorKeyFromKey (const std::string& key)
{
  std::string::size_type ipos = key.find ('.');
  if (ipos == std::string::npos)
    return "";
  return key.substr (ipos+1, std::string::npos);
}


} // namespace SG
