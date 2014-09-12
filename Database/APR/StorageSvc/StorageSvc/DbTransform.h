/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Header: /cvs/PF/pool/StorageSvc/StorageSvc/DbTransform.h,v 1.5 2003/10/29 13:58:14 frankb Exp $
#ifndef POOL_DBTRANSFORM_H
#define POOL_DBTRANSFORM_H

// Framework includes
#include "StorageSvc/DbStatus.h"

class Guid;

/* 
 *  pool namespace declaration
 */
namespace pool  {
  class DbTypeInfo;

  /** @class DbTransform DbTransform.h StorageSvc/DbTransform.h
    *
    * @author   M.Frank
    * @version  1.0
    * @date     03/09/2002
    */
  class DbTransform    {
  private:
    /// This class does not want to be instantiated....
    DbTransform()   {}
  public:
    /// Access shape registry
    static DbStatus getShape    (const Guid&        shape_Guid,
                                 const DbTypeInfo*& shape);
    /// Access entry in shape registry
    static DbStatus regShape    (const DbTypeInfo*  shape);
    /// Access entry in shape registry
    static DbStatus removeShape (const DbTypeInfo*  shape);
  };
}      // End namespace pool
#endif // POOL_DBTRANSFORM_H
