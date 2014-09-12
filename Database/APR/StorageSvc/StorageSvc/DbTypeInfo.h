/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbTypeInfo.h 601961 2014-06-16 14:49:09Z mnowak $
//====================================================================
//  DbTypeInfo definition
//--------------------------------------------------------------------
//
//  Package    : StorageSvc  (The POOL project)
//
//  Description: Type information for persistent objects
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBTYPEINFO_H
#define POOL_DBTYPEINFO_H 1

// Framework include files
#include "DataModelRoot/RootType.h"
#include "StorageSvc/pool.h"
#include "StorageSvc/Shape.h"

// C/C++ include files
#include <vector>

/*
 *  POOL namespace declaration
 */
namespace pool  {

  // Forward declaration
  class DbColumn;
  class DbTypeInfo;

  /** @class DbTypeInfo DbTypeInfo.h StorageSvc/DbTypeInfo.h
    * Definition of class DbTypeInfo
    *
    * Description:
    * Generalized persistent type information
    *
    * @author  M.Frank
    * @version 1.0
    */
  class DbTypeInfo : public Shape   {

  public:
    /// Definition of reflection class handle
    typedef RootType TypeH;
    /// Column Container definition
    typedef std::vector<const DbColumn*> Columns;

  private:
    /// Reference counter
    mutable int m_refCount;
    /// Column definitions
    Columns     m_columns;
    /// Multiplicity
    int         m_mult;
    /// Reflection class
    TypeH       m_class;

  protected:
    /// Constructor with type id
    DbTypeInfo(const Guid& guid, TypeH cl);
    DbTypeInfo(const Guid& guid);

    /// Destructor
    virtual ~DbTypeInfo();

    /// Load type information object from string representation
    DbStatus i_fromString(const std::string& string_rep);

  public:
    /// Destroy type information; to be used with extreme care
    void destroy() {  delete this; }
    /// Create type information using name
    /** Note: Existence of reflection class is Mandatory!  */
    static DbTypeInfo* create(const std::string& cl_name);
    /// Create type information using name
    static DbTypeInfo* create(const std::string& cl_name, Columns& cols);
    /// Create type information using Guid only Class must already be registered.
    /** Note: Existence of reflection class is Mandatory!  */
    static DbTypeInfo* create(const Guid& guid);
    /// Create type information using Guid only Class must already be registered.
    /** Note: Existence of reflection class is Mandatory!  */
    static DbTypeInfo* create(const Guid& guid, Columns& cols);
    /// Create type information using Guid only Class must already be registered.
    /** Note: Existence of reflection class is NOT mandatory!  */
    static DbTypeInfo* createEx(const Guid& guid);
    /// Create type information using Guid only Class must already be registered.
    /** Note: Existence of reflection class is NOT mandatory!  */
    static DbTypeInfo* createEx(const Guid& guid, Columns& cols);
    /// Load type information object from string representation
    static const DbTypeInfo* fromString(const std::string& string_rep);
    /// Access type name by type identifier from RTTI
    static const std::string typeName(const std::type_info& typ);
    // Requests ownership
    void addRef() const { ++m_refCount; }
    /// Releases the ownership
    void deleteRef() const;
    /// Access to reflection class (if availible)
    TypeH clazz() const;
    /// Prepare for the case of discrete columns: Column information
    const Columns& columns()  const       {  return m_columns;                }
    /// Modify content of the object by editing columns
    Columns& columns()                    {  return m_columns;                }
    /// Create string representation of the type information object
    const std::string toString() const;
    /// Allow usage of base classes
    DbStatus declareBase(const DbTypeInfo* pInfo);
  };
}      // End namespace pool
#endif // POOL_DBTYPEINFO_H
