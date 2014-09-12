/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INCLUDE_PERSISTENCYSVC_DATABASECONNECTIONPOLICY_H
#define INCLUDE_PERSISTENCYSVC_DATABASECONNECTIONPOLICY_H


namespace pool {

  /** @class DatabaseConnectionPolicy DatabaseConnectionPolicy.h PersistencySvc/DatabaseConnectionPolicy.h
   *
   *  DatabaseConnectionPolicy is a class holding information on the policy to
   *  be followed whenever a database is opened under different situations
   *  in terms of transaction context and database existence.
   *
   */

  class DatabaseConnectionPolicy {
  public:
    /// Opening modes for a database
    typedef enum { RAISE_ERROR,
		   CREATE,
		   OVERWRITE,
		   UPDATE,
		   READ } Mode;

    /// Default constructor. Initializes the data members with their default values
    DatabaseConnectionPolicy();

    /// Destructor
    ~DatabaseConnectionPolicy() {};

    /// Copy constructor
    DatabaseConnectionPolicy( const DatabaseConnectionPolicy& rhs );

    /// Assignment operator
    DatabaseConnectionPolicy& operator=( const DatabaseConnectionPolicy& rhs );

    /// Retrieves the opening mode when an existing database is opened for writing
    Mode writeModeForExisting() const;

    /** Sets the opening mode when an existing database is opened for writing.
     *  Acceptable values are UPDATE (default), OVERWRITE and RAISE_ERROR.
     *  Returns false if any other value is passed, true otherwise.
     */
    bool setWriteModeForExisting( Mode mode );

    /// Retrieves the opening mode when a non existing database is opened for writing
    Mode writeModeForNonExisting() const;

    /** Sets the opening mode when a non existing database is opened for writing
     *  Acceptable values are RAISE_ERROR (default), CREATE and UPDATE.
     *  Returns false if any other value is passed, true otherwise.
     */
    bool setWriteModeForNonExisting( Mode mode );

    /// Retrieves the opening mode of a database in an update transaction, when opened for reading.
    Mode readMode() const;

    /** Sets the opening mode of a database in an update transaction, when opened for reading.
     *  Acceptable values are READ (default) and UPDATE.
     *  Returns false if any other value is passed, true otherwise.
     */
    bool setReadMode( Mode mode );

  private:
    /// The opening mode when an existing database is opened for writing. Default is UPDATE
    Mode m_writeModeForExisting;

    /// The opening mode when a non existing database is opened for writing. Default is RAISE_ERROR
    Mode m_writeModeForNonExisting;

    /// The opening mode of a database in an update transaction, when opened for reading. Default is READ
    Mode m_readMode;
  };

}

#include "DatabaseConnectionPolicy.inl"

#endif
