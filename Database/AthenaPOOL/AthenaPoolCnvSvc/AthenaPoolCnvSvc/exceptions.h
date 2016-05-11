// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
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
o */
[[noreturn]]
void throwExcUnsupportedVersion (const std::type_info& ti, const Guid& guid);

  
} // namespace AthenaPoolCnvSvc


#endif // not EXCEPTIONS_H
