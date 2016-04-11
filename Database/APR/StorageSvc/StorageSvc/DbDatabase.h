/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbDatabase.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbDatabase and related class definitions
//--------------------------------------------------------------------
//
//  Package    : StorageSvc  (The POOL project)
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBDATABASE_H
#define POOL_DBDATABASE_H

// Framework include files
#include "PersistentDataModel/Token.h"
#include "DataModelRoot/RootType.h"
#include "StorageSvc/pool.h"
#include "StorageSvc/DbSection.h"
#include "StorageSvc/DbHandleBase.h"

// STL include files
#include <vector>
#include <string>
#include <map>

/*
 *  POOL namespace declaration
 */
namespace pool  {

  // Forward declarations
  class IOODatabase;
  class IDbDatabase;
  class IDbContainer;
  class DbDomain;
  class DbDatabase;
  class DbDatabaseObj;
  class DbContainer;
  class DbContainerObj;
  class DbTransaction;
  class DbOption;
  class DbTypeInfo;

  /** @class DbDatabase DbDatabase.h StorageSvc/DbDatabase.h
   *
   * Description:
   * Handle managing a DbDatabaseObj, a generic Database object.
   *
   * @author  M.Frank
   * @version 1.0
   */
  class DbDatabase : public DbHandleBase<DbDatabaseObj> {
  public:
    /// Sections definition
    typedef std::vector<DbSection>              ContainerSections;
    /// Database redirection definition
    typedef std::pair<size_t,std::string>       Redirection;
    typedef std::map<std::string,Redirection>   Redirections;

  protected:
    /// Parameter definition
    typedef std::pair<std::string, std::string> Parameter;
    /// Parameter container definition
    typedef std::vector< Parameter >            Parameters;

    /// Assign transient object properly (including reference counting)
    void switchPtr(const DbDatabaseObj* obj) const;
  public:
    /// Constructor with initializing arguments
    DbDatabase(const DbType& typ=POOL_StorageType) { m_type = typ;        }
    /// Copy constructor
    DbDatabase(const DbDatabase& cp) : Base()   { switchPtr(cp.m_ptr);    }
    /// Constructor
    DbDatabase(const DbDatabaseObj* obj)        { switchPtr(obj);         }
    /// Standard destructor
    virtual ~DbDatabase()                       { switchPtr(0);           }
    /// Assignment (copy) operator
    DbDatabase& operator=(const DbDatabase& copy) {
      if ( &copy != this )  {
        m_type = copy.m_type;
        switchPtr( copy.m_ptr );
      }
      return *this;
    }
    /// Assignment operator to reset handle using 0
    DbDatabase& operator=(const int /* nuller */ )    {
      switchPtr(0);
      return *this;
    }
    /// Access reference counter
    int refCount()  const;
    /// Allow query if Transaction is active
    bool transactionActive() const;
    /// Access to access mode member
    DbAccessMode openMode() const;
    /// Access to db name (FID)
    const std::string& name() const;
    /// Set db name (FID) after opening in case opening happened by PFN
    void setName(const std::string& nam);
    /// Access to db logon string
    const std::string& logon() const;
    /// Access the size of the database: May be undefined for some technologies
    long long int size()  const;
    /// Allow access to the Database implementation
    IOODatabase* db()   const;
    /// Add domain to session
    DbStatus add(const std::string& name, DbContainerObj* cnt) const;
    /// Find domain in session
    DbStatus remove(const DbContainerObj* cnt);
    /// Select container object in Database
    const DbContainerObj* find(const std::string& nam) const;
    /// Access the token of the database object
    const Token* token() const;
    /// Access to domain object
    const DbDomain& containedIn()    const;
    /// Check for existence of Database within domain
    bool exist(const DbDomain& domH, const std::string& nam) const;
    /// Open Database using given domain
    /** @param   domH      [IN]  Valid handle to domain object.
      * @param   pfn       [IN]  Physical file name of the database.
      * @param   lfn       [IN]  Logical file name of the database.
      * @param   mode      [IN]  Open mode (Default=READ).
      * @return Status code indicating success or failure.
      */
    DbStatus open(const DbDomain& domH,
                  const std::string& pfn,
                  const std::string& fid,
                  DbAccessMode mode = pool::READ) const;
    /// Re-open database with changing access permissions
    /** @param   mode      [IN]  Open mode (Valid modes are READ, UPDATE).
      * @return Status code indicating success or failure.
      */
    DbStatus reopen(DbAccessMode mode = pool::READ) const;
    /// Close Database
    DbStatus close() const;
    /// End database access, but still leave database accessible
    DbStatus retire() const;
    /// Check if the database was opened
    bool isOpen() const;
    /// Start/Commit/Rollback Database Transaction
    /** @param   refTr     [IN]  Valid transaction handle.
      * @return Status code indicating success or failure.
      */
    DbStatus transAct(DbTransaction& refTr);
    /// Set options
    /** @param   refOpt    [IN]  Reference to option object.
      * @return Status code indicating success or failure.
      */
    DbStatus setOption(const DbOption& refOpt);
    /// Access options
    /** @param   refOpt    [IN]  Reference to option object.
      * @return Status code indicating success or failure.
      */
    DbStatus getOption(DbOption& refOpt) const;
    /// Add a persistent parameter to the file
    DbStatus addParam(const std::string& nam, const std::string& val) const;
    /// Retrieve the number of user parameters
    int nParam() const;
    /// Retrieve existing parameter by name
    DbStatus param(const std::string& nam, std::string& val) const;
    /// Retrieve all parameters
    DbStatus params(Parameters& vals) const;
    /// Retrieve association link from link container with also using section information
    DbStatus getRedirection(const Token& token, Token::OID_t& linkH) const;
    /// Access to all token redirections from merged files
    const Redirections& redirections() const;

    /// Retrieve association link from link container with also using section information
    DbStatus getLink(const Token::OID_t& lnkH, Token* pToken, const DbSection& section) const;
    /// Retrieve association link from link container
    DbStatus getLink(const Token::OID_t& lnkH, Token* pToken) const;
    /// Retrieve container name from link container (using token oid, rather than contID)
    std::string cntName(const Token& token) const;
    /// Add association link to link container
    DbStatus makeLink(const Token* pToken, Token::OID_t& linkH) const;
    /// Access to sections if availible
    const ContainerSections& sections(const std::string& cnt) const;
    /// Add persistent shape to the Database
    DbStatus addShape (const DbTypeInfo* pShape);
    /// Retrieve persistent type information by class handle
    const DbTypeInfo* objectShape(const RootType& typeH)  const;
    /// Retrieve persistent type information by name
    const DbTypeInfo* objectShape(const Guid& nam) const;
    /// Retrieve persistent type information by container
    const DbTypeInfo* contShape(const std::string& nam) const;
    /// Access local container token (if container exists)
    const Token* cntToken(const std::string& cntName) const;
    /// Allow access to all known containers
    DbStatus containers(std::vector<const Token*>& conts, 
                        bool intern=false) const;
    /// Allow access to all known associations between containers
    DbStatus associations(std::vector<const Token*>& assocs) const;
    /// Allow access to all known shapes used by the database
    DbStatus shapes(std::vector<const DbTypeInfo*>& shaps) const;
    /// Let the implementation access the internals
    IDbDatabase* info()  const;
    /// Update database age
    void setAge(int value) const;
    /// Access age value
    int age()  const;
  };
}       // End namespace pool
#endif  // POOL_DBDatabase_H
