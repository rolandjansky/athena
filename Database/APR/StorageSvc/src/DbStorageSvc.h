/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Header: /cvs/PF/pool/StorageSvc/src/DbStorageSvc.h,v 1.36 2008/03/04 18:34:59 frankb Exp $
#ifndef POOL_DBSTORAGESVC_H
#define POOL_DBSTORAGESVC_H

// Framework include files
#include "StorageSvc/DbSession.h"
#include "StorageSvc/DbDomain.h"
#include "StorageSvc/IStorageSvc.h"

/*
 *   POOL namespace declaration
 */
namespace pool  {

  // Forward declarations
  class DbStorageExplorer;
  class DbClassMap;
  class DbModule;
  class DbOption;
  class DbSelect;

  /** @class DbStorageSvc DbStorageSvc.h POOLCore/DbStorageSvc.h
    *
    * The DbStorageSvc class is able to handle user request for
    *     - transient objects to become persistent and
    *     - persistent objects to be made transient.
    *     What the storage manager however needs to perform this task 
    *     are object mappings, which translate between the two representations.
    *
    * This functionality is defined in the IDbStorageSvc interface and 
    * implemented in the DbStorageSvc class. Please refer to the header
    * file POOLCore/IDbStorageSvc for further details.
    *
    * @author  Markus Frank
    * @version 1.0
    */
  class DbStorageSvc  : virtual public IStorageSvc
  {
    typedef std::vector<const Token*> TokenVec;
  private:
    /// Service Name                               
    std::string         m_name;
    /// Reference counter                          
    unsigned int        m_refCount;
    /// Database session handle
    DbSession           m_sesH;
    /// Database domain handle
    DbDomain            m_domH;
    /// Property: AgeLimit indicating the maximal allowed age of files
    int                 m_ageLimit;
    /// Technology type
    DbType              m_type;
    /// Reference to storage explorer object
    DbStorageExplorer*  m_explorer;
    /// Service context
    void*               m_context;
  public:

    /// Standard Constructor: Constructs an object of type DbStorageSvc.
    DbStorageSvc();

    /// Initializing Constructor: Constructs an object of type DbStorageSvc.
    DbStorageSvc(void* ctxt, const std::string& name /*, int technology */);

    /// Standard destructor.
    virtual ~DbStorageSvc();

    DbStorageSvc (const DbStorageSvc&) = delete;
    DbStorageSvc& operator= (const DbStorageSvc&) = delete;

    /// Label of the specific class
    static const char* catalogLabel()  {   return "pool_DbStorageSvc";       }

    /// Database session handle
    DbSession& sessionHdl()                               {   return m_sesH;  }
    /// Database domain handle
    DbDomain& domainHdl()                                 {   return m_domH;  }

    /// IInterface implementation: Query interfaces of Interface
    virtual DbStatus queryInterface(const Guid& riid, void** ppvUnknown);

    /// IInterface implementation: Reference Interface instance               
    virtual unsigned int addRef();

    /// IInterface implementation: Release Interface instance                 
    virtual unsigned int release();

    /**@name IService interface                                   */

    /// IService implementation override: Initilize Service
    virtual DbStatus initialize();

    /// IService implementation override: Finalize Service     
    virtual DbStatus finalize();

    /// IService implementation: Retrieve name of the service               
    virtual const std::string& name() const    { return m_name;  }

    /**@name IStorageExplorer interface                           */

    /// Register object identified by its pointer with for write.
    /**
      * @param   trCxtH    [IN] Handle to the Transaction context.
      * @param   refDB     [IN] Reference to Database descriptor 
      * @param   refCont   [IN] Reference to container name 
      * @param   technology[IN] Specialised sub-technology
      * @param   object    [IN] Pointer to persistent data object.
      * @param   shapeH    [IN] Handle to persistent type information
      * @param   refpTok  [OUT] Reference to location for storing the
      *                         pointer of the persistent object token.
      *
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus allocate(      TransactionH          trCxtH,
                                    const FileDescriptor& refDB,
                                    const std::string&    refCont,
                                    int                   technology,
                                    const void*           object,
                                    ShapeH                shapeH,
                                    Token*&               refpTok);

    /// In place update of an existing object.
    /**
      * @param   trCxtH    [IN] Handle to the Transaction context.
      * @param   refDB     [IN] Reference to Database descriptor 
      * @param   object    [IN] Pointer to persistent data object.
      * @param   shapeH    [IN] Handle to persistent type information
      * @param   refToken  [IN] Reference to token containing the location
      *                         information of the persistent object.
      *
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus update(        TransactionH          trCxtH,
                                    const FileDescriptor& refDB,
                                    const void*           object,
                                    ShapeH                shapeH,
                                    const Token&          refToken);

    /// Destroy an existing persistent object.
    /**
      * @param   trCxtH    [IN] Handle to the Transaction context.
      * @param   refDB     [IN] Reference to Database descriptor 
      * @param   refToken  [IN] Reference to token containing the location
      *                         information of the persistent object.
      *
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus destroy(       TransactionH          trCxtH,
                                    const FileDescriptor& refDB,
                                    const Token&          refToken);

    /// Free a persistent object from the heap.
    /**
      * @param   pToken    [IN] Token to the persistent object.
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus free(          const Token&        pToken);

    /// Read a persistent object from the medium.
    /** Reading an object does not create the object.
      *
      * @param   refDB     [IN] Reference to Database descriptor 
      * @param   persToken [IN] Reference to persistent token information.
      * @param   object   [OUT] Pointer to persistent data pointer.
      * @param   shapeH    [IN] Desired object shape to be read
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus read(          const FileDescriptor& refDB,
                                    const Token&          persToken,
                                    ShapeH                shapeH,
                                    void**                object);

    /// Get container name for object
    /**
      * @param   refDB     [IN] Reference to Database descriptor
      * @param   pToken    [IN] Token to the persistent object.
      * @return                 std::string container name.
      */
    virtual std::string getContName(const FileDescriptor& refDB,
                                    const Token&          persToken);

    /// Retrieve persistent shape from Storage manager.
    /** The persistent shape is saved at write time to a Database.
      * To match the transient shape of an object to the persistent shape of 
      * the data at the time the data were written should allow for schema
      * evolution and object transformation(s).
      *
      * @param   refDB     [IN] Reference to Database descriptor 
      * @param   objType   [IN] Shape identifier.
      * @param   shapeH   [OUT] Handle to persistent mapping of a given
      *                         object type.
      *
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus getShape(    const FileDescriptor& refDB,
                                  const Guid&           objType,
                                  ShapeH&               shapeH);

    /// Create a persistent shape a given Database container location.
    /** This request involves Database actions. On creation of a 
      * shape the corresponding Database container is created.
      *
      * @param   refDB     [IN] Reference to Database descriptor 
      * @param   cntName   [IN] Shape/Container name.
      * @param   shapeID   [IN] Shape identifier.
      * @param   shapeH   [OUT] Handle to persistent mapping of a given
      *                         object type.
      *
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus createShape( const FileDescriptor& refDB,
                                  const std::string&    cntName,
                                  const Guid&           shapeID,
                                  ShapeH&               shapeH);

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
    virtual DbStatus endSession(  const SessionH       session);

    /// Check the existence of a logical Database unit.
    /** 
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
    virtual DbStatus existsConnection(const SessionH        sessionH, 
                                      int                   mode,
                                      const FileDescriptor& refDB);

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

    /// Reconnect to a logical Database unit with different access mode
    /** In order to reconnect, the database obviously must already be open.
      *
      * @param    refDB    [IN] Descriptor of the Database to be re-opened. 
      * @param    mode     [IN] Flag to indicate the accessmode of the session.
      *                         Since a database can only be re-opened if it 
      *                         exists, possible values may only be:
      *                         READ, UPDATE.
      *
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus reconnect(   FileDescriptor&     refDB,
                                  int                 mode );

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

    /// Query the access mode of a Database unit.
    /** In order to reconnect, the database obviously must already be open.
      *
      * @param    refDB    [IN] Descriptor of the Database to be queried. 
      * @param    mode    [OUT] Open mod to the database.
      *
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus openMode(    FileDescriptor&     refDB,
                                  int&                mode );

    /// Start a new Transaction sequence.
    /** When a new Transaction sequence is started, a new context is delivered
      * to the user. Using this context he will then start collecting objects
      * for persistency. After this excercise he will finally commit the
      * objects he collected during the open Transaction.
      *
      * @param    dbcH     [IN] Handle to the Database context containing all 
      *                         information necessary to access the 
      *                         proper Database. The Database handle was
      *                         retrieved when connecting to the Database.
      *
      * @param    refCtxt [OUT] Handle to a location where the pointer
      *                         of the Transaction context can be stored.
      *
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus startTransaction(const ConnectionH   dbcH,
                                      TransactionH&       refCtxt);

    /// End/Finish an existing Transaction sequence.
    /** At  this  phase all  objects, which were marked for  write when 
      * the Transaction was started, are either going to be made persistent
      * or scratched. After the Transaction ended, the Transaction context
      * is invalidated and may no longer be used independent wether the 
      * Transaction was successful or not.
      *
      * @param    ctxtH [IN]    Transaction context handle.
      * @param    typ   [IN]    Enum indicating an action to be performed.
      *                         Valid arguments are COMMIT and ROLLBACK.
      *                         ROLLBACK can only be suported if the 
      *                         underlying technology supports the necessary
      *                         features.
      *
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus endTransaction(TransactionH        ctxtH,
                                    Transaction::Action typ);

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
  };
}       // End namespace pool
#endif  // POOL_DBSTORAGESVC_H
