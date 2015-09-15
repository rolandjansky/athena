/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENCYSVC_ICONFIGURATION_H
#define PERSISTENCYSVC_ICONFIGURATION_H

#include "POOLCore/Context.h"
#include "PersistencySvc/DatabaseConnectionPolicy.h"

namespace pool {

  // forward declarations
  class IFileCatalog;

    /** @class IConfiguration
     *
     *  IConfiguration holds the file catalog reference,
     *  the token validator, the data transform and database connection policy objects,
     *  that are used in the various operations of the PersistencySvc classes.
     *
     */
  class IConfiguration : virtual public Context {
    public:
      /// Constructor. 
      IConfiguration();
      
      /// Destructor
      virtual ~IConfiguration(){}
      
      /** Sets the attached file catalog
       *
       *  @param catalog   [IN] Reference to the file catalog
       */
      virtual void setFileCatalog( IFileCatalog& catalog ) = 0;
      
      /// Retrieves the file catalog in use.
      virtual IFileCatalog& fileCatalog() = 0;
      virtual const IFileCatalog& fileCatalog() const = 0;
      
      /// Sets the database connection policy
      virtual void setDatabaseConnectionPolicy( const DatabaseConnectionPolicy& policy ) = 0;
      
      /// Returns the database connection policy.
      virtual const DatabaseConnectionPolicy& databaseConnectionPolicy() const = 0;
  };
}

inline
pool::IConfiguration::IConfiguration():Context(){
}

#endif
