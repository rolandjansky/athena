/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENCYSVC_PLACEMENT_H
#define PERSISTENCYSVC_PLACEMENT_H

// Framework include files
#include "PersistencySvc/DatabaseSpecification.h"
#include "DataModelRoot/RootType.h"

namespace pool  {

  /** @class Placement Placement.h Placement/IContainer.h
   *
   *  Placement is a class holding all the necessary information
   *  to guide the writing of an object in a physical place.
   *
   */

  class Placement  {
  public:
    /// Default Constructor
    Placement();
    /// Constructor with initialization
    Placement( const std::string& dbName,
               DatabaseSpecification::NameType dbNameType,
               const std::string& container,
               const RootType& shapeID,
               long technology );

    /// Constructor with initialization
    Placement( const std::string& dbName,
               DatabaseSpecification::NameType dbNameType,
               const std::string& container,
               long technology );

    /// Set database name
    void setDatabase( const std::string& dbName,
		      DatabaseSpecification::NameType nameType );
    /// Access database identifier
    const std::string& databaseName() const;
    /// Access the database identifier type
    DatabaseSpecification::NameType databaseNameType() const;

    /// Set container name
    void setContainerName( const std::string& containerName );
    /// Access container identifier
    const std::string& containerName() const;

    /// Access database identifier
    void setShapeID( const RootType& id );
    /// Access database identifier
    const RootType& shapeID() const;

    /// Set technology type
    void setTechnology( long technology );
    /// Access technoliogy type
    long technology() const;

  private:
    /// Technology identifier
    long                  m_technology;
    /// Database identifier
    std::string           m_dbID;
    /// Container identifier
    std::string           m_contID;
    /// Object global identifier
    RootType              m_shapeID;
    /// Type of the database identifier
    DatabaseSpecification::NameType  m_dbNameType;
  };
}       // End namespace pool

#include "Placement.inl"

#endif
