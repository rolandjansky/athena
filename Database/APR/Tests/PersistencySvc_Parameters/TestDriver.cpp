/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TestDriver.h"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <memory>


#include "StorageSvc/DbType.h"

#include "FileCatalog/URIParser.h"
#include "FileCatalog/IFileCatalog.h"

#include "PersistencySvc/ISession.h"
#include "PersistencySvc/ITransaction.h"
#include "PersistencySvc/DatabaseConnectionPolicy.h"
#include "PersistencySvc/IDatabase.h"
#include "PersistencySvc/IDatabaseParameters.h"
#include "PersistencySvc/IPersistencySvc.h"


int numTokenInstances();

pool::TestDriver::TestDriver():
  m_fileCatalog( 0 ),
  m_fileName( "pool.root" ),
  m_parameters()
{
  
  std::cout << "[OVAL] Creating a file catalog" << std::endl;
  m_fileCatalog = new pool::IFileCatalog;
  if ( ! m_fileCatalog ) {
    throw std::runtime_error( "Could not create a file catalog" );
  }
  pool::URIParser p;
  p.parse();
  m_fileCatalog->setWriteCatalog( p.contactstring() );
  m_fileCatalog->connect();
}

pool::TestDriver::~TestDriver()
{
  if ( m_fileCatalog ) delete m_fileCatalog;
  std::cout << "[OVAL] Number of floating tokens : " << numTokenInstances() << std::endl;
}

void
pool::TestDriver::write()
{
  pool::IFileCatalog& catalog = *m_fileCatalog;
  catalog.start();

  std::cout << "Creating the persistency service" << std::endl;
  std::unique_ptr< pool::IPersistencySvc > persistencySvc( pool::IPersistencySvc::create(catalog) );

  // Set up the policy.
  pool::DatabaseConnectionPolicy policy;
  policy.setWriteModeForNonExisting( pool::DatabaseConnectionPolicy::CREATE );
  policy.setWriteModeForExisting( pool::DatabaseConnectionPolicy::OVERWRITE );
  persistencySvc->session().setDefaultConnectionPolicy( policy );

  // Start an update transaction
  if ( ! ( persistencySvc->session().transaction().start( pool::ITransaction::UPDATE ) ) ) {
    throw std::runtime_error( "Could not start an update transaction" );
  }

  // Opening again a database
  pool::IDatabase* db = persistencySvc->session().databaseHandle( m_fileName, pool::DatabaseSpecification::PFN );
  if ( ! db ) {
    throw std::runtime_error( "Could not retrieve a database handle" );
  }

  db->setTechnology( pool::ROOT_StorageType.type() );
  db->connectForWrite();

  // Retrieving the existing parameters
  pool::IDatabaseParameters& databaseParameters = db->parameters();
  std::set< std::string > parameterNames = databaseParameters.parameterNames();
  for ( std::set< std::string >::const_iterator iParameterName = parameterNames.begin();
	iParameterName != parameterNames.end(); ++iParameterName ) {
    m_parameters.insert( std::make_pair( *iParameterName, databaseParameters.value( *iParameterName ) ) );
  }

  // adding a new parameter
  databaseParameters.addParameter( std::string( "newParameterName" ), std::string( "newParameterValue" ) );
  m_parameters.insert( std::make_pair( std::string( "newParameterName" ), std::string( "newParameterValue" ) ) );

  // Committing the transaction
  std::cout << "Committing the transaction." << std::endl;
  if ( ! persistencySvc->session().transaction().commit() ) {
    throw std::runtime_error( "Could not commit the transaction." );
  }

  catalog.commit();
  delete db;
}


void
pool::TestDriver::read()
{
  pool::IFileCatalog& catalog = *m_fileCatalog;
  catalog.start();

  std::cout << "Creating the persistency service" << std::endl;
  std::unique_ptr< pool::IPersistencySvc > persistencySvc( pool::IPersistencySvc::create(catalog) );

  // Starting a read transaction
  if ( ! persistencySvc->session().transaction().start( pool::ITransaction::READ ) ) {
    throw std::runtime_error( "Could not start a read transaction." );
  }

  // Opening the database for reading
  pool::IDatabase* db = persistencySvc->session().databaseHandle( m_fileName, pool::DatabaseSpecification::PFN );
  if ( ! db ) {
    throw std::runtime_error( "Could not retrieve a database handle" );
  }

  db->connectForRead();

  // Retrieve the database parameters
  const pool::IDatabaseParameters& databaseParameters = db->parameters();
  std::set< std::string > parameterNames = databaseParameters.parameterNames();
  std::map< std::string, std::string > retrievedParameters;
  for ( std::set< std::string >::const_iterator iParameterName = parameterNames.begin();
	iParameterName != parameterNames.end(); ++iParameterName ) {
    retrievedParameters.insert( std::make_pair( *iParameterName, databaseParameters.value( *iParameterName ) ) );
  }

  // Check if the number of parameters is the expected one
  if ( retrievedParameters.size() != m_parameters.size() ) {
    std::ostringstream error;
    error << "Retrieved " << retrievedParameters.size() << " parameters instead of " << m_parameters.size();
    throw std::runtime_error( error.str() );
  }

  // Check the parameters that I expect to be existing and their values
  for ( std::map< std::string, std::string >::const_iterator iParameter = m_parameters.begin();
	iParameter != m_parameters.end(); ++iParameter ) {
    const std::string& parameterName = iParameter->first;
    std::map< std::string, std::string >::const_iterator iRetrievedParameter = retrievedParameters.find( parameterName );
    if ( iRetrievedParameter == retrievedParameters.end() ) {
      throw std::runtime_error( "Parameter \"" + parameterName + "\" not found in the database file" );
    }
    const std::string& parameterValue = iParameter->second;
    const std::string& retrievedParameterValue = iRetrievedParameter->second;
    if ( retrievedParameterValue != parameterValue ) {
      throw std::runtime_error( "Parameter \"" + parameterName + "\" has the value \"" + retrievedParameterValue + "\" instead of \"" + parameterValue + "\"" );
    }
    if ( parameterName != "FID" )
      std::cout << "[OVAL] ";
    else
      std::cout << "       ";
    std::cout << "\"" << parameterName << "\" : \"" << parameterValue << "\"" << std::endl;
  }

  // Committing the transaction
  std::cout << "Committing the transaction." << std::endl;
  if ( ! persistencySvc->session().transaction().commit() ) {
    throw std::runtime_error( "Could not commit the transaction." );
  }

  catalog.commit();
  delete db;
}
