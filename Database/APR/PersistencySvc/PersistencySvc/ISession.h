/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INCLUDE_PERSISTENCYSVC_ISESSION_H
#define INCLUDE_PERSISTENCYSVC_ISESSION_H

#include <string>
#include <vector>
#include "PersistencySvc/DatabaseSpecification.h"

namespace pool {

  // forward declarations
  class DatabaseConnectionPolicy;
  class ITransaction;
  class IDatabase;
  class ITechnologySpecificAttributes;
  class IFileCatalog;

  /** @class ISession ISession.h PersistencySvc/ISession.h
   *
   *  ISession is the interface class for user (macroscopic) sessions
   *  Every transaction and connection to a database can be performed
   *  within a session. It therefore defines a logical context within
   *  which all the operations are performed.
   *  It further provides access to the file catalog and
   *  the shape transformation registry in use.
   */

  class ISession {
  protected:
  public:
    /// Default destructor
    virtual ~ISession() {}

    /// Sets the default policy when databases are opened/connected
    virtual void setDefaultConnectionPolicy( const DatabaseConnectionPolicy& policy ) = 0;

    /// Retrieves the default connection policy
    virtual const DatabaseConnectionPolicy& defaultConnectionPolicy() const = 0;

    /** Explicitly disconnects all the databases.
     *  If a transaction is active, then all the changes since the last commit are aborted.
     */
    virtual void disconnectAll() = 0;

    /// Returns the transaction object
    virtual ITransaction& transaction() = 0;
    virtual const ITransaction& transaction() const = 0;

    /// Returns a vector with the file identifiers of the presently open databases.
    virtual std::vector< std::string > connectedDatabases() const = 0;

    /// Returns a pointer to a database object. The user acquires ownership of that object.
    virtual IDatabase* databaseHandle( const std::string& dbName,
                                       DatabaseSpecification::NameType dbNameType ) = 0;

    /// Returns the file catalog in use
    virtual IFileCatalog& fileCatalog() = 0;

    /// Returns the object holding the technology specific attributes for a given technology domain
    virtual const ITechnologySpecificAttributes& technologySpecificAttributes( long technology ) const = 0;
    virtual ITechnologySpecificAttributes& technologySpecificAttributes( long technology ) = 0;
  };

}

#endif
