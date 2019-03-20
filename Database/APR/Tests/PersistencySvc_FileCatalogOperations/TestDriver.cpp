/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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


pool::TestDriver::TestDriver():
  m_fileCatalog( 0 ),
  m_fileName1( "pool1.root" ),
  m_lfn1( "lfn1" ),
  m_fileName2( "pool2.root" ),
  m_lfn2( "lfn2" ),
  m_fileName3( "pool3.root" ),
  m_lfn3( "lfn3" ),
  m_events( 100 )
{
  std::cout << "[OVAL] Creating a file catalog" << std::endl;
  m_fileCatalog = new pool::IFileCatalog;
  pool::URIParser p;
  p.parse();
  m_fileCatalog->setWriteCatalog( p.contactstring() );
  m_fileCatalog->connect();
}

pool::TestDriver::~TestDriver()
{
  if ( m_fileCatalog ) delete m_fileCatalog;
  std::cout << "[OVAL] Number of floating tokens : " << Token::numInstances() << std::endl;
}


void
pool::TestDriver::write()
{
  pool::IFileCatalog& catalog = *m_fileCatalog;

  std::cout << "Creating the persistency service" << std::endl;
  std::unique_ptr< pool::IPersistencySvc > persistencySvc( pool::IPersistencySvc::create(catalog) );
  catalog.start();

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
  placementHint_SimpleTestClass.setFileName( m_fileName1 );
  placementHint_SimpleTestClass.setContainerName( "SimpleTestClass_Container" );
  placementHint_SimpleTestClass.setTechnology( pool::ROOTKEY_StorageType.type() );

  std::vector< SimpleTestClass* > v_simpleTestClass;
  std::cout << "Writing " << m_events << " objects" << std::endl;
  for ( int i = 0; i < m_events; ++i ) {

    ///////////////////////////////////////////////////////////
    SimpleTestClass* object_SimpleTestClass = new SimpleTestClass();
    v_simpleTestClass.push_back( object_SimpleTestClass );
    object_SimpleTestClass->data = i;
    Token* token_SimpleTestClass = persistencySvc->registerForWrite( placementHint_SimpleTestClass,
									   object_SimpleTestClass,
									   class_SimpleTestClass );
    if ( ! token_SimpleTestClass ) {
      throw std::runtime_error( "Could not write an object" );
    }
    token_SimpleTestClass->release();
  }

  std::cout << "Committing the transaction." << std::endl;
  if ( ! persistencySvc->session().transaction().commit() ) {
    throw std::runtime_error( "Could not commit the transaction." );
  }

  catalog.commit();

  // Removing the "cache"
  for ( std::vector< SimpleTestClass* >::iterator iObject = v_simpleTestClass.begin();
	iObject != v_simpleTestClass.end(); ++iObject ) {
    delete *iObject;
  }
  v_simpleTestClass.clear();
}


void
pool::TestDriver::addlfns()
{
  m_fileCatalog->start(); 
  m_fileCatalog->registerLFN( m_fileCatalog->lookupPFN(m_fileName1), m_lfn1 );
  m_fileCatalog->commit();
}

void
pool::TestDriver::read()
{
  pool::IFileCatalog& catalog = *m_fileCatalog;

  std::cout << "Creating the persistency service" << std::endl;
  std::unique_ptr< pool::IPersistencySvc > persistencySvc( pool::IPersistencySvc::create(catalog) );
  catalog.start();

  // Starting a read transaction
  if ( ! persistencySvc->session().transaction().start( pool::ITransaction::READ ) ) {
    throw std::runtime_error( "Could not start a read transaction." );
  }

  pool::IDatabase* db = persistencySvc->session().databaseHandle( m_lfn1, pool::DatabaseSpecification::LFN );
  if ( ! db ) {
    throw std::runtime_error( "Could not retrieve a database handle" );
  }

  const std::string& pfn = db->pfn();
  std::cout << "PFN1 = " << pfn << std::endl;
  const std::string& fid = db->fid();
  std::cout << "FID1 = " << fid << std::endl;

  db->connectForRead(); // will open db in read mode.

  pool::IContainer* container = db->containerHandle( "SimpleTestClass_Container" );
  if ( ! container ) {
    throw std::runtime_error( "Could not retrieve the container" );
  }

  pool::ITokenIterator* tokenIterator = container->tokens( "" );
  if ( ! tokenIterator ) {
    throw std::runtime_error( "Could not obtain a token iterator" );
  }

  int i = 0;
  std::vector< SimpleTestClass* > v_simpleTestClass;
  Token* token = tokenIterator->next();
  while ( token ) {
    void* data_simpleTestClass = persistencySvc->readObject( *token );
    if ( data_simpleTestClass == 0 ) {
      throw std::runtime_error( "Could not read the stored data" );
    }
    SimpleTestClass* object_simpleTestClass = reinterpret_cast< SimpleTestClass* >( data_simpleTestClass );
    if ( object_simpleTestClass->data != i ) {
      throw std::runtime_error( "Unexpected data read back" );
    }
    ++i;
    delete object_simpleTestClass;
    token->release();
    token = tokenIterator->next();
  }

  delete tokenIterator;
  delete container;
  delete db;

  std::cout << "Committing the transaction." << std::endl;
  if ( ! persistencySvc->session().transaction().commit() ) {
    throw std::runtime_error( "Could not commit the transaction." );
  }

  catalog.commit();
}
