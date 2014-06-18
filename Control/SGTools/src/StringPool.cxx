/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: StringPool.cxx,v 1.3 2007-12-10 22:20:03 ssnyder Exp $
/**
 * @file  SGTools/StringPool.cxx
 * @author scott snyder
 * @date Mar 2007
 * @brief Maintain a mapping of strings to 64-bit ints.
 */


#include "SGTools/StringPool.h"
#include "SGTools/crc64.h"
#include "CxxUtils/unordered_map.h"
#include <map>
#include <cstdlib>
#include <iostream>
#include <iomanip>


namespace SG {


//***************************************************************************
// Implementation class.
//


class StringPoolImpl
{
public:
  /// Remember an additional mapping from key to string.
  bool registerKey (StringPool::sgkey_t key,
                    const std::string& str,
                    StringPool::sgaux_t aux);

  /// Find the string corresponding to a given key.
  const std::string* keyToString (StringPool::sgkey_t key,
                                  StringPool::sgaux_t& aux) const;

  /// Debugging dump.  Write to stdout.
  void dump() const;

private:
  // Hash function for the key.
  // Just cast the low bits to a size_t.
  struct keyhash
  {
    std::size_t operator() (StringPool::sgkey_t key) const
    { return static_cast<std::size_t> (key); }
  };

  // The key hash table.
  typedef std::pair<StringPool::sgaux_t, std::string> pair_t;
  typedef SG::unordered_map<StringPool::sgkey_t, pair_t, keyhash> keymap_t;
  keymap_t m_keymap;
};


/**
 * @brief Remember an additional mapping from key to string.
 * @param key The key to enter.
 * @param str The string to enter.
 * @param aux Auxiliary data to include along with the string.
 * @return True if successful; false if the @c key already
 *         corresponds to a different string.
 */
bool StringPoolImpl::registerKey (StringPool::sgkey_t key,
                                  const std::string& str,
                                  StringPool::sgaux_t aux)
{
  StringPoolImpl::keymap_t::iterator i = m_keymap.find (key);
  if (i == m_keymap.end()) {
    pair_t& p = m_keymap[key];
    p.first = aux;
    p.second = str;
  }
  else if (i->second.first != aux || i->second.second != str)
    return false;
  return true;
}


/**
 * @brief Find the string corresponding to a given key.
 * @param key The key to look up.
 * @param aux[out] Auxiliary data associated with the key.
 * @return Pointer to the string found, or null.
 *         We can find keys as long as the corresponding string
 *         was given to either @c stringToKey() or @c registerKey().
 */
const std::string*
StringPoolImpl::keyToString (StringPool::sgkey_t key,
                             StringPool::sgaux_t& aux) const
{
  StringPoolImpl::keymap_t::const_iterator i = m_keymap.find (key);
  if (i != m_keymap.end()) {
    aux = i->second.first;
    return &i->second.second;
  }
  return 0;
}


/**
 * @brief Debugging dump.  Write to cout.
 */
void StringPoolImpl::dump() const
{
  for (keymap_t::const_iterator i = m_keymap.begin();
       i != m_keymap.end();
       ++i)
  {
    std::cout << std::hex << std::setw(18) << i->first << " "
              << std::dec << std::setw(9) << i->second.first << " "
              << i->second.second << "\n";
  }
}


//***************************************************************************
// StringPool class.
//

/**
 * @brief Constructor.
 */
StringPool::StringPool()
  : m_impl (new StringPoolImpl)
{
}


/**
 * @brief Destructor.
 */
StringPool::~StringPool()
{
  delete m_impl;
}


/**
 * @brief Find the key for a string.
 * @param str The string to look up.
 * @param aux Auxiliary data to include along with the string.
 * @return A key identifying the string.
 *         A given string will always return the same key.
 *         Will abort in case of a hash collision!
 */
StringPool::sgkey_t StringPool::stringToKey (const std::string& str,
                                             sgaux_t aux /*= 0*/)
{
  uint64_t crc = crc64 (str);
  if (aux) crc = crc64addint (crc, aux);
  sgkey_t key = (crc & sgkey_t_max);
  if (!m_impl->registerKey (key, str, aux))
    std::abort();
  return key;
}


/**
 * @brief Find the string corresponding to a given key.
 * @param key The key to look up.
 * @return Pointer to the string found, or null.
 *         We can find keys as long as the corresponding string
 *         was given to either @c stringToKey() or @c registerKey().
 */
const std::string* StringPool::keyToString (sgkey_t key) const
{
  sgaux_t aux;
  return m_impl->keyToString (key, aux);
}


/**
 * @brief Find the string corresponding to a given key.
 * @param key The key to look up.
 * @param aux[out] Auxiliary data associated with the key.
 * @return Pointer to the string found, or null.
 *         We can find keys as long as the corresponding string
 *         was given to either @c stringToKey() or @c registerKey().
 */
const std::string* StringPool::keyToString (sgkey_t key,
                                            sgaux_t& aux) const
{
  return m_impl->keyToString (key, aux);
}


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
bool StringPool::registerKey (sgkey_t key,
                              const std::string& str,
                              sgaux_t aux /*= 0*/)
{
  // Make sure the primary mapping is registered first.
  stringToKey (str, aux);
  return m_impl->registerKey (key, str, aux);
}


/**
 * @brief Debugging dump.  Write to cout.
 */
void StringPool::dump () const
{
  m_impl->dump();
}


} // namespace SG
