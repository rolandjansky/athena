/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaPoolCnvSvc/src/exceptions.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Exceptions that can be thrown from AthenaPoolCnvSvc.
 */


#include "AthenaPoolCnvSvc/exceptions.h"
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
  os << "AthenaPoolCnvSvc::::ExcUnsupported version: "
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


} // namespace AthenaPoolCnvSvc
