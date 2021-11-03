/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

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
#include "StorageSvc/Transaction.h"

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
  class DbOption;
  class DbTypeInfo;
  typedef const class Shape        *ShapeH;

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
    void switchPtr(DbDatabaseObj* obj);
  public:
    /// Constructor with initializing arguments
    DbDatabase(const DbType& typ=POOL_StorageType) { m_type = typ;        }
    /// Copy constructor
    DbDatabase(const DbDatabase& cp) : Base()   { switchPtr(cp.m_ptr);    }
    /// Constructor
    DbDatabase(DbDatabaseObj* obj)              { switchPtr(obj);         }
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
    /// Access to access mode member
    DbAccessMode openMode() const;
    /// Access to db name (FID)
    const std::string& name() const;
    /// Set db name (FID) after opening in case opening happened by PFN
    void setName(const std::string& nam);
    /// Access to db logon string
    const std::string& logon() const;
    /// Access the size of the database: May be undefined for some technologies
    long long int size();
    /// Allow access to the Database implementation
    IOODatabase* db();
    const IOODatabase* db()   const;
    /// Add domain to session
    DbStatus add(const std::string& name, DbContainerObj* cnt);
    /// Find domain in session
    DbStatus remove(const DbContainerObj* cnt);
    /// Select container object in Database
    const DbContainerObj* find(const std::string& nam) const;
    /// Select container object in Database
    DbContainerObj* find(const std::string& nam);
    /// Access the token of the database object
    const Token* token() const;
    /// Access to domain object
    const DbDomain& containedIn()    const;
          DbDomain& containedIn();
    /// Check for existence of Database within domain
    bool exist(DbDomain& domH, const std::string& nam) const;
    /// Open Database using given domain
    /** @param   domH      [IN]  Valid handle to domain object.
      * @param   pfn       [IN]  Physical file name of the database.
      * @param   lfn       [IN]  Logical file name of the database.
      * @param   mode      [IN]  Open mode (Default=READ).
      * @return Status code indicating success or failure.
      */
    DbStatus open(DbDomain&          domH,
                  const std::string& pfn,
                  const std::string& fid,
                  DbAccessMode mode = pool::READ);
    /// Re-open database with changing access permissions
    /** @param   mode      [IN]  Open mode (Valid modes are READ, UPDATE).
      * @return Status code indicating success or failure.
      */
    DbStatus reopen(DbAccessMode mode = pool::READ);
    /// Close Database
    DbStatus close();
    /// End database access, but still leave database accessible
    DbStatus retire();
    /// Check if the database was opened
    bool isOpen() const;
    /// Commit/Rollback Database Transaction
    /** @param   action     [IN]  action to perform
      * @return Status code indicating success or failure.
      */
    DbStatus transAct(Transaction::Action action);
    /// Set options
    /** @param   refOpt    [IN]  Reference to option object.
      * @return Status code indicating success or failure.
      */
    DbStatus setOption(const DbOption& refOpt);
    /// Access options
    /** @param   refOpt    [IN]  Reference to option object.
      * @return Status code indicating success or failure.
      */
    DbStatus getOption(DbOption& refOpt);
    /// Add a persistent parameter to the file
    DbStatus addParam(const std::string& nam, const std::string& val);
    /// Retrieve the number of user parameters
    int nParam();
    /// Retrieve existing parameter by name
    DbStatus param(const std::string& nam, std::string& val);
    /// Retrieve all parameters
    DbStatus params(Parameters& vals);
    /// Access to all token redirections from merged files
    const Redirections& redirections() const;

    /// read an object referenced by the token
    DbStatus read(const Token& token, ShapeH shape, void** object);
    /// Calculate required OID modification (shift) for source OID (oid) for a given merge section 
    DbStatus getRedirection(const Token::OID_t& oid, int merge_section, Token::OID_t& shift);
    /// Expand OID into a full Token, based on the Links table. For merged files provide links section#
    DbStatus getLink(const Token::OID_t& oid, int merge_section, Token* pTok);
    /// Retrieve container name from link container (using token oid, rather than contID)
    std::string cntName(Token& token);
    /// Add association link to link container
    DbStatus makeLink(Token* pToken, Token::OID_t& linkH);
    /// Access to sections if availible
    const ContainerSections& sections(const std::string& cnt);
    /// Add persistent shape to the Database
    DbStatus addShape (const DbTypeInfo* pShape);
    /// Retrieve persistent type information by class handle
    const DbTypeInfo* objectShape(const RootType& typeH);
    /// Retrieve persistent type information by name
    const DbTypeInfo* objectShape(const Guid& nam);
    /// Retrieve persistent type information by container
    const DbTypeInfo* contShape(const std::string& nam);
    /// Access local container token (if container exists)
    const Token* cntToken(const std::string& cntName);
    /// Allow access to all known containers
    DbStatus containers(std::vector<const Token*>& conts, 
                        bool intern=false);
    /// Allow access to all known associations between containers
    DbStatus associations(std::vector<const Token*>& assocs);
    /// Allow access to all known shapes used by the database
    DbStatus shapes(std::vector<const DbTypeInfo*>& shaps);
    /// Let the implementation access the internals
    IDbDatabase* info();
    const IDbDatabase* info()  const;
    /// Update database age
    void setAge(int value);
    /// Access age value
    int age()  const;
  };
}       // End namespace pool
#endif  // POOL_DBDatabase_H
