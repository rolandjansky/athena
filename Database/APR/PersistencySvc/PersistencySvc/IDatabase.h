/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INCLUDE_PERSISTENCYSVC_IDATABASE_H
#define INCLUDE_PERSISTENCYSVC_IDATABASE_H

// includes
#include <string>
#include <vector>

namespace pool {

  // forward declarations
  class DatabaseConnectionPolicy;
  class IContainer;
  class ITechnologySpecificAttributes;
  class IDatabaseParameters;

  /** @class IDatabase IDatabase.h PersistencySvc/IDatabase.h
   *
   *  IDatabase is the base class for database objects
   *
   */

  class IDatabase {
  public:
    /// Current open mode enumeration
    typedef enum { CLOSED,
		   READ,
		   UPDATE } OpenMode;

    /// Empty destructor
    virtual ~IDatabase() {};

    /// Connects explicitly to the database for read operations
    virtual void connectForRead() = 0;
    virtual void connectForRead( const DatabaseConnectionPolicy& policy ) = 0;

    /// Connects explicitly to the database for write/update operations
    virtual void connectForWrite() = 0;
    virtual void connectForWrite( const DatabaseConnectionPolicy& policy ) = 0;

    /// Disconnects from the database
    virtual void disconnect() = 0;

    /// Returns the opening mode. It can be used to check whether the database is connected.
    virtual OpenMode openMode() const = 0;

    /** Reopens in a different mode.
     *  If the database was in UPDATE mode before, then all changes since
     *  the last commit are aborted.
     */
    virtual void revertMode( OpenMode mode ) = 0;

    /// Returns the file identifier of this database
    virtual const std::string& fid() const = 0;

    /// Returns the physical file name of this database
    virtual const std::string& pfn() const = 0;

    /** Sets the technology identifier for this database.
     *  This can only be called for newly created databases
     *  before the connect method is called. Otherwise false is returned.
     */
    virtual bool setTechnology( long technology ) = 0;

    /// Returns the technology identifier for this database
    virtual long technology() const = 0;

    /// Returns the names of the containers in this database
    virtual std::vector< std::string > containers() = 0;

    /// Returns a pointer to a container object. The user acquires ownership of that object.
    virtual IContainer* containerHandle( const std::string& name ) = 0;

    /// Returns the object holding the technology specific attributes
    virtual const ITechnologySpecificAttributes& technologySpecificAttributes() const = 0;
    virtual ITechnologySpecificAttributes& technologySpecificAttributes() = 0;

    /// Returns the object holding the database parameters
    virtual const IDatabaseParameters& parameters() const = 0;
    virtual IDatabaseParameters& parameters() = 0;
  };

}

#endif
