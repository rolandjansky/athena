/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
// $Id: IStorageExplorer.h 726071 2016-02-25 09:23:05Z krasznaa $
//
//  Package    : pool/StorageSvc (The pool framework)
//
//  @author      M.Frank
//
//====================================================================
// $Header: /cvs/PF/pool/StorageSvc/StorageSvc/IStorageExplorer.h,v 1.19 2010/05/18 09:45:51 avalassi Exp $
#ifndef POOL_ISTORAGEEXPLORER_H
#define POOL_ISTORAGEEXPLORER_H

// Framework include files
#include "StorageSvc/DbStatus.h"
#include "StorageSvc/DbDatabase.h"

// STL include files
#include <string>
#include <vector>
#include <utility>

class Guid;
class Token;

/*
 *      pool namespace declaration
 */
namespace pool  {

  // Forward declarations
  typedef class Session     *SessionH;
  typedef const class Shape *ShapeH;
  class FileDescriptor;
  class DbSelect;
  class DbOption;

  /** @class IStorageExplorer IStorageExplorer.h StorageSvc/IStorageExplorer.h
    *
    * The IStorageExplorer interface is able to handle user request for
    * - Accessing open databases in a domain.
    * - Accessing existing containers in a database.
    * - Accessing container level associations within a database.
    * - Accessing shapes known to a given database.
    * - Scan containers
    *
    * @author  Markus Frank
    * @version 1.0
    */
  class IStorageExplorer   {
  protected:
    /// Destructor (called only by sub-classes)
    virtual ~IStorageExplorer()   {     }

  public:
    /// Database parameter definition
    typedef std::pair<std::string, std::string>   Parameter;
    /// Database parameter container definition
    typedef std::vector< Parameter >              Parameters;

    /// Retrieve interface ID
    static const Guid& interfaceID();

    /// IInterface implementation: Query interfaces of Interface
    virtual DbStatus queryInterface(const Guid& riid, void** ppvUnkn) = 0;

    /// IInterface implementation: Reference Interface instance               
    virtual unsigned int addRef() = 0;

    /// IInterface implementation: Release Interface instance                 
    virtual unsigned int release() = 0;

    /// Start a new Database Session.
    /** The Database session handles Databases of one given type. This
      * involves specific handling of a given domain represented 
      * by a technology type. All subsequent actions involving Database
      * actions will re-use this technology identifier.
      * The session is a purely logical concept, which typically cannot fail
      * unless the underlying technology requires global initialization calls.
      *
      * @param    mode     [IN] Flag to indicate the accessmode of the session.
      *                         READ, NEW/CREATE/WRITE, UPDATE, RECREATE
      * @param    tech     [IN] Flag indicating the technology type of the
      *                         Database  the user  wants to connect to.
      * @param    session [OUT] Token or handle to the Database session.
      *                         This handle may later be used to open a
      *                         new Database connection.
      *
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus startSession(int                 mode,
                                  int                 tech,
                                  SessionH&           session) = 0;

    /// End the Database session.
    /** The  request to end a Database session requires, that all pending 
      * Transactions and connections are closed. Otherwise internally the
      * close will be forced and  potentially data on pending Transactions
      * will be lost. The token will be invalidated may not be used at any 
      * longer once the session ended.
      *
      * @param    session  [IN] Handle to the Database 
      *                         session. This handle was retrieved when 
      *                         starting the session. 
      *
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus endSession(  const SessionH           session) = 0;

    /// Connect to a logical Database unit.
    /** A connection is equivalent to the triple (OCISession, OCIServer, 
      * OCISvcCtx) in ORACLE, a login to a datasource using ODBC, or a 
      * single federation for Objectivity. For file based technologies, 
      * such as root, MS Access, ODBC/Text etc., this is involves the 
      * opening of the requested file.
      *
      * @param    sessionH [IN] Session context to be used to open the Database.
      * @param    mode     [IN] Flag to indicate the accessmode of the session.
      *                         READ, NEW/CREATE/WRITE, UPDATE, RECREATE.
      * @param    refDB   [I/O] Descriptor of the Database to be opened. 
      *                         On successful return the Database handle is
      *                         valid.
      *
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus connect(     const SessionH      sessionH, 
                                  int                 mode,
                                  FileDescriptor&     refDB) = 0;

    /// Disconnect from a logical Database unit.
    /** The  request for disconnect requires, that all pending Transactions
      * are already commited. Otherwise data are lost. On disconnection the 
      * access to the Database is finalized, closed. The token will be 
      * invalidated may not be used at any longer after disconnection.
      *
      * @param    refDB    [IN] Descriptor of the Database access. 
      *                         This handle was retrieved when connecting 
      *                         to the logical Database.
      *                         On successful return the Database handle will
      *                         be invalidated.
      *
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus disconnect(    FileDescriptor&     refDB) = 0;

    /** Access the size of the database: May be undefined for some technologies
      *
      * @param    refDB    [IN] Descriptor of the Database access. 
      *                         This handle was retrieved when connecting 
      *                         to the logical Database.
      *
      * @return                 Database size. std::string::npos in case of failure.
      */
    virtual long long int databaseSize(  FileDescriptor&     refDB)  const = 0;

    /// Access the containers in a given database.
    /** 
      *  @param   refDB     [IN] Reference to Database descriptor 
      *  @param   conts    [OUT] Vector with tokens to used containers.
      *  @param   intern    [IN] Flag to access also internal containers
      *  @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus containers(  const FileDescriptor&      refDB,
                                  std::vector<const Token*>& conts,
                                  bool                       intern=false) = 0;

    /// Access the container level associations between objects.
    /** 
      *  @param   refDB     [IN] Reference to Database descriptor 
      *  @param   assocs   [OUT] Vector with tokens to used containers.
      *  @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus associations(const FileDescriptor&      refDB,
                                  std::vector<const Token*>& assocs) = 0;

    /// Access Shapes known to the database.
    /** 
      *  @param   refDB     [IN] Reference to Database descriptor 
      *  @param   shapes   [OUT] Vector with shapes to used within the database.
      *  @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus shapes(      const FileDescriptor&      refDB,
                                  std::vector<ShapeH>&       shapes) = 0;

    /// Add a persistent parameter to the database
    /** Add a user specified parameter to the database.
      * The database must be open in CREATE or UPDATE mode.
      *
      *  @param   refDB     [IN] Reference to Database descriptor 
      *  @param   nam       [IN] Name of the user parameter to be added.
      *  @param   val       [IN] Value of the user parameter to be added.
      *  @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus addDbParam(  const FileDescriptor&      refDB,
                                  const std::string&         nam, 
                                  const std::string&         val) = 0;

    /// Retrieve existing parameter by name
    /** Retrieve single user specified parameter from the database.
      *
      *  @param   refDB     [IN] Reference to Database descriptor 
      *  @param   nam       [IN] Name of the user parameter to be retrieved.
      *  @param   val      [OUT] Value of the user parameter to be retrieved.
      *  @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus dbParam(     const FileDescriptor&      refDB,
                                  const std::string&         nam, 
                                  std::string&               val) = 0;

    /// Retrieve all parameters
    /** Retrieve all user parameters from the database.
      *
      *  @param   refDB     [IN] Reference to Database descriptor 
      *  @param   vals     [OUT] Vector containing all parameter nam/value pairs.
      *  @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus dbParams(    const FileDescriptor&      refDB,
                                  Parameters&                vals) = 0;

    /// Retrieve association link from token with redirection when file is merged.
    /** Retrieve all user parameters from the database.
      *
      *  @param   refDB        [IN]  Reference to Database descriptor
      *  @param   redirections [OUT] Vector containing all parameter nam/value pairs.
      *  @return                     DbStatus code indicating success or failure.
      */
    virtual DbStatus dbRedirections(const FileDescriptor& refDB,
                                    pool::DbDatabase::Redirections& redirections) = 0;

    /// Access options for a given database domain.
    /** Domain options are global options, which refer to the
      * database technology and not to a particular connection.
      *
      * Note: The options depend on the underlying implementation
      * and are not normalized.
      *
      *  @param   sessionH  [IN] Session context to be used to open the Database.
      *  @param   opt       [IN] Reference to option object.
      *
      *  @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus getDomainOption(const SessionH  sessionH, 
                                     DbOption&       opt) = 0;

    /// Set options for a given database domain.
    /** Domain options are global options, which refer to the
      * database technology and not to a particular connection.
      *
      * Note: The options depend on the underlying implementation
      * and are not normalized.
      *
      *  @param   sessionH  [IN] Session context to be used to open the Database.
      *  @param   opt       [IN] Reference to option object.
      *
      *  @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus setDomainOption(const SessionH  sessionH, 
                                     const DbOption& opt) = 0;

    /// Access options for a given database.
    /** Note: The options depend on the underlying implementation
      * and are not normalized.
      *
      *  @param   refDB     [IN] Reference to Database descriptor 
      *  @param   opt       [IN] Reference to option object.
      *
      *  @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus getDatabaseOption(const FileDescriptor& refDB,
                                       DbOption& opt) = 0;

    /// Set options for a given database.
    /** Note: The options depend on the underlying implementation
      * and are not normalized.
      *
      *  @param   refDB     [IN] Reference to Database descriptor 
      *  @param   opt       [IN] Reference to option object.
      *
      *  @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus setDatabaseOption(const FileDescriptor& refDB,
                                       const DbOption& opt) = 0;

    /// Access options for a given database container.
    /** Note: The options depend on the underlying implementation
      * and are not normalized.
      *
      *  @param   refDB     [IN] Reference to Database descriptor 
      *  @param   cntName   [IN] Name of the database container
      *  @param   opt       [IN] Reference to option object.
      *
      *  @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus getContainerOption(const FileDescriptor& refDB,
                                        const std::string& cntName,
                                        DbOption& opt) = 0;

    /// Set options for a given database container.
    /** Note: The options depend on the underlying implementation
      * and are not normalized.
      *
      *  @param   refDB     [IN] Reference to Database descriptor 
      *  @param   cntName   [IN] Name of the database container
      *  @param   opt       [IN] Reference to option object.
      *
      *  @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus setContainerOption(const FileDescriptor& refDB,
                                        const std::string& cntName,
                                        const DbOption& opt) = 0;

    /// Initiate a selection on a given container.
    /** The criteria according to which the selection is supposed
      * to be executed must be present in the passed criteria.
      * Empty criteria will select all entries in the container.
      *
      *  @param   refDB       [IN] Reference to Database descriptor 
      *  @param   cntName     [IN] Container name to be scanned.
      *  @param   sel     [IN/OUT] Selection criteria
      *  @return                   DbStatus code indicating success or failure.
      */
    virtual DbStatus select(      const FileDescriptor& refDB,
                                  const std::string&    cntName,
                                  DbSelect&             sel) = 0;

    /// Access the next token of a sequence
    /** The token to the next entry in the scanned container is
      * returned. it is up to the user to release the allocated
      * memory used by the token.
      *
      *  @param   context     [IN] Reference to selection context.
      *  @param   refpTok [IN/OUT] Location to store pointer to token
      *  @return                   DbStatus code indicating success or failure.
      */
    virtual DbStatus next(DbSelect& context, Token*& refpTok) = 0;
  };
}       // End namespace pool
#endif  // POOL_ISTORAGEEXPLORER_H
