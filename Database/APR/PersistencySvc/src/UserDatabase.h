/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INCLUDE_PERSISTENCYSVC_USERDATABASE_H
#define INCLUDE_PERSISTENCYSVC_USERDATABASE_H

#include "PersistencySvc/IDatabase.h"
#include "PersistencySvc/ISession.h"
#include "PersistencySvc/DatabaseSpecification.h"
#include "PersistencySvc/ITechnologySpecificAttributes.h"

namespace pool {
  // forward declarations
  class DatabaseConnectionPolicy;
  class ITransaction;

  namespace PersistencySvc {
    // forward declarations
    class TechnologyDispatcher;
    class DatabaseHandler;
    class DatabaseRegistry;

    /** @class UserDatabase
     *
     *  UserDatabase is an implementation of the IDatabase interface.
     *
     */
    class UserDatabase : virtual public IDatabase,
                         virtual public ITechnologySpecificAttributes
    {
    public:
      /// Constructor
      UserDatabase( TechnologyDispatcher& technologyDispatcher,
		    const DatabaseConnectionPolicy& policy,
		    IFileCatalog& fileCatalog,
		    ITransaction& transaction,
		    DatabaseRegistry& registry,
		    const std::string& name,
		    DatabaseSpecification::NameType nameType );

      /// Destructor
      ~UserDatabase();

      /// Returns the database handler
      DatabaseHandler& databaseHandler();

      /// Connects explicitly to the database for read operations
      void connectForRead();
      void connectForRead( const DatabaseConnectionPolicy& policy );

      /// Connects explicitly to the database for write/update operations
      void connectForWrite();
      void connectForWrite( const DatabaseConnectionPolicy& policy );

      /// Disconnects from the database
      void disconnect();

      /// Returns the opening mode. It can be used to check whether the database is connected.
      IDatabase::OpenMode openMode() const;

      /** Reopens in a different mode.
       *  If the database was in UPDATE mode before, then all changes since
       *  the last commit are aborted.
       */
      void revertMode( IDatabase::OpenMode mode );

      /// Returns the file identifier of this database
      const std::string& fid() const;

      /// Returns the physical file name of this database
      const std::string& pfn() const;

      /** Sets the technology identifier for this database.
       *  This can only be called for newly created databases
       *  before the connect method is called. Otherwise false is returned.
       */
      bool setTechnology( long technology );

      /// Returns the technology identifier for this database
      long technology() const;

      /// Returns the names of the containers in this database
      std::vector< std::string > containers();

      /// Returns a pointer to a container object. The user acquires ownership of that object.
      IContainer* containerHandle( const std::string& name );

      /// Returns the object holding the technology specific attributes
      const ITechnologySpecificAttributes& technologySpecificAttributes() const;
      ITechnologySpecificAttributes& technologySpecificAttributes();

    protected:
      bool attributeOfType( const std::string& attributeName,
                            void* data,
                            const std::type_info& typeInfo,
                            const std::string& option ) const;

      bool setAttributeOfType( const std::string& attributeName,
                               const void* data,
                               const std::type_info& typeInfo,
                               const std::string& option );
    private:
      /// Reference to the technology dispatcher
      TechnologyDispatcher&                   m_technologyDispatcher;
      /// Reference to the policy
      const DatabaseConnectionPolicy&         m_policy;
      /// Reference to the file catalog
      IFileCatalog&                           m_catalog;
      /// Reference to the global transaction
      ITransaction&                           m_transaction;
      /// Reference to the database registry
      DatabaseRegistry&                       m_registry;
      /// The database name
      mutable std::string                     m_name;
      /// The database name spacification
      mutable DatabaseSpecification::NameType m_nameType;
      /// The technology identifier of the database
      mutable long                            m_technology;
      /// Checks if the technology identifier has been set
      mutable bool                            m_technologySet;
      /// The underlying database handler
      DatabaseHandler*                        m_databaseHandler;
      /// Current open mode
      IDatabase::OpenMode                     m_openMode;
      /// Flag indicating whether a connection has been already made once
      mutable bool                            m_alreadyConnected;
      /// Other names used.
      mutable std::string                     m_the_fid;
      mutable std::string                     m_the_pfn;

      /// Checks in the registry if the database handler already exists
      bool checkInRegistry();

      /// Converts a technology string to the technology (long) identifier
      void setTechnologyIdentifier( const std::string& sTechnology ) const;
    };
  }
}

#endif
