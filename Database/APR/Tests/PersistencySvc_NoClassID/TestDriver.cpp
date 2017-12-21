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

#include "MyTestClass.h"

int numTokenInstances();

pool::TestDriver::TestDriver():
  m_fileCatalog( 0 ),
  m_fileName( "pool.root" ),
  m_events( 100 )
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
  RootType class_MyTestClass ( "MyTestClass" );

  // Defining the placement objects
  Placement placementHint_MyTestClass;
  placementHint_MyTestClass.setFileName( m_fileName );
  placementHint_MyTestClass.setContainerName( "MyTestClass_Container" );
  placementHint_MyTestClass.setTechnology( pool::ROOTKEY_StorageType.type() );

  std::vector< MyTestClass* > v_myTestClass;
  std::cout << "Writing " << m_events << " objects" << std::endl;
  for ( int i = 0; i < m_events; ++i ) {
    MyTestClass* object_MyTestClass = new MyTestClass();
    v_myTestClass.push_back( object_MyTestClass );
    object_MyTestClass->data = i;
    Token* token_MyTestClass = persistencySvc->registerForWrite( placementHint_MyTestClass,
								       object_MyTestClass,
								       class_MyTestClass );
    if ( ! token_MyTestClass ) {
      throw std::runtime_error( "Could not write an object" );
    }
    m_tokens.push_back( token_MyTestClass );
  }

  std::cout << "Committing the transaction." << std::endl;
  if ( ! persistencySvc->session().transaction().commit() ) {
    throw std::runtime_error( "Could not commit the transaction." );
  }

  // Removing the "cache"
  for ( std::vector< MyTestClass* >::iterator iObject = v_myTestClass.begin();
	iObject != v_myTestClass.end(); ++iObject ) {
    delete *iObject;
  }

  catalog.commit();
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

  std::cout << "Reading back the objects." << std::endl;
  for ( int i = 0; i < m_events; ++i ) {
    void* data_myTestClass = persistencySvc->readObject( *( m_tokens[ i ] ) );
    if ( data_myTestClass == 0 ) {
      throw std::runtime_error( "Could not read the stored data" );
    }
    MyTestClass* object_myTestClass = reinterpret_cast< MyTestClass* >( data_myTestClass );
    if ( object_myTestClass->data != i ) {
      std::ostringstream error;
      error << "MyTestClassy object written is different from object read";
      throw std::runtime_error( error.str() );
    }
    delete object_myTestClass;
  }

  std::cout << "Committing the transaction." << std::endl;
  if ( ! persistencySvc->session().transaction().commit() ) {
    throw std::runtime_error( "Could not commit the transaction." );
  }

  for ( std::vector< Token* >::iterator iToken = m_tokens.begin();
	iToken != m_tokens.end(); ++iToken ) {
    (*iToken)->release();
  }
  m_tokens.clear();

  catalog.commit();
}
