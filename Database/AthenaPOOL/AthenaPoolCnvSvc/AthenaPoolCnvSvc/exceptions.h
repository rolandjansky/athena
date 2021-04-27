// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolCnvSvc/exceptions.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Exceptions that can be thrown from AthenaPoolCnvSvc.
 */


#ifndef ATHENAPOOLCNVSVC_EXCEPTIONS_H
#define ATHENAPOOLCNVSVC_EXCEPTIONS_H


#include "PersistentDataModel/Guid.h"
#include <stdexcept>
#include <typeinfo>
#include <string>


namespace AthenaPoolCnvSvc {


/**
 * @brief Exception --- Can't find dictionary information for class.
 *
 * POOL was unable to find the dictionary information for a given class.
 */
class ExcNoDictForClass
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param ti The requested class.
   */
  ExcNoDictForClass (const std::type_info& ti);
};


/**
 * @brief Throw a AthenaPoolCnvSvc::ExcNoDictForClass exception.
 * @param ti The requested class.
 */
[[noreturn]]
void throwExcNoDictForClass (const std::type_info& ti);

  
/**
 * @brief Exception --- Unsupported persistent version of CLASS found.
 *
 * The guid of the persistent class was not recognized.
 */
class ExcUnsupportedVersion
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param ti The class being read.
   * @param guid The GUID of the persistent class.
   */
  ExcUnsupportedVersion (const std::type_info& ti, const Guid& guid);
};


/**
 * @brief Throw a AthenaPoolCnvSvc::ExcUnsupportedVersion exception.
 * @param ti The class being read.
 * @param guid The GUID of the persistent class.
 */
[[noreturn]]
void throwExcUnsupportedVersion (const std::type_info& ti, const Guid& guid);

  
/**
 * @brief Exception --- Caught an exception during conversion.
 *
 * Encountered an exception during conversion.
 */
class ExcCaughtException
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param fnname Name of the function being executed.
   * @param action What we were doing when we caught the exception.
   * @param ex The caught exception.
   * @param ti The class being thinned.
   * @param key Key of the class being thinned.
   */
  ExcCaughtException (const char* fnname,
                      const char* action,
                      const std::exception& ex,
                      const std::type_info& ti,
                      const std::string& key);
};


/**
 * @brief Throw a AthenaPoolCnvSvc::ExcCaughtException exception.
 * @param fnname Name of the function being executed.
 * @param action What we were doing when we caught the exception.
 * @param ex The caught exception.
 * @param ti The class being thinned.
 * @param key Key of the class being thinned.
 *
 * Will also print a stack trace if exctrace is enabled.
 */
[[noreturn]]
void throwExcCaughtException (const char* fnname,
                              const char* action,
                              const std::exception& ex,
                              const std::type_info& ti,
                              const std::string& key);

  
} // namespace AthenaPoolCnvSvc


#endif // not EXCEPTIONS_H
