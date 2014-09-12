/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbReflex.h 601961 2014-06-16 14:49:09Z mnowak $
//====================================================================
//  DbDatabase and related class definitions
//--------------------------------------------------------------------
//
//  Package    : StorageSvc  (The POOL project)
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBREFLEX_H
#define POOL_DBREFLEX_H

// Framework include files
#include "PersistentDataModel/Guid.h"
#include "DataModelRoot/RootType.h"

// C++ include files
#include <string>
#include <vector>
#include <typeinfo>
#include <set>

/*
 *  POOL namespace declaration
 */
namespace pool  {
  typedef RootType TypeH;

  /** @class DbReflex DbReflex.h StorageSvc/DbReflex.h
    *
    * Description:
    * Handle transparent loading of reflection classes.
    *
    * @author  M.Frank
    * @version 1.0
    */
  class DbReflex  {
  private:
    /// No public construction
    DbReflex() {}
    /// No public destruction
    ~DbReflex() {}
  public:
    /// Access classes by class name
    /** @param name     [IN]    type name
      *
      * @return  Pointer to reflection class if present.
      */
    static const TypeH forTypeName(const std::string& name);

    /// Access classes by RTTI type information
    /** @param info     [IN]    referenece to RTTI typeid
      *
      * @return  Pointer to reflection class if present.
      */
    static const TypeH forTypeInfo(const std::type_info& info);

    /// Access classes by Guid
    /** @param info     [IN]    Reference to Guid
      *
      * @return  Pointer to reflection class if present.
      */
    static const TypeH forGuid(const Guid& info);

    /// Access full class name with scope
    /** @param type     [IN]    Reflection type 
      *
      * @return  String containing full scoped name
      */
    static std::string fullTypeName(const TypeH& type);

    /// Determine Guid (normalized string form) from reflection type.
    /** @param type     [IN]    Reflection type 
      *
      * @return  String containing full scoped name
      */
    static Guid guid(const TypeH& id);
  };
}       // End namespace pool
#endif  // POOL_DBREFLEX_H
