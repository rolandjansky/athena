/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Header: /cvs/PF/pool/StorageSvc/src/DbStorageExplorer.h,v 1.16 2010/05/11 00:16:07 frankb Exp $
#ifndef POOL_DBSTORAGEEXPLORER_H
#define POOL_DBSTORAGEEXPLORER_H

// Framework include files
#include "StorageSvc/IStorageExplorer.h"

/*
 *  pool namespace declaration
 */
namespace pool  {

  // Forward declarations
  class DbStorageSvc;
  class DbDomain;
  class DbOption;

  /** @class DbStorageExplorer DbStorageExplorer.h StorageSvc/DbStorageExplorer.h
    *
    * The DbStorageExplorer class is able to handle user request for
    * accessing database information such as the container information and
    * to issue scans over database containers.
    *
    * This functionality is defined in the IStorageExplorer interface and 
    * implemented in the DbStorageExplorer class. Please refer to the header
    * file IStorageExplorer for further details.
    *
    * @author  Markus Frank
    * @version 1.0
    */
  class DbStorageExplorer  : virtual public IStorageExplorer
  {
    typedef std::vector<const Token*> TokenVec;

    /// Reference to outer interface
    DbStorageSvc*       m_pOuter;

    /// Reference to domain handle
    DbDomain&           m_domH;

    /// Explorer name 
    std::string         m_name;
    
  public:

    /// Standard Constructor: Constructs an object of type DbStorageExplorer.
    DbStorageExplorer();

    /// Initializing Constructor: Constructs an object of type DbStorageExplorer.
    /** Once a proper framework exists, most of these arguments will go away.
      */
    DbStorageExplorer(void*              framework, 
                      const std::string& name,
                      DbDomain&          domH,
                      DbStorageSvc*      pOuter);

    /// Standard destructor.
    virtual ~DbStorageExplorer();

    /**@name IStorageExplorer interface                           */

    /// IInterface implementation: Query interfaces of Interface
    virtual DbStatus queryInterface(const Guid& riid, 
                                    void** ppvUnknown);

    /// IInterface implementation: Reference Interface instance               
    virtual unsigned int addRef();

    /// IInterface implementation: Release Interface instance                 
    virtual unsigned int release();

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
                                  SessionH&           session);

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
    virtual DbStatus endSession(const SessionH       session);

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
                                  FileDescriptor&     refDB);

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
    virtual DbStatus disconnect(  FileDescriptor&  refDB);

    /** Access the size of the database: May be undefined for some technologies
      *
      * @param    refDB    [IN] Descriptor of the Database access. 
      *                         This handle was retrieved when connecting 
      *                         to the logical Database.
      *
      * @return                 Database size. std::string::npos in case of failure.
      */
    virtual long long int databaseSize(  FileDescriptor&     refDB)  const;

    /// Access the containers in a given database.
    /** 
      * @param   refDB     [IN] Reference to Database descriptor 
      * @param   conts    [OUT] Vector with tokens to used containers.
      * @param   intern    [IN] Flag to access also internal containers
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus containers(  const FileDescriptor& refDB,
                                  TokenVec&             conts,
                                  bool                  intern);

    /// Access the container level associations between objects.
    /** 
      * @param   refDB     [IN] Reference to Database descriptor 
      * @param   assocs   [OUT] Vector with tokens to used containers.
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus associations(const FileDescriptor& refDB,
                                  TokenVec&             assocs);

    /// Access Shapes known to the database.
    /** 
      * @param   refDB     [IN] Reference to Database descriptor 
      * @param   shapes   [OUT] Vector with shapes to used within the database.
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus shapes(      const FileDescriptor& refDB,
                                  std::vector<ShapeH>&  shapes);

    /// Add a persistent parameter to the database
    /** Add a user specified parameter to the database.
      * The database must be open in CREATE or UPDATE mode.
      *
      *  @param   refDB     [IN] Reference to Database descriptor 
      *  @param   nam       [IN] Name of the user parameter to be added.
      *  @param   val       [IN] Value of the user parameter to be added.
      *  @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus addDbParam(  const FileDescriptor& refDB,
                                  const std::string& nam, 
                                  const std::string& val);

    /// Retrieve existing parameter by name
    /** Retrieve single user specified parameter from the database.
      *
      *  @param   refDB     [IN] Reference to Database descriptor 
      *  @param   nam       [IN] Name of the user parameter to be retrieved.
      *  @param   val      [OUT] Value of the user parameter to be retrieved.
      *  @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus dbParam(     const FileDescriptor& refDB,
                                  const std::string& nam, 
                                  std::string& val);

    /// Retrieve all parameters
    /** Retrieve all user parameters from the database.
      *
      *  @param   refDB     [IN] Reference to Database descriptor 
      *  @param   vals     [OUT] Vector containing all parameter nam/value pairs.
      *  @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus dbParams(    const FileDescriptor& refDB,
                                  Parameters& vals);

    /// Retrieve association link from token with redirection when file is merged.
    /** Retrieve all user parameters from the database.
      *
      *  @param   refDB      [IN] Reference to Database descriptor 
      *  @param   redirections [OUT] Vector containing all parameter nam/value pairs.
      *  @return                     DbStatus code indicating success or failure.
      */
    virtual DbStatus dbRedirections(const FileDescriptor& refDB,
                                    pool::DbDatabase::Redirections& redirections);

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
                                     DbOption&       opt);

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
                                     const DbOption& opt);

    /// Access options for a given database.
    /** Note: The options depend on the underlying implementation
      * and are not normalized.
      *
      *  @param   refDB     [IN] Reference to Database descriptor 
      *  @param   opt       [IN] Reference to option object.
      *
      *  @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus getDatabaseOption( const FileDescriptor& refDB,
                                        DbOption& opt);

    /// Set options for a given database.
    /** Note: The options depend on the underlying implementation
      * and are not normalized.
      *
      *  @param   refDB     [IN] Reference to Database descriptor 
      *  @param   opt       [IN] Reference to option object.
      *
      *  @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus setDatabaseOption( const FileDescriptor& refDB,
                                        const DbOption& opt);

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
                                        DbOption& opt);

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
                                        const DbOption& opt);

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
                                  DbSelect&             sel);

    /// Access the next token of a sequence
    /** The token to the next entry in the scanned container is
      * returned. it is up to the user to release the allocated
      * memory used by the token.
      *
      *  @param   context     [IN] Reference to selection context.
      *  @param   refpTok [IN/OUT] Location to store pointer to token
      *  @return                   DbStatus code indicating success or failure.
      */
    virtual DbStatus next(DbSelect& context, Token*& refpTok);
  };
}       // End namespace pool
#endif  // POOL_DBSTORAGEEXPLORER_H
