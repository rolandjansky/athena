/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INCLUDE_PERSISTENCYSVC_USERSESSION_H
#define INCLUDE_PERSISTENCYSVC_USERSESSION_H

#include "PersistencySvc/ISession.h"

namespace pool {

  class DatabaseConnectionPolicy;

  namespace PersistencySvc {

    // forward declarations
    class DatabaseRegistry;
    class TechnologyDispatcher;
    class GlobalTransaction;

    /** @class UserSession
     *
     *  UserSession is an implementation of the ISession interface
     *
     */

    class UserSession : virtual public ISession
    {
    public:
      /// Constructor
      UserSession( IFileCatalog& fileCatalog );

      /// Destructor
      virtual ~UserSession();

      UserSession (const UserSession&) = delete;
      UserSession& operator= (const UserSession&) = delete;

      // Signatures needed for the PersistencySvc
      DatabaseRegistry& registry();
      TechnologyDispatcher& technologyDispatcher();

      /// Sets the default policy when databases are opened/connected
      void setDefaultConnectionPolicy( const DatabaseConnectionPolicy& policy );

      /// Retrieves the default connection policy
      const DatabaseConnectionPolicy& defaultConnectionPolicy() const;

      /** Explicitly disconnects all the databases.
       *  If a transaction is active, then all the changes since the last commit are aborted.
       */
      void disconnectAll();

      /// Returns the transaction object
      ITransaction& transaction();
      const ITransaction& transaction() const;

      /// Returns a vector with the file identifiers of the presently open databases.
      std::vector< std::string > connectedDatabases() const;
      
      /// Returns a pointer to a database object. The user acquires ownership of that object.
      IDatabase* databaseHandle( const std::string& dbName,
				 DatabaseSpecification::NameType dbNameType );

      /// Returns the file catalog in use
      IFileCatalog& fileCatalog();

      /// Set the file catalog to be used
      void setFileCatalog(IFileCatalog& catalog);

      /// Returns the object holding the technology specific attributes for a given technology domain
      const ITechnologySpecificAttributes& technologySpecificAttributes( long technology ) const;
      ITechnologySpecificAttributes& technologySpecificAttributes( long technology );

      /// Returns the global transaction object
      ITransaction& globalTransaction();
      
    private:
      DatabaseConnectionPolicy*      m_policy;
      IFileCatalog*                  m_catalog;
      DatabaseRegistry*              m_registry;
      GlobalTransaction*             m_transaction;
      TechnologyDispatcher*          m_technologyDispatcher;
    };
  }
}

#endif
