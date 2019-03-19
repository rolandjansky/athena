/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//  DbDatabaseObj object definition
//--------------------------------------------------------------------
//
//  Package    : System (The POOL project)
//
//  Description: Generic data persistency
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBDATABASEOBJ_H
#define POOL_DBDATABASEOBJ_H 1

// We need the definition of the domain for defining the Database
#include "PersistentDataModel/Guid.h"
#include "StorageSvc/DbAccessObj.h"
#include "StorageSvc/DbDomain.h"
#include "StorageSvc/DbDatabase.h"
#include "StorageSvc/DbContainer.h"
#include "StorageSvc/DbSection.h"

// STL include files
#include <map>
#include <string>
#include <algorithm>

/*
 *  POOL namespace declaration
 */
namespace pool    {

  // Forward declarations
  class DbToken;

  /** @class DbDatabaseObj DbDatabaseObj.h src/DbDatabaseObj.h
    *
    * Description:
    *
    * Implementation independent part of a Database object
    * objects.
    *
    * @author  M.Frank
    * @version 1.0
    */
  class DbDatabaseObj : public  DbAccessObj<std::string, DbContainerObj > {
  private:
    /// Reflection class identifier
    typedef RootType                            TypeH;
    /// Database redirection definition
    typedef std::pair<size_t,std::string>            Redirection;
    typedef std::map<std::string,Redirection>        Redirections;
    /// Database section container definition
    typedef std::vector<DbSection>                   ContainerSections;
    typedef std::map<std::string,ContainerSections>  Sections;
    /// Database parameter definition
    typedef std::pair<std::string, std::string>      Parameter;
    /// Database parameter container definition
    typedef std::vector< Parameter >                 Parameters;
    /// Collection of retired database containers
    typedef std::vector< DbContainerObj >            Containers;
    /// Parameter map definition
    typedef std::map<std::string, std::string>       ParamMap;
    /// Definition of map with link elements
    typedef std::map< Guid , DbToken* >              LinkMap;
    /// Definition of array with link elements
    typedef std::vector<DbToken*>                    LinkVector;
    /// Definition of map with shape elements
    typedef std::map< Guid , const DbTypeInfo* >     ShapeMap;
    /// Handle to domain
    DbDomain                      m_dom;
    /// Handle to link container
    DbContainer                   m_links;
    /// Handle to shapes container
    DbContainer                   m_shapes;
    /// Handle to the parameter container
    DbContainer                   m_params;
    /// Technology dependent implementation block
    IDbDatabase*                  m_info;
    /// Map containing all links
    LinkMap                       m_linkMap;
    /// Random access array containing all links
    LinkVector                    m_linkVec;
    /// Map containing all links
    ShapeMap                      m_shapeMap;
    /// Map with all cached file properties
    ParamMap                      m_paramMap;
    /// Map with all cached mappings between known reflection classes
    std::map<TypeH, const DbTypeInfo*> m_classMap;
    /// Collection of retired database containers
    Containers                    m_retiredConts;
    /// Internal string representation type
    const DbTypeInfo*             m_string_t;
    /// Physical Database login
    std::string                   m_logon;
    /// File age counter
    int                           m_fileAge;
    /// Token describing the object
    Token*                        m_token;
    /// The section of merged files indexed by container name
    Sections                      m_sections;
    /// The section of merged files indexed by database name
    Redirections                  m_redirects;

  private:
    /// Perform cleanup of internal data structures
    DbStatus cleanup();
  public:
    /// Standard constructor of a Database obejct
    DbDatabaseObj( const DbDomain& dom, 
                   const std::string& pfn,
                   const std::string& fid,
                   DbAccessMode mode = pool::READ);
    /// Standard Destructor
    virtual ~DbDatabaseObj();
    /// Access to the logon string
    const std::string& logon() const  {  return m_logon;        }
    /// Access the size of the database: May be undefined for some technologies
    long long int size();
    /// Access to technology dependent implementation
    IDbDatabase* info() const         {  return m_info;         }
    /// Access to domain handle (CONST)
    DbDomain& domain()                {  return m_dom;          }
    /// Access age value
    int  age()  const                 {  return m_fileAge;      }
    /// Access the token of the database object
    const Token* token() const        {  return m_token;        }
    /// Access to all token redirections from merged files
    const Redirections& redirections() const { return m_redirects; }

    /// Open Database object
    DbStatus open();
    /// Re-open database with changing access permissions
    DbStatus reopen(DbAccessMode mode);
    /// Close database object
    DbStatus close();
    /// End database access, but still leave database accessible
    DbStatus retire();
    /// Check for pending updates
    bool updatesPending() const;
    /// Execute Database Transaction action
    DbStatus transAct(Transaction::Action action);

    /// Retrieve association link from token with redirection when file is merged.
    DbStatus getRedirection(const Token& token, Token::OID_t& obj_linkH);
    /// Retrieve association link from link container with also using section information
    DbStatus getLink(const Token::OID_t& lnkH, Token* pToken, const DbSection& section);
    /// Retrieve container name from link container (using token oid, rather than contID)
    std::string cntName(const Token& token);
    /// Add association link to link container
    DbStatus makeLink(const Token* pToken, Token::OID_t& refLink);
    /// Retrieve persistent type information by class handle
    const DbTypeInfo* objectShape(const TypeH& classH);
    /// Retrieve persistent type information by shape identifier
    const DbTypeInfo* objectShape(const Guid& nam);
    /// Retrieve persistent type information by container
    const DbTypeInfo* contShape(const std::string& nam);
    /// Add persistent type to the Database
    DbStatus addShape (const DbTypeInfo* pType);
    /// Access local container token (if container exists)
    const Token* cntToken(const std::string& cntName);
    /// Allow access to all known containers
    DbStatus containers(std::vector<const Token*>& conts,
                      bool intern);
    /// Allow access to all known associations between containers
    DbStatus associations(std::vector<const Token*>& conts);
    /// Allow access to all known shapes used by the database
    DbStatus shapes(std::vector<const DbTypeInfo*>& shaps);
    /// Retrieve the number of user parameters
    int nParam();
    /// Add a persistent parameter to the file
    DbStatus addParam(const std::string& nam, const std::string& val);
    /// Retrieve existing parameter by name
    DbStatus param(const std::string& nam, std::string& val);
    /// Retrieve all parameters
    DbStatus params(std::vector< std::pair<std::string, std::string> >& vals);
    /// Set options
    DbStatus setOption(const DbOption& refOpt);
    /// Access options
    DbStatus getOption(DbOption& refOpt);
    /// Access to sections if availible
    const ContainerSections& sections(const std::string& cnt) const;

    /// Update database age
    void setAge(int value);
  };
}       // End namespace pool
#endif  // POOL_DBDATABASEOBJ_H
