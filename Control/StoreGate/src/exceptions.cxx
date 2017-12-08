/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file StoreGate/src/exceptions.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Exceptions that can be thrown from StoreGate.
 */


#include "StoreGate/exceptions.h"
#include <sstream>


namespace SG {


/**
 * @brief Constructor.
 */
ExcNullHandleKey::ExcNullHandleKey()
  : std::runtime_error ("SG::ExcNullHandleKey: Attempt to dereference a Read/Write/UpdateHandle with a null key.")
{
}


/**
 * @brief Throw a SG::ExcNullHandleKey exception.
 */
[[noreturn]]
void throwExcNullHandleKey()
{
  throw ExcNullHandleKey();
}


//****************************************************************************


/**
 * @brief Constructor.
 * @param key The supplied key string.
 */
ExcBadHandleKey::ExcBadHandleKey (const std::string& key)
  : std::runtime_error ("SG::ExcBadHandleKey: Bad key format for VarHandleKey: `" + key + "'")
{
}


//****************************************************************************


/**
 * @brief Constructor.
 * @param name Name of the called method.
 */
ExcForbiddenMethod::ExcForbiddenMethod (const std::string& name)
  : std::runtime_error ("SG::ExcForbiddenMethod: Forbidden method called: `" + name + "'")
{
}


//****************************************************************************


/// Helper: format exception error string.
std::string excHandleInitError_format (CLID clid,
                                       const std::string& sgkey,
                                       const std::string& storename)
                                       
{
  std::ostringstream os;
  os << "SG::ExcHandleInitError: "
     << "Error initializing VarHandle from VarHandleKey: "
     << storename << "+" << sgkey << "[" << clid << "]";
  return os.str();
}


/**
 * @brief Constructor.
 * @param clid CLID from the key.
 * @param sgkey StoreGate key from the key.
 * @param storename Store name from the key.
 */
ExcHandleInitError::ExcHandleInitError (CLID clid,
                                        const std::string& sgkey,
                                        const std::string& storename)
  : std::runtime_error (excHandleInitError_format (clid, sgkey, storename))
{
}


//****************************************************************************


/// Helper: format exception error string.
std::string excUninitKey_format (CLID clid,
                                 const std::string& sgkey,
                                 const std::string& storename)
                                       
{
  std::ostringstream os;
  os << "SG::ExcUninitKey: "
     << "Error initializing VarHandle from uninitialized VarHandleKey: "
     << storename << "+" << sgkey << "[" << clid << "]; "
     << "keys should be initialized in your initialize().";
  return os.str();
}


/**
 * @brief Constructor.
 * @param clid CLID from the key.
 * @param sgkey StoreGate key from the key.
 * @param storename Store name from the key.
 */
ExcUninitKey::ExcUninitKey (CLID clid,
                            const std::string& sgkey,
                            const std::string& storename)
  : std::runtime_error (excUninitKey_format (clid, sgkey, storename))
{
}


//****************************************************************************


/// Helper: format exception error string.
std::string excConstObject_format (CLID clid,
                                   const std::string& sgkey,
                                   const std::string& storename)
  
{
  std::ostringstream os;
  os << "SG::ExcConstObject: "
     << "Tried to retrieve non-const pointer to const object via VarHandleKey: "
     << storename << "+" << sgkey << "[" << clid << "]";
  return os.str();
}


/**
 * @brief Constructor.
 * @param clid CLID from the key.
 * @param sgkey StoreGate key from the key.
 * @param storename Store name from the key.
 */
ExcConstObject::ExcConstObject (CLID clid,
                                const std::string& sgkey,
                                const std::string& storename)
  : std::runtime_error (excConstObject_format (clid, sgkey, storename))
{
}


//****************************************************************************


/// Helper: format exception error string.
std::string excNullWriteHandle_format (CLID clid,
                                       const std::string& sgkey,
                                       const std::string& storename)
  
{
  std::ostringstream os;
  os << "SG::ExcNullWriteHandle: "
     << "Attempt to dereference write handle before record: "
     << storename << "+" << sgkey << "[" << clid << "]";
  return os.str();
}


/**
 * @brief Constructor.
 * @param clid CLID from the key.
 * @param sgkey StoreGate key from the key.
 * @param storename Store name from the key.
 */
ExcNullWriteHandle::ExcNullWriteHandle (CLID clid,
                                        const std::string& sgkey,
                                        const std::string& storename)
  : std::runtime_error (excNullWriteHandle_format (clid, sgkey, storename))
{
}


/**
 * @brief Throw a SG::ExcNullWriteHandle exception.
 * @param clid CLID from the key.
 * @param sgkey StoreGate key from the key.
 * @param storename Store name from the key.
 */
void throwExcNullWriteHandle (CLID clid,
                              const std::string& sgkey,
                              const std::string& storename)
{
  throw ExcNullWriteHandle (clid, sgkey, storename);
}


//****************************************************************************


/// Helper: format exception error string.
std::string excNullReadHandle_format (CLID clid,
                                      const std::string& sgkey,
                                      const std::string& storename)
  
{
  std::ostringstream os;
  os << "SG::ExcNullReadHandle: "
     << "Dereference of read handle failed: "
     << storename << "+" << sgkey << "[" << clid << "]";
  return os.str();
}


/**
 * @brief Constructor.
 * @param clid CLID from the key.
 * @param sgkey StoreGate key from the key.
 * @param storename Store name from the key.
 */
ExcNullReadHandle::ExcNullReadHandle (CLID clid,
                                      const std::string& sgkey,
                                      const std::string& storename)
  : std::runtime_error (excNullReadHandle_format (clid, sgkey, storename))
{
}


/**
 * @brief Throw a SG::ExcNullReadHandle exception.
 * @param clid CLID from the key.
 * @param sgkey StoreGate key from the key.
 * @param storename Store name from the key.
 */
void throwExcNullReadHandle (CLID clid,
                             const std::string& sgkey,
                             const std::string& storename)
{
  throw ExcNullReadHandle (clid, sgkey, storename);
}


//****************************************************************************


/// Helper: format exception error string.
std::string excNullUpdateHandle_format (CLID clid,
                                        const std::string& sgkey,
                                        const std::string& storename)
  
{
  std::ostringstream os;
  os << "SG::ExcNullUpdateHandle: "
     << "Dereference of update handle failed: "
     << storename << "+" << sgkey << "[" << clid << "]";
  return os.str();
}


/**
 * @brief Constructor.
 * @param clid CLID from the key.
 * @param sgkey StoreGate key from the key.
 * @param storename Store name from the key.
 */
ExcNullUpdateHandle::ExcNullUpdateHandle (CLID clid,
                                          const std::string& sgkey,
                                          const std::string& storename)
  : std::runtime_error (excNullUpdateHandle_format (clid, sgkey, storename))
{
}


/**
 * @brief Throw a SG::ExcNullUpdateHandle exception.
 * @param clid CLID from the key.
 * @param sgkey StoreGate key from the key.
 * @param storename Store name from the key.
 */
void throwExcNullUpdateHandle (CLID clid,
                               const std::string& sgkey,
                               const std::string& storename)
{
  throw ExcNullUpdateHandle (clid, sgkey, storename);
}


//****************************************************************************


/// Helper: format exception error string.
std::string excUpdatedObjectFailure_format (CLID clid,
                                            const std::string& sgkey,
                                            const std::string& storename)
  
{
  std::ostringstream os;
  os << "SG::ExcUpdatedObjectFailure: "
     << "updatedObject failed: "
     << storename << "+" << sgkey << "[" << clid << "]";
  return os.str();
}


/**
 * @brief Constructor.
 * @param clid CLID from the key.
 * @param sgkey StoreGate key from the key.
 * @param storename Store name from the key.
 */
ExcUpdatedObjectFailure::ExcUpdatedObjectFailure (CLID clid,
                                                  const std::string& sgkey,
                                                  const std::string& storename)
  : std::runtime_error (excUpdatedObjectFailure_format (clid, sgkey, storename))
{
}


/**
 * @brief Throw a SG::ExcUpdatedObjectFailure exception.
 * @param clid CLID from the key.
 * @param sgkey StoreGate key from the key.
 * @param storename Store name from the key.
 */
void throwExcUpdatedObjectFailure (CLID clid,
                                   const std::string& sgkey,
                                   const std::string& storename)
{
  throw ExcUpdatedObjectFailure (clid, sgkey, storename);
}


} // namespace SG
