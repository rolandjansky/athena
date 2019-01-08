/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbColumn.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbColumn class definitions
//--------------------------------------------------------------------
//
//  Package    : StorageSvc  (The POOL project)
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBCOLUMN_H
#define POOL_DBCOLUMN_H 1

// Framework include files
#include "StorageSvc/DbStatus.h"
// STL include files
#include <typeinfo>
#include <string>

/*
 *  POOL namespace declaration
 */
namespace pool  {

  /** @class DbColumn DbColumn.h StorageSvc/DbColumn.h
    *
    * Description:
    * Definition of the generic column in a (relational) Database.
    *
    * @author  M.Frank
    * @version 1.0
    */
  class DbColumn    {
  public:
    enum Options {PRIMARY_KEY = 1 << 0, 
                  NOT_NULL    = 1 << 1,
                  UNIQUE      = 1 << 2,
                  CLUSTERED   = 1 << 3,
                  NONCLUSTERED= 1 << 4
    };
    // Accepted Data type definitions
    enum Type  {
          UNKNOWN = 0,        
          UCHAR,        USHORT,        UINT,        ULONG,
          CHAR,         SHORT,         INT,         LONG,
          BOOL,         FLOAT,         DOUBLE,
          STRING,       NTCHAR,        LONG_STRING, LONG_NTCHAR,
          OBJECT_REF,   CONTAINED_REF, TOKEN,       BLOB, 
          ANY, POINTER, LONGLONG,      ULONGLONG,
          NATIVE_TYPES = 1000
    };
  private:
    friend class DbTypeInfo;
    /// OS independent type name
    std::string           m_typeName;
    /// Column name
    std::string           m_colName;
    /// Offset from object pointer
    int                   m_offset;
    /// For fixed size arrays: number of object elements
    int                   m_nElem;
    /// Column options
    int                   m_opts;
    /// Object size
    int                   m_size;
    /// Storage Type identifier
    int                   m_type;
    /// Cache class information (if available)
    /// Do not allow unwanted instantiations by users
    DbColumn()
      : m_offset(0),
        m_nElem(0),
        m_opts(0),
        m_size(0),
        m_type(0)
    {}
  public:
    /// Default copy constructor
    DbColumn(const DbColumn&) = default;
    DbColumn& operator=(const DbColumn&) = default;
    /// Initializing constructor for fixed size arrays
    DbColumn(  const std::string& column_name, 
               int   object_type,
               int   object_offset,
               int   object_size,
               int   number_of_elements=1,
               int   opt=0);
    /// Initializing constructor for composite columns
    DbColumn(  const std::string& nam,
               const std::string& clazz,
               int   object_type,
               int   object_offset);
    /// Standard destructor
    virtual ~DbColumn()                 {                              }
    /// Is it a valid (known) data type ?
    bool isValid()    const             { return m_typeName.length()>0;}
    /// Access to column name
    const std::string& name() const     { return m_colName;            }
    /// Access to OS independent type name
    const std::string& typeName() const { return m_typeName;           }
    /// Access to options
    int options()   const               { return m_opts;               }
    /// Offset of column from this pointer
    int offset()    const               { return m_offset;             }
    /// Size of a single object within the column
    int size()      const               { return m_size;               }
    /// Number of elements
    int nElement()  const               { return m_nElem;              }
    /// Integer type identifier
    int typeID()    const               { return m_type;               }
    /// Create string representation of the type information object
    const std::string toString() const;
    /// Load type information object from string representation
    DbStatus fromString(const std::string& string_rep);
    /// Access type name by type identifier from RTTI
    static const std::string typeName(int typ);
  };
}       // End namespace pool
#endif  // POOL_DBCOLUMN_H
