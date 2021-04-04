/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolCnvSvc/src/exceptions.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Exceptions that can be thrown from AthenaPoolCnvSvc.
 */


#include "AthenaPoolCnvSvc/exceptions.h"
#include "CxxUtils/exctrace.h"
#include "GaudiKernel/System.h"
#include <sstream>


namespace AthenaPoolCnvSvc {


/// Helper: format exception error string.
std::string excNoDictForClass_format (const std::type_info& ti)
{
  std::ostringstream os;
  os << "AthenaPoolCnvSvc::::ExcNoDictForClass: "
     << "Can't find dictionary information for class: ";
  os << System::typeinfoName(ti);
  return os.str();
}


/**
 * @brief Constructor.
 * @param ti The requested class.
 */
ExcNoDictForClass::ExcNoDictForClass (const std::type_info& ti)
  : std::runtime_error (excNoDictForClass_format (ti))
{
}


/**
 * @brief Throw a AthenaPoolCnvSvc::ExcNoDictForClass exception.
 * @param ti The requested class.
 */
void throwExcNoDictForClass (const std::type_info& ti)
{
  throw ExcNoDictForClass (ti);
}


//*************************************************************************


/// Helper: format exception error string.
std::string excUnsupportedVersion_format (const std::type_info& ti,
                                          const Guid& guid)
{
  std::ostringstream os;
  os << "AthenaPoolCnvSvc::::ExcUnsupportedVersion: "
     << "Unsupported persistent version of "
     << System::typeinfoName(ti)
     << " found; guid: " << guid.toString();
  return os.str();
}


/**
 * @brief Constructor.
 * @param ti The requested class.
 * @param guid The GUID of the persistent class.
 */
ExcUnsupportedVersion::ExcUnsupportedVersion (const std::type_info& ti,
                                              const Guid& guid)
  : std::runtime_error (excUnsupportedVersion_format (ti, guid))
{
}


/**
 * @brief Throw a AthenaPoolCnvSvc::ExcUnsupportedVersion exception.
 * @param ti The class being read.
 * @param guid The GUID of the persistent class.
 */
void throwExcUnsupportedVersion (const std::type_info& ti, const Guid& guid)
{
  throw ExcUnsupportedVersion (ti, guid);
}


//*************************************************************************


/// Helper: format exception error string.
std::string excCaughtException_format (const char* fnname,
                                       const char* action,
                                       const std::exception& ex,
                                       const std::type_info& ti,
                                       const std::string& key)
{
  std::ostringstream os;
  os << "AthenaPoolCnvSvc::::ExcCaughtException: "
     << "Caught exception in " << fnname
     << " while " << action
     << System::typeinfoName(ti) << "/" << key
     << ": " << System::typeinfoName(typeid(ex)) << ": " << ex.what();
  return os.str();
}


/**
 * @brief Constructor.
 * @param fnname Name of the function being executed.
 * @param action What we were doing when we caught the exception.
 * @param ex The caught exception.
 * @param ti The class being thinned.
 * @param key Key of the class being thinned.
 */
ExcCaughtException::ExcCaughtException (const char* fnname,
                                        const char* action,
                                        const std::exception& ex,
                                        const std::type_info& ti,
                                        const std::string& key)
  : std::runtime_error (excCaughtException_format (fnname, action, ex, ti, key))
{
}


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
void throwExcCaughtException (const char* fnname,
                              const char* action,
                              const std::exception& ex,
                              const std::type_info& ti,
                              const std::string& key)
{
  std::cout.flush();
  std::cerr.flush();
  CxxUtils::exctrace (ex);
  throw ExcCaughtException (fnname, action, ex, ti, key);
}


} // namespace AthenaPoolCnvSvc
