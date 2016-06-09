// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IStringPool.h,v 1.3 2008-09-03 17:19:10 ssnyder Exp $
/**
 * @file AthenaKernel/IStringPool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2007
 * @brief Abstract interface for looking up strings/CLIDs in a pool.
 */


#ifndef ATHENAKERNEL_ISTRINGPOOL_H
#define ATHENAKERNEL_ISTRINGPOOL_H


#include "AthenaKernel/sgkey_t.h"
#include "GaudiKernel/ClassID.h"
#include <string>


/**
 * @brief Abstract interface for looking up strings/CLIDs in a pool.
 */
class IStringPool
{
public:
  /// Destructor.
  virtual ~IStringPool() {}

  /// Type of the keys.
  typedef SG::sgkey_t sgkey_t;


  /**
   * @brief Find the key for a string/CLID pair.
   * @param str The string to look up.
   * @param clid The CLID associated with the string.
   * @return A key identifying the string.
   *         A given string will always return the same key.
   *         Will abort in case of a hash collision!
   */
  virtual
  sgkey_t stringToKey (const std::string& str, CLID clid) = 0;

  /**
   * @brief Find the string corresponding to a given key.
   * @param key The key to look up.
   * @return Pointer to the string found, or null.
   *         We can find keys as long as the corresponding string
   *         was given to either @c stringToKey() or @c registerKey().
   */
  virtual
  const std::string* keyToString (sgkey_t key) const = 0;

  /**
   * @brief Find the string and CLID corresponding to a given key.
   * @param key The key to look up.
   * @param clid[out] The found CLID.
   * @return Pointer to the string found, or null.
   *         We can find keys as long as the corresponding string
   *         was given to either @c stringToKey() or @c registerKey().
   */
  virtual
  const std::string* keyToString (sgkey_t key,
                                  CLID& clid) const = 0;

  /**
   * @brief Remember an additional mapping from key to string/CLID.
   * @param key The key to enter.
   * @param str The string to enter.
   * @param clid The CLID associated with the string.
   * @return True if successful; false if the @c key already
   *         corresponds to a different string.
   *
   * This registers an additional mapping from a key to a string;
   * it can be found later through @c lookup() on the string.
   * Logs an error if @c key already corresponds to a different string.
   */
  virtual
  void registerKey (sgkey_t key,
                    const std::string& str,
                    CLID clid) = 0;
};


#endif // not ATHENAKERNEL_ISTRINGPOOL_H
