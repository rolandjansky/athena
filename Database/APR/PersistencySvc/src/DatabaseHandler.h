/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INCLUDE_PERSISTENCYSVC_DATABASEHANDLER_H
#define INCLUDE_PERSISTENCYSVC_DATABASEHANDLER_H

#include "StorageSvc/FileDescriptor.h"
#include "StorageSvc/DbDatabase.h"
#include <vector>
#include <set>
#include <utility>

class Guid;
class Token;

namespace pool {

  // forward declarations
  class Session;
  class Transaction;
  class IStorageSvc;
  class IStorageExplorer;
  class IContainer;

  namespace PersistencySvc {

    /** @class DatabaseHandler
     * 
     * DatabaseHandler is a class taking care of the micro-connections
     * and the micro transactions for a given database.
     * It also gives access to the underlying containers.
     */

    class DatabaseHandler {
    public:
      /// Constructor. Connects to the database
      DatabaseHandler( IStorageSvc& storageSvc,
                       IStorageExplorer& storageExplorer,
                       Session* session,
                       long technology,
                       const std::string& fid,
                       const std::string& pfn,
                       long accessmode );

      /// Destructor. Disconnects from the database
      ~DatabaseHandler();

      /// Starts the transaction
      bool startTransaction();

      /// Commits the transaction
      bool commitTransaction();

      /// Commits and holds the transaction
      bool commitAndHoldTransaction();

      /// Rolls back the transaction
      void rollBackTransaction();

      /// Checks if a transaction is active
      bool isTransactionActive() const;

      /// Returns the container name for object
      std::string getContName(const Token& token);

      /// Gives the list of containers
      std::vector< std::string > containers();

      /// Returns a container handle
      IContainer* container( const std::string& containerName );

      /// Returns the physical file name
      const std::string& pfn() const;

      /// Returns the file identifier
      const std::string& fid() const;

      /// Returns the merged file identifier
      pool::DbDatabase::Redirections redirections() const;

      /// Returns the technology identifier
      long technology() const;

      /// Returns the access mode
      long accessMode() const;

      /// Reconnects with a new access mode
      void reconnect( long accessMode );

      /// Writes an object and returns a token
      Token* writeObject( const std::string& containerName,
                          long minorTechnology,
                          const void* object,
                          const RootType& type );

      /// Reads an object given a token
      void* readObject( const Token& token, void* object = 0 );

      /// Deletes an object from the persistent store
      bool destroyObject( const Token& token );

      /// Returns an attrtibute
      bool attribute( const std::string& attributeName,
                      void* data,
                      const std::type_info& typeInfo,
                      const std::string& option ) const;

      /// Sets an attrtibute
      bool setAttribute( const std::string& attributeName,
                         const void* data,
                         const std::type_info& typeInfo,
                         const std::string& option );

      /// Returns the names of all the currently available parameters
      std::set< std::string > parameters() const;

      /// Returns the value of a parameter given its name. If the parameter does not exist an empty string is returned.
      std::string parameterValue( const std::string& name ) const;

      /// Adds a new parameter with a given value
      void addParameter( const std::string& name, const std::string& value );

    private:
      /// IStorageSvc reference
      IStorageSvc&      m_storageSvc;
      /// IStorageExplorer reference
      IStorageExplorer& m_storageExplorer;
      /// Pointer to the session for this database
      Session*          m_session;
      /// Micro transaction handler for this database
      Transaction*      m_transaction;
      /// File descriptor for this database
      FileDescriptor    m_fileDescriptor;
      /// Technology identifier
      long              m_technology;
      /// Current access mode
      long              m_accessMode;
    };
  }
}

#endif
