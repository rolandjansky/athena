/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INCLUDE_PERSISTENCYSVC_PERSISTENCYSVCCONFIGURATION_H
#define INCLUDE_PERSISTENCYSVC_PERSISTENCYSVCCONFIGURATION_H

#include "PersistencySvc/IConfiguration.h"

namespace pool {

  // forward declarations
  class IFileCatalog;

  namespace PersistencySvc {
    /** @class PersistencySvcConfiguration
     *
     *  PersistencySvcConfiguration holds the file catalog reference,
     *  the token validator, the data transform and database connection policy objects,
     *  that are used in the various operations of the PersistencySvc classes.
     *
     */
    class PersistencySvcConfiguration : virtual public IConfiguration {
    public:
      /// Constructor. Creates a dummy token validator and a default data transform
      PersistencySvcConfiguration();
      
      /// Destructor
      virtual ~PersistencySvcConfiguration();
      
      /** Sets the attached file catalog
       *
       *  @param catalog   [IN] Reference to the file catalog
       */
      void setFileCatalog( IFileCatalog& catalog );
      
      /// Retrieves the file catalog in use.
      IFileCatalog& fileCatalog();
      const IFileCatalog& fileCatalog() const;
      
      /// Sets the database connection policy
      void setDatabaseConnectionPolicy( const DatabaseConnectionPolicy& policy );
      
      /// Returns the database connection policy.
      const DatabaseConnectionPolicy& databaseConnectionPolicy() const;

    private:
      /// The file catalog
      IFileCatalog* m_fileCatalog;
      /// The default database connection policy
      DatabaseConnectionPolicy m_policy;
    };
  }
}

#endif
