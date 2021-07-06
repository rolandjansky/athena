/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Header: /cvs/PF/pool/StorageSvc/StorageSvc/IStorageSvc.h,v 1.33 2008/05/13 14:39:31 witoldp Exp $
#ifndef POOL_ISTORAGESVC_H
#define POOL_ISTORAGESVC_H

// Framework include files
#include "StorageSvc/DbStatus.h"
#include "StorageSvc/Transaction.h"

// STL include files
#include <string>

// Forward declarations

class Guid;
class Token;

/*
 *     pool namespace declaration
 */
namespace pool  {

  // Forward declarations
  class FileDescriptor;
  typedef class Session            *SessionH;
  typedef class DatabaseConnection *ConnectionH;
  typedef const class Shape        *ShapeH;

  /** @class IStorageSvc IStorageSvc.h StorageSvc/IStorageSvc.h
    *
    * The IStorageSvc interface is able to handle user request for
    *     - transient objects to become persistent and
    *     - persistent objects to be made trasient.
    *     What the storage manager however needs to perform this task 
    *     are object mappings, which translate between the two representations.
    *
    * The activity of the storage manager includes the Transaction handling and
    * hence the management of
    *     - Database sessions:
    *       The Database session handles Databases of one given type. This
    *       involves specific handling of a given domain represented
    *       by a technology type. 
    *     - Database connections: A connection is equivalent to the triple
    *       (OCISession, OCIServer, OCISvcCtx) in ORACLE, a login to a 
    *       datasource using ODBC, or a single federation for Objectivity.
    *       For file based technologies, such as root, MS Access, 
    *       ODBC/Text etc., this is involves the opening of the file.
    *     - Database Transactions: Start and end a Transaction.
    *
    * @author  Markus Frank
    * @version 1.0
    */
  class IStorageSvc   {
  protected:
    /// Destructor (called only by sub-classes)
    virtual ~IStorageSvc()   {     }

  public:
    /// Retrieve interface ID
    static const Guid& interfaceID();

    /// Retrieve category name
    static const char* category()             { return "pool_IStorageSvc"; }

    /// IInterface implementation: Query interfaces of Interface
    virtual DbStatus queryInterface(const Guid& riid, void** ppvUnkn) = 0;

    /// IInterface implementation: Reference Interface instance               
    virtual unsigned int addRef() = 0;

    /// IInterface implementation: Release Interface instance                 
    virtual unsigned int release() = 0;

    /// Get container name for object
    /**
      * @param   refDB     [IN] Reference to Database descriptor 
      * @param   pToken    [IN] Token to the persistent object.
      * @return                 std::string container name.
      */
    virtual std::string getContName(FileDescriptor& refDB,
                                    Token&          pToken) = 0;

    /// Register object for write
    /**
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
    virtual DbStatus allocate(    FileDescriptor&       refDB,
                                  const std::string&    refCont,
                                  int                   technology,
                                  const void*           object,
                                  ShapeH                shapeH,
                                  Token*&               refpTok) = 0;


    /// Read a persistent object from the medium.
    /** Reading an object does not create the object.
      *
      * @param   refDB     [IN] Reference to Database descriptor 
      * @param   pToken    [IN] Reference to persistent token information.
      * @param   object   [OUT] Pointer to persistent data pointer.
      * @param   shapeH    [IN] Desired object shape to be read
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus read(        const FileDescriptor& refDB,
                                  const Token&          pToken,
                                  ShapeH                shapeH,
                                  void**                object) = 0;

    /// In place update of an existing object.
    /**
      * @param   refDB     [IN] Reference to Database descriptor 
      * @param   object    [IN] Pointer to persistent data object.
      * @param   shapeH    [IN] Handle to persistent type information
      * @param   refToken  [IN] Reference to token containing the location
      *                         information of the persistent object.
      *
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus update(      FileDescriptor&       refDB,
                                  const void*           object,
                                  ShapeH                shapeH,
                                  Token&                refToken) = 0;

    /// Destroy an existing persistent object.
    /**
      * @param   refDB     [IN] Reference to Database descriptor 
      * @param   refToken  [IN] Reference to token containing the location
      *                         information of the persistent object.
      *
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus destroy(           FileDescriptor& refDB,
                                        Token&          refToken) = 0;

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
    virtual DbStatus getShape(    FileDescriptor&       refDB,
                                  const Guid&           objType,
                                  ShapeH&               shapeH) = 0;

    /// Create a persistent shape a given Database container.
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
                                  ShapeH&               shapeH) = 0;

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
                                      const FileDescriptor& refDB) = 0;

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
                                  int                 mode ) = 0;

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
    virtual DbStatus disconnect(  FileDescriptor&     refDB) = 0;

    /// Query the access mode of a Database unit.
    /** In order to reconnect, the database obviously must already be open.
      *
      * @param    refDB    [IN] Descriptor of the Database to be queried. 
      * @param    mode    [OUT] Open mod to the database.
      *
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus openMode(    FileDescriptor&     refDB,
                                  int&                mode ) = 0;

    /// End/Finish an existing Transaction sequence.
    /** At  this  phase all  objects, which were marked for  write when 
      * the Transaction was started, are either going to be made persistent
      * or scratched. After the Transaction ended, the Transaction context
      * is invalidated and may no longer be used independent wether the 
      * Transaction was successful or not.
      *
      * @param    conn  [IN]    Database connection
      * @param    typ   [IN]    Enum indicating an action to be performed.
      *                         Valid arguments are COMMIT and ROLLBACK.
      *                         ROLLBACK can only be suported if the 
      *                         underlying technology supports the necessary
      *                         features.
      *
      * @return                 DbStatus code indicating success or failure.
      */
    virtual DbStatus endTransaction( ConnectionH conn, Transaction::Action typ) = 0;

    /// DbStatus code definitions
    enum : unsigned {
      /// Error tag
      ISTORAGESVC_ERROR         = DbStatus::Error,
      /// Invalid Database session token
      INVALID_SESSION_TOKEN     = ISTORAGESVC_ERROR+2,
      /// Invalid Database connection token
      INVALID_CONNECTION_TOKEN  = ISTORAGESVC_ERROR+4,
      /// Invalid Database Transaction token
      INVALID_TRANSACTION_TOKEN = ISTORAGESVC_ERROR+6,
      /// Invalid Database authentication 
      INVALID_AUTHENTICATION    = ISTORAGESVC_ERROR+8,
      /// The Transaction failed
      TRANSACTION_FAILED        = ISTORAGESVC_ERROR+10,
      /// Object pointer is invalid
      INVALID_OBJECT            = ISTORAGESVC_ERROR+12,
      /// Invalid object address
      INVALID_OBJ_TOKEN         = ISTORAGESVC_ERROR+14,
      /// Sorry, the requested object is not loaded
      OBJ_NOT_LOADED            = ISTORAGESVC_ERROR+16,
      /// Database does not exist
      CONNECTION_NOT_EXISTING   = ISTORAGESVC_ERROR+18,
      /// Terminator
      ISTORAGESVC_LAST          = ISTORAGESVC_ERROR+20,
      /// Info tag
      ISTORAGESVC_INFO          = DbStatus::Info,
      /// Requested shape is unfortunately not availible
      SHAPE_NOT_AVAILIBLE       = ISTORAGESVC_INFO+2,
      /// Requested shape the persistent shape
      IS_PERSISTENT_SHAPE       = ISTORAGESVC_INFO+4
    };
  };

  // Factory function
  IStorageSvc* createStorageSvc(const std::string& componentName);
  
}       // End namespace pool
#endif  // POOL_ISTORAGESVC_H
