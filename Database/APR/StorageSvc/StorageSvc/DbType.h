/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbType.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbType definition file
//--------------------------------------------------------------------
//
//  Package    : System (The pool framework)
//
//  Description: Management of the object Databases
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBTYPE_H
#define POOL_DBTYPE_H 1

// Framework include files
#include <string>

/*
 *   POOL namespace declaration
 */
namespace pool    {

  // Forward declarations
  class DbPrint;

  /** @class DbType DbType.h  StorageSvc/DbType.h
  */
  class DbType    {
  private:
    /// Database type
    int m_type;
  public:
    /// MASK: all bits set (All bits set)
    static const constexpr unsigned int MASK       = ~0x0;
    /// MINOR type mask (1 low byte set)
    static const int MINOR_MASK = 0xFF;
    /// MAJOR type mask (3 high bytes set)
    static const constexpr unsigned int MAJOR_MASK = MASK<<8;

  public:
    /// Standard constructor
    DbType() : m_type(0)                {                         }
    /// Constructor with initializer from 
    DbType(int value) : m_type(value)   {                         }
    /// Default copy constructor
    DbType(const DbType&) = default;
    /// Standard destructor
    ~DbType() = default;
    /// Assignment operator
    DbType& operator=(const DbType& typ) {
      if ( this != &typ ) {
        m_type = typ.type();
      }
      return *this;
    }
    /// Equal operator
    bool operator==(const DbType& typ) const    
    {  return (m_type&MAJOR_MASK) == (typ.type()&MAJOR_MASK);     }
    /// Operator less
    bool operator<(const DbType& typ) const    
    {  return (m_type&MAJOR_MASK) < (typ.type()&MAJOR_MASK);      }
    /// Access to full type
    int type() const              {  return m_type;               }
    /// Access to major type
    int majorType() const         {  return type()&MAJOR_MASK;    }
    /// Access to minor type
    int minorType() const         {  return type()&MINOR_MASK;    }
    /// Check if types match
    bool match(DbType typ)  const {  return type() == typ.type(); }
    /// Check if type is within allowed range
    void check() const;
    /// Human readable storage type
    const std::string storageName()  const;
    /// Error message on missing back-end driver implementation
    void missingDriver( DbPrint& str) const;
    /// Error processing on bad storage type
    void badStorageType()  const;
    /// Access known storage type object by name
    static DbType getType(const std::string& name);
  };

  /// Check if type is within allowed range
  inline void DbType::check()  const {
    // Assume 0xF00 is biggest known mayor type
    if ( majorType() == 0 || majorType() > 0xF00 )         {
      badStorageType();
    }
  }

  inline const DbType makeTechnology(int major_typ, int minor_typ)
  { return ((major_typ<<8) + (DbType::MINOR_MASK&minor_typ));                       }

  // Data storage technique identifiers
  static const DbType TEST_StorageType       = makeTechnology(0,0);
  static const DbType POOL_StorageType       = makeTechnology(1,0);
  static const DbType ROOT_StorageType       = makeTechnology(2,0);
  static const DbType ROOTKEY_StorageType    = makeTechnology(2,1);
  static const DbType ROOTTREE_StorageType   = makeTechnology(2,2);
  static const DbType ROOTTREEINDEX_StorageType = makeTechnology(2,3);
  static const DbType OBJY_StorageType       = makeTechnology(3,0);
  static const DbType ACCESS_StorageType     = makeTechnology(4,0);
  static const DbType EXCEL_StorageType      = makeTechnology(5,0);
  static const DbType TEXTJET_StorageType    = makeTechnology(6,0);
  static const DbType SQLSERVER_StorageType  = makeTechnology(7,0);
  static const DbType MYSQL_StorageType      = makeTechnology(8,0);
  static const DbType ORACLE_StorageType     = makeTechnology(9,0);
  static const DbType XML_StorageType        = makeTechnology(10,0);

  static const DbType POOL_RDBMS_StorageType             = makeTechnology(11,0);
  static const DbType POOL_RDBMS_HOMOGENEOUS_StorageType = makeTechnology(11,1);
  static const DbType POOL_RDBMS_POLYMORPHIC_StorageType = makeTechnology(11,2);

}       // End namespace pool
#endif  // POOL_DBTYPE_H
