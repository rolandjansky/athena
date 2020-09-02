// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file SGTools/exceptions.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2013
 * @brief Exceptions that can be thrown by SGTools.
 */


#ifndef SGTOOLS_EXCEPTIONS_H
#define SGTOOLS_EXCEPTIONS_H


#include "AthenaKernel/sgkey_t.h"
#include "GaudiKernel/ClassID.h"
#include <typeinfo>
#include <stdexcept>
#include <string>


namespace SG {


/**
 * @brief Exception --- Bad cast of DataProxy with CLID 
 *
 * You tried to retrieve an object in StoreGate as an incompatible type.
 */
class ExcBadDataProxyCast
  : public std::bad_cast
{
public:
  /**
   * @brief Constructor.
   * @param id CLID of the DataProxy.
   * @param tid Type to which we're trying to convert the object.
   */
  ExcBadDataProxyCast (CLID id, const std::type_info& tid);


  // Needed for c++98 compatibility.
  ~ExcBadDataProxyCast() throw() {}


  /**
   * @brief Return the message for this exception.
   */
  virtual const char* what() const throw();


private:
  /// The message for this exception.
  std::string m_what;
};


/**
 * @brief Throw an ExcBadDataProxyCast exception.
 * @param id CLID of the DataProxy.
 * @param tid Type to which we're trying to convert the object.
 */
[[noreturn]]
void throwExcBadDataProxyCast (CLID id, const std::type_info& tid);


/**
 * @brief Exception --- Proxy collision for clid/key
 *
 * DataStore was attempting to fill in the CLID/key for a dummy proxy,
 * but there was already another proxy present with that CLID/key.
 *
 * This should never happen.
 */
class ExcProxyCollision
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param id CLID we're trying to set on the dummy proxy.
   * @param key Key we're trying to set on the dummy proxy.
   * @param primary_id CLID of the existing proxy.
   * @param primary_key Key of the existing proxy.
   */
  ExcProxyCollision (CLID id, const std::string& key,
                     CLID primary_id, const std::string& primary_key);
};


/**
 * @brief Exception --- sgkey hash collision
 *
 * We're trying to add a new non-transient key to a store, but the
 * key+clid hashes to a sgkey_t value that's already used.
 */
class ExcSgkeyCollision
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param new_key String SG key of the new item we're trying to add.
   * @param new_clid CLID for the new item we're trying to add.
   * @param old_key String SG key of the existing item.
   * @param old_clid CLID for the existing item.
   * @param sgkey Hashed key of both.
   */
  ExcSgkeyCollision (const std::string& new_key, CLID new_clid,
                     const std::string& old_key, CLID old_clid,
                     sgkey_t sgkey);
};


} // namespace SG


#endif // not SGTOOLS_EXCEPTIONS_H
