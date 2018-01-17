// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file StoreGate/exceptions.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Exceptions that can be thrown from StoreGate.
 */


#ifndef STOREGATE_EXCEPTIONS_H
#define STOREGATE_EXCEPTIONS_H


#include "GaudiKernel/ClassID.h"
#include <stdexcept>
#include <typeinfo>
#include <string>


namespace SG {


/**
 * @brief Exception --- Attempt to dereference a Read/Write/UpdateHandle with a null key.
 *
 * An explicit key must always be given when using Read/Write/UpdateHandle.
 */
class ExcNullHandleKey
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   */
  ExcNullHandleKey();
};


/**
 * @brief Throw a SG::ExcNullHandleKey exception.
 */
[[noreturn]]
void throwExcNullHandleKey();


/**
 * @brief Exception --- Bad key format for VarHandleKey.
 *
 * The key for a VarHandle must be of the form KEY or STORE/KEY;
 * no more than one slash may be present in the key string.
 */
class ExcBadHandleKey
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param key The supplied key string.
   */
  ExcBadHandleKey (const std::string& key);
};


/**
 * @brief Exception --- Forbidden method called.
 */
class ExcForbiddenMethod
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param name Name of the called method.
   */
  ExcForbiddenMethod (const std::string& name);
};


/**
 * @brief Exception --- Error initializing VarHandle from VarHandleKey.
 */
class ExcHandleInitError
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param clid CLID from the key.
   * @param sgkey StoreGate key from the key.
   * @param storename Store name from the key.
   */
  ExcHandleInitError (CLID clid,
                      const std::string& sgkey,
                      const std::string& storename);
};


/**
 * @brief Exception --- Tried to create a handle from an uninitialized key.
 */
class ExcUninitKey
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param clid CLID from the key.
   * @param sgkey StoreGate key from the key.
   * @param storename Store name from the key.
   */
  ExcUninitKey (CLID clid,
                const std::string& sgkey,
                const std::string& storename);
};


/**
 * @brief Exception --- Tried to retrieve non-const pointer to const object.
 */
class ExcConstObject
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param clid CLID from the key.
   * @param sgkey StoreGate key from the key.
   * @param storename Store name from the key.
   */
  ExcConstObject (CLID clid,
                  const std::string& sgkey,
                  const std::string& storename);
};


/**
 * @brief Exception --- Attempt to dereference write handle before record.
 */
class ExcNullWriteHandle
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param clid CLID from the key.
   * @param sgkey StoreGate key from the key.
   * @param storename Store name from the key.
   */
  ExcNullWriteHandle (CLID clid,
                      const std::string& sgkey,
                      const std::string& storename);
};


/**
 * @brief Throw a SG::ExcNullWriteHandle exception.
 * @param clid CLID from the key.
 * @param sgkey StoreGate key from the key.
 * @param storename Store name from the key.
 */
[[noreturn]]
void throwExcNullWriteHandle (CLID clid,
                              const std::string& sgkey,
                              const std::string& storename);


/**
 * @brief Exception --- Deference of read handle failed.
 */
class ExcNullReadHandle
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param clid CLID from the key.
   * @param sgkey StoreGate key from the key.
   * @param storename Store name from the key.
   */
  ExcNullReadHandle (CLID clid,
                     const std::string& sgkey,
                     const std::string& storename);
};


/**
 * @brief Throw a SG::ExcNullReadHandle exception.
 * @param clid CLID from the key.
 * @param sgkey StoreGate key from the key.
 * @param storename Store name from the key.
 */
[[noreturn]]
void throwExcNullReadHandle (CLID clid,
                             const std::string& sgkey,
                             const std::string& storename);


/**
 * @brief Exception --- Deference of update handle failed.
 */
class ExcNullUpdateHandle
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param clid CLID from the key.
   * @param sgkey StoreGate key from the key.
   * @param storename Store name from the key.
   */
  ExcNullUpdateHandle (CLID clid,
                       const std::string& sgkey,
                       const std::string& storename);
};


/**
 * @brief Throw a SG::ExcNullUpdateHandle exception.
 * @param clid CLID from the key.
 * @param sgkey StoreGate key from the key.
 * @param storename Store name from the key.
 */
[[noreturn]]
void throwExcNullUpdateHandle (CLID clid,
                               const std::string& sgkey,
                               const std::string& storename);


/**
 * @brief Exception --- updatedObject failed.
 */
class ExcUpdatedObjectFailure
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param clid CLID from the key.
   * @param sgkey StoreGate key from the key.
   * @param storename Store name from the key.
   */
  ExcUpdatedObjectFailure (CLID clid,
                           const std::string& sgkey,
                           const std::string& storename);
};


/**
 * @brief Throw a SG::ExcUpdatedObjectFailure exception.
 * @param clid CLID from the key.
 * @param sgkey StoreGate key from the key.
 * @param storename Store name from the key.
 */
[[noreturn]]
void throwExcUpdatedObjectFailure (CLID clid,
                                   const std::string& sgkey,
                                   const std::string& storename);


/**
 * @brief Exception --- Attempt to get non-const VarHandleKey from non-owning VarHandle.
 *
 * If a Read/Write/Update handle is initialized from a HandleKey object, then
 * that HandleKey is const, and cannot be retrieved as a non-const reference.
 * A non-const HandleKey can only be retrieved if the handle was not
 * initialized from a HandleKey.
 */
class ExcNonConstHandleKey
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param clid CLID from the key.
   * @param sgkey StoreGate key from the key.
   * @param storename Store name from the key.
   */
  ExcNonConstHandleKey (CLID clid,
                        const std::string& sgkey,
                        const std::string& storename);
};


/**
 * @brief Throw a SG::ExcNonConstHandleKey exception.
 * @param clid CLID from the key.
 * @param sgkey StoreGate key from the key.
 * @param storename Store name from the key.
 */
[[noreturn]]
void throwExcNonConstHandleKey (CLID clid,
                                const std::string& sgkey,
                                const std::string& storename);


} // namespace SG


#endif // not STOREGATE_EXCEPTIONS_H
