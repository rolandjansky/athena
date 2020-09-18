// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  SGTools/StringPool.h
 * @author scott snyder
 * @date Mar 2007
 * @brief Maintain a mapping of strings to 64-bit ints.
 *
 * We map from strings to integer keys using a hash function.
 * We keep a table of hashed strings so that we can later return the string
 * given the key.
 * We allow registering additional key->string mappings as well, as long
 * as there aren't collisions.
 * We also keep an auxiliary int that gets hashed along with the string.
 *
 * A string can be marked as `transient' (using SG::transientKey).
 * If we encounter a hash collision while inserting a transient key,
 * we do not fail, but instead use find an alternate hash for it.
 */

#ifndef SGTOOLS_STRINGPOOL_H
#define SGTOOLS_STRINGPOOL_H

#include <stdint.h>
#include <string>
#include <memory>

namespace SG {

class StringPoolImpl;

class StringPool
{
public:
  /// Type of the integer keys.
  typedef uint32_t sgkey_t;

  /// Type of auxiliary data.
  typedef unsigned int sgaux_t;

  /// Number of bits we'll use in the keys.
  /// Leave a few spare bits in case we want to overload them for flags later.
  static const int sgkey_t_nbits = 30;
  static const sgkey_t sgkey_t_max =
    (static_cast<sgkey_t>(1) << sgkey_t_nbits) - 1;

  /// Constructor.
  StringPool();

  /// Destructor.
  ~StringPool();

  /// Copy/move constructors.
  StringPool (const StringPool& other);
  StringPool (StringPool&& other);

  /// Assignment/move operators.
  StringPool& operator= (const StringPool& other);
  StringPool& operator= (StringPool&& other);

  /**
   * @brief Find the key for a string.
   * @param str The string to look up.
   * @param aux Auxiliary data to include along with the string.
   * @return A key identifying the string.
   *         A given string will always return the same key.
   *         Will throw ExcSgkeyCollision in case of a hash collision
   *         (for a non-transient string)!
   */
  sgkey_t stringToKey (const std::string& str, sgaux_t aux = 0);

  /**
   * @brief Find the string corresponding to a given key.
   * @param key The key to look up.
   * @return Pointer to the string found, or null.
   *         We can find keys as long as the corresponding string
   *         was given to either @c stringToKey() or @c registerKey().
   */
  const std::string* keyToString (sgkey_t key) const;

  /**
   * @brief Find the string corresponding to a given key.
   * @param key The key to look up.
   * @param aux[out] Auxiliary data associated with the key.
   * @return Pointer to the string found, or null.
   *         We can find keys as long as the corresponding string
   *         was given to either @c stringToKey() or @c registerKey().
   */
  const std::string* keyToString (sgkey_t key, sgaux_t& aux) const;

  /**
   * @brief Remember an additional mapping from key to string.
   * @param key The key to enter.
   * @param str The string to enter.
   * @param aux Auxiliary data to include along with the string.
   * @return True if successful; false if the @c key already
   *         corresponds to a different string.
   *
   * This registers an additional mapping from a key to a string;
   * it can be found later through @c lookup() on the string.
   */
  bool registerKey (sgkey_t key,
                    const std::string& str,
                    sgaux_t aux = 0);


  /**
   * @brief Number of registered mappings.
   */
  size_t size() const;

  /**
   * @brief Debugging dump.  Write to cout.
   */
  void dump() const;


  /**
   * @brief Empty the pool.
   */
  void clear();

private:
  std::unique_ptr<StringPoolImpl> m_impl;
};


} // namespace SG


#endif // not SGTOOLS_STRINGPOOL_H
