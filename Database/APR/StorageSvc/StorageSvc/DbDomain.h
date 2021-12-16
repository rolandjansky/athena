/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbDomain.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbDomain and related class definitions
//--------------------------------------------------------------------
//
//  Package    : StorageSvc  (The POOL project)
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBDOMAIN_H
#define POOL_DBDOMAIN_H

// Framework include files
#include "StorageSvc/pool.h"
#include "StorageSvc/DbHandleBase.h"

// STL include files
#include <vector>

class Token;

/*
 *  POOL namespace declaration
 */
namespace pool    {

  // Forward declarations
  class IDbDomain;
  class IOODatabase;
  class DbDomainObj;
  class DbDatabaseObj;
  class DbSession;
  class DbOption;

  /** @class DbDomain DbDomain.h StorageSvc/DbDomain.h
    *
    * Description:
    *
    * Handle managing a DbDomainObj, a generic Database domain object.
    *
    * @author  M.Frank
    * @version 1.0
    */
  class DbDomain : public DbHandleBase<DbDomainObj>  {
  public:
    /// Friend declarations
    friend class DbDomainObj;
  private:
    /// Assign transient object properly (including reference counting)
    void switchPtr(DbDomainObj* obj);
  public:
    /// Friend's constructor
    DbDomain(DbDomainObj* dom);
    /// Constructor using Db type
    DbDomain(const DbType& type)   {   m_type = type.majorType();       }
    /// Copy constructor
    DbDomain(const DbDomain& copy)      
    : DbHandleBase<DbDomainObj>()              {   *this = copy;        }
    /// Standard destructor
    virtual ~DbDomain()                        {   switchPtr( 0 );      }
    /// Assignment operator
    DbDomain& operator=(const DbDomain& copy)  {
      if ( &copy != this )  {
        m_type = copy.m_type;
        switchPtr(copy.m_ptr);
      }
      return *this;
    }
    /// Assignment operator to reset handel easily using 0
    DbDomain& operator=(const int /* null_obj */ )    {
      switchPtr(0);
      return *this;
    }
    /// Acces to the domain name
    const std::string& name() const;
    /// Access reference counter
    int refCount() const;

    /// Check for the existence of a domain within a session
    /**
      * @param  sesH          [IN]  Handle to the database session
      *                             this database domain belongs to.
      * @param  technology    [IN]  Technology identifier of the domain.
      *
      * @return boolean value indicating if the database was found.
      */
    bool exist(DbSession& sesH, int technology) const;

    /// Open domain within a session environment
    /**
      * @param  context       [IN]  Handle to user defined domain context
      * @param  sesH          [IN]  Handle to the database session
      *                             this database domain belongs to.
      * @param  technology    [IN]  Technology identifier of this domain.
      * @param  mod           [IN]  Access mode: READ, UPDATE, CREATE etc.
      *
      * @return DbStatus code indicating success or failure.
      */
    DbStatus open(DbSession&  sesH, 
                  const DbType&     technology,
                  DbAccessMode      mod=pool::READ);
    /// Close domain
    DbStatus close();
    /// Access to access mode
    DbAccessMode openMode() const;
    /// Access to session handle
    DbSession containedIn() const;
    /// Check if Database exists within the domain
    bool existsDbase(const std::string& db_name);
    /// Find Database in domain
    const DbDatabaseObj* find(const std::string& db_name) const;
          DbDatabaseObj* find(const std::string& db_name);
    /// Add domain to session
    DbStatus add(const std::string& nam, DbDatabaseObj* db);
    /// Find domain in session
    DbStatus remove(DbDatabaseObj* db);
    /// Increase the age of all open databases
    DbStatus ageOpenDbs();
    /// Check if databases are present, which aged a lot and need to be closed
    DbStatus closeAgedDbs();
    /// Set the maximal allowed age limit for files in this domain
    void setAgeLimit(int value);
    /// Access the maximal age limit
    int ageLimit()  const;
    /// Let the implementation access the internals
    IDbDomain* info();
    const IDbDomain* info() const;

    /// Set domain specific options
    /** @param refOpt   [IN]  Reference to option object
      *
      * @return DbStatus code indicating success or failure.  
      */
    DbStatus setOption(const DbOption& refOpt);

    /// Access domain specific options
    /** @param refOpt   [IN]  Reference to option object
      *
      * @return DbStatus code indicating success or failure.  
      */
    DbStatus getOption(DbOption& refOpt) const;

    /// Allow access to the Database implementation
    IOODatabase* db();
    const IOODatabase* db()   const;
  };
}       // End namespace pool
#endif  // POOL_DBDOMAIN_H
