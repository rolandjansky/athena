/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
  class IFileCatalog;
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
      virtual ~UserDatabase();

      /// Returns the database handler
      DatabaseHandler& databaseHandler();

      /// Connects explicitly to the database for read operations
      virtual void connectForRead() override;
      virtual void connectForRead( const DatabaseConnectionPolicy& policy ) override;

      /// Connects explicitly to the database for write/update operations
      virtual void connectForWrite() override;
      virtual void connectForWrite( const DatabaseConnectionPolicy& policy ) override;

      /// Disconnects from the database
      virtual void disconnect() override;

      /// Returns the opening mode. It can be used to check whether the database is connected.
      virtual IDatabase::OpenMode openMode() const override;

      /// Returns the file identifier of this database
      virtual const std::string& fid() override;

      /// Returns the physical file name of this database
      virtual const std::string& pfn() override;

      /** Sets the technology identifier for this database.
       *  This can only be called for newly created databases
       *  before the connect method is called. Otherwise false is returned.
       */
      virtual bool setTechnology( long technology ) override;

      /// Returns the technology identifier for this database
      virtual long technology() const override;

      /// Returns the names of the containers in this database
      virtual std::vector< std::string > containers() override;

      /// Returns a pointer to a container object. The user acquires ownership of that object.
      virtual IContainer* containerHandle( const std::string& name ) override;

      /// Returns the object holding the technology specific attributes
      virtual const ITechnologySpecificAttributes& technologySpecificAttributes() const override;
      virtual ITechnologySpecificAttributes& technologySpecificAttributes() override;

    protected:
      virtual
      bool attributeOfType( const std::string& attributeName,
                            void* data,
                            const std::type_info& typeInfo,
                            const std::string& option ) override;

      virtual
      bool setAttributeOfType( const std::string& attributeName,
                               const void* data,
                               const std::type_info& typeInfo,
                               const std::string& option ) override;
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
      std::string                             m_name;
      /// The database name spacification
      DatabaseSpecification::NameType         m_nameType;
      /// The technology identifier of the database
      long                                    m_technology;
      /// Checks if the technology identifier has been set
      bool                                    m_technologySet;
      /// The underlying database handler
      DatabaseHandler*                        m_databaseHandler;
      /// Current open mode
      IDatabase::OpenMode                     m_openMode;
      /// Flag indicating whether a connection has been already made once
      bool                                    m_alreadyConnected;
      /// Other names used.
      std::string                             m_the_fid;
      std::string                             m_the_pfn;

      /// Checks in the registry if the database handler already exists
      bool checkInRegistry();

      /// Converts a technology string to the technology (long) identifier
      void setTechnologyIdentifier( const std::string& sTechnology );
    };
  }
}

#endif
