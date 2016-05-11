/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaPoolCnvSvc/src/TPCnvElt.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Helper for calling a TP converter from an Athena converter.
 */


#include "AthenaPoolCnvSvc/TPCnvElt.h"
#include "AthenaPoolCnvSvc/exceptions.h"
#include "StorageSvc/DbReflex.h"


namespace AthenaPoolCnvSvc {


/**
 * @brief Given a @c type_info, get the corresponding pool guid.
 * @param ti @c type_info to look for.
 *
 * Throws an exception on errors.
 */
Guid guidFromTypeinfo (const std::type_info& ti)
{
  pool::TypeH typ = pool::DbReflex::forTypeInfo (ti);
  if (!typ) throwExcNoDictForClass (ti);
  return pool::DbReflex::guid (typ);
}


} // namespace AthenaPoolCnvSvc
