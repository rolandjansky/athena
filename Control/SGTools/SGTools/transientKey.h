// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file SGTools/tranientKey.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2020
 * @brief Define naming convention for transient SG keys.
 *
 * StoreGate hashes a string key + CLID to a 30-bit hashed key (sgkey).
 * Previously, we didn't allow for hash collisions here.
 * However, trigger views can result in adding many (100's at least)
 * more objects to SG, thus making hash collisions more likely.
 *
 * To ameliorate this, we allow tagging a SG string key as `transient'.
 * If there is a collision while adding a transient key, then we choose
 * a new, unused hash rather than failing.  On the other hand, it is not
 * permitted to write out a key tagged as transient.
 *
 * Currently, a SG key starting with an underscore is considered transient.
 * But the functions here should be used rather than assuming this,
 * in case this needs to change.
 */


#include <string>


#ifndef SGTOOLS_TRANSIENTKEY_H
#define SGTOOLS_TRANSIENTKEY_H


namespace SG {


/**
 * @brief Test to see if a key is transoent.
 * @param key The key to test.
 */
bool isTransientKey (const std::string& key)
{
  return !key.empty() && key[0] == '_';
}


/**
 * @brief Make a key transient.
 * @param key The key to transform;
 */
std::string transientKey (const std::string& key)
{
  if (isTransientKey (key)) {
    return key;
  }
  return "_" + key;
}



} // namespace SG


#endif // not SGTOOLS_TRANSIENTKEY_H
