/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//  DbDomainObj object definition
//--------------------------------------------------------------------
//
//  Package    : StorageSvc  (The POOL project)
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBDOMAINOBJ_H
#define POOL_DBDOMAINOBJ_H 1

// Framework include files
#include "PersistentDataModel/Token.h"
#include "StorageSvc/DbAccessObj.h"
#include "StorageSvc/DbSession.h"

/*
 *  POOL namespace declaration
 */
namespace pool    {

  // Forward declarations
  class DbDatabaseObj;
  class DbOption;
  class IDbDomain;

  /** Db objects: DbDomainObj

      Description:
      Implementation independent part of a Database domain object
      objects.

      @author  M.Frank
      @version 1.0
  */
  class DbDomainObj : public  DbAccessObj<std::string, DbDatabaseObj >  {
  private:
    /// Handle to session
    DbSession       m_session;
    /// Maximal age of files allowed.
    int             m_maxAge;
    /// Technology dependent stuff
    IDbDomain*      m_info;
  public:
    /// Constructor
    DbDomainObj(DbSession&  session, 
                const DbType&     typ,
                DbAccessMode      mode = pool::READ);
    /// Standard destructor
    virtual ~DbDomainObj();
    /// Access to technology dependent implementation
    IDbDomain* info()                   {    return m_info;     }
    const IDbDomain* info() const       {    return m_info;     }
    /// Access session handle
    DbSession& session()                {    return m_session;  }
    const DbSession& session() const    {    return m_session;  }
    /// Set the maximal allowed age limit for files in this domain
    void       setAgeLimit(int value)   {    m_maxAge = value;  }
    /// Access the maximal age limit
    int        ageLimit()  const        {    return m_maxAge;   }
    /// Check for Database existence within domain
    bool existsDbase(const std::string& nam);
    /// Open domain with possible change of access mode
    DbStatus open(DbAccessMode mode);
    /// Open domain in default access mode
    DbStatus open();
    /// Close domain
    DbStatus close();
    /// Increase the age of all open databases
    DbStatus ageOpenDbs();
    /// Check if databases are present, which aged a lot and need to be closed
    DbStatus closeAgedDbs();
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
  };
}      // End namespace pool
#endif // POOL_DBDOMAINOBJ_H
