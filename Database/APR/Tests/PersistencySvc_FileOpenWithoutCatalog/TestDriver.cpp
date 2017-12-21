/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TestDriver.h"
#include "libname.h"

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <memory>

#include "PersistentDataModel/Placement.h"
#include "PersistentDataModel/Token.h"

#include "StorageSvc/DbType.h"

#include "FileCatalog/URIParser.h"
#include "FileCatalog/IFileCatalog.h"

#include "PersistencySvc/ISession.h"
#include "PersistencySvc/ITransaction.h"
#include "PersistencySvc/DatabaseConnectionPolicy.h"
#include "PersistencySvc/IDatabase.h"
#include "PersistencySvc/IContainer.h"
#include "PersistencySvc/ITokenIterator.h"
#include "PersistencySvc/IPersistencySvc.h"

int numTokenInstances();

pool::TestDriver::TestDriver():
  m_fileCatalog( 0 ),
  m_fileName( "pool.root" )
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

  // Retrieving the class descriptions
  RootType class_SimpleTestClass( "SimpleTestClass" );

  // Defining the placement objects
  Placement placementHint_SimpleTestClass;
  placementHint_SimpleTestClass.setFileName( m_fileName );
  placementHint_SimpleTestClass.setContainerName( "SimpleTestClass_Container" );
  placementHint_SimpleTestClass.setTechnology( pool::ROOTKEY_StorageType.type() );

  std::vector< SimpleTestClass* > v_simpleTestClass;

  std::cout << "Writing 100 objects." << std::endl;
  for ( int i = 0; i < 100; ++i ) {
    SimpleTestClass* object_SimpleTestClass = new SimpleTestClass();
    v_simpleTestClass.push_back( object_SimpleTestClass );
    object_SimpleTestClass->setNonZero();
    Token* token_SimpleTestClass = persistencySvc->registerForWrite( placementHint_SimpleTestClass,
									   object_SimpleTestClass,
									   class_SimpleTestClass );
    if ( ! token_SimpleTestClass ) {
      throw std::runtime_error( "Could not write an object" );
    }
    token_SimpleTestClass->release();
    m_simpleTestClass.push_back( *object_SimpleTestClass );
  }


  // Committing the transaction
  std::cout << "Committing the transaction." << std::endl;
  if ( ! persistencySvc->session().transaction().commit() ) {
    throw std::runtime_error( "Could not commit the transaction." );
  }

  // Removing the "cache"
  for ( std::vector< SimpleTestClass* >::iterator iObject = v_simpleTestClass.begin();
	iObject != v_simpleTestClass.end(); ++iObject ) {
    delete *iObject;
  }

  // Removing the entries from the catalog
  catalog.rollback();
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

  // Opening a database
  pool::IDatabase* db = persistencySvc->session().databaseHandle( m_fileName, pool::DatabaseSpecification::PFN );
  if ( ! db ) {
    throw std::runtime_error( "Could not retrieve a database handle" );
  }
  db->setTechnology( pool::ROOT_StorageType.type() );
  db->connectForRead();

  std::vector< std::string > containers = db->containers();
  for ( std::vector< std::string >::const_iterator iContainer = containers.begin();
	iContainer != containers.end(); ++iContainer ) {
    pool::IContainer* container = db->containerHandle( *iContainer );
    if ( ! container ) {
      throw std::runtime_error( "Could not retrieve the container" );
    }
    std::cout << "Container : " << container->name() << std::endl;
    pool::ITokenIterator* tokenIterator = container->tokens( "" );
    if ( ! tokenIterator ) {
      throw std::runtime_error( "Could not obtain a token iterator" );
    }

    int numberOfObjects = 0;
    Token* token = tokenIterator->next();
    while ( token ) {
      ++numberOfObjects;
      token->release();
      token = tokenIterator->next();
    }

    if ( numberOfObjects != 100 ) {
      throw std::runtime_error( "Unexpected number of objects in that container." );
    }
    else {
      std::cout << numberOfObjects << " objects found in the container." << std::endl;
    }

    delete tokenIterator;
    delete container;
  }

  delete db;

  std::cout << "Committing the transaction." << std::endl;
  if ( ! persistencySvc->session().transaction().commit() ) {
    throw std::runtime_error( "Could not commit the transaction." );
  }

  catalog.commit();
}
