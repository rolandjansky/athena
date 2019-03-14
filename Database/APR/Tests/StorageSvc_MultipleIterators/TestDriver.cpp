/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TestDriver.h"
#include "SimpleTestClass.h"
#include "TestClassPrimitives.h"

#include "libname.h"

#include "PersistentDataModel/Guid.h"
#include "PersistentDataModel/Token.h"


#include "StorageSvc/Shape.h"
#include "StorageSvc/IStorageSvc.h"
#include "StorageSvc/IStorageExplorer.h"
#include "StorageSvc/DbSelect.h"
#include "StorageSvc/DbReflex.h"
#include "StorageSvc/DatabaseConnection.h"
#include "StorageSvc/FileDescriptor.h"
#include "StorageSvc/DataCallBack.h"
#include "StorageSvc/DbType.h"
#include "StorageSvc/DbObjectCallBack.h"

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <memory>


static const std::string file = "pool_test.root";
static const std::string container = "container";
static const int nObjects = 100;


TestDriver::TestDriver()
{
}

TestDriver::~TestDriver()
{
   std::cout << "[OVAL] Number of floating tokens : " << Token::numInstances() << std::endl;
}


void
TestDriver::testWriting()
{
  pool::IStorageSvc* storSvc = pool::createStorageSvc("StorageSvc");
  if ( ! storSvc ) {
    throw std::runtime_error( "Could not create a StorageSvc object" );
  }
  storSvc->addRef();
  pool::Session* sessionHandle = 0;
  if ( ! ( storSvc->startSession( pool::CREATE, pool::ROOT_StorageType.type(), sessionHandle ).isSuccess() ) ) {
    throw std::runtime_error( "Could not start a session." );
  }

  pool::FileDescriptor fd( file, file );
  if ( ! ( storSvc->connect( sessionHandle, pool::CREATE, fd ).isSuccess() ) ) {
    throw std::runtime_error( "Could not start a connection." );
  }
  pool::DatabaseConnection* connection = fd.dbc();

  // Retrieve the dictionary
  const RootType class_SimpleTestClass  ( "SimpleTestClass" );
  if ( ! class_SimpleTestClass ) {
    throw std::runtime_error( "Could not retrieve the dictionary for class SimpleTestClass" );
  }
  //std::string guid_nam_SimpleTestClass = class_SimpleTestClass->propertyList()->getProperty( "ClassID" );
  //if ( guid_nam_SimpleTestClass.empty() ) {
  //  std::ostringstream error;
  //  error << "There is no ClassID property for class \"SimpleTestClass\"" << std::ends;
  //  throw std::runtime_error( error.str() );
  //}
  //pool::Guid guid_SimpleTestClass(guid_nam_SimpleTestClass);
  const RootType class_TestClassPrimitives  ( "TestClassPrimitives" );
  if ( ! class_TestClassPrimitives ) {
    throw std::runtime_error( "Could not retrieve the dictionary for class TestClassPrimitives" );
  }
  //std::string guid_nam_TestClassPrimitives = class_TestClassPrimitives->propertyList()->getProperty( "ClassID" );
  //if ( guid_nam_TestClassPrimitives.empty() ) {
  //  std::ostringstream error;
  //  error << "There is no ClassID property for class \"TestClassPrimitives\"" << std::ends;
  //  throw std::runtime_error( error.str() );
  //}
  //pool::Guid guid_TestClassPrimitives(guid_nam_TestClassPrimitives);

  std::vector< SimpleTestClass* > myObjects_SimpleTestClass;
  std::vector< TestClassPrimitives* > myObjects_TestClassPrimitives;
  for ( int i = 0; i < nObjects; ++i ) {
    myObjects_SimpleTestClass.push_back( new SimpleTestClass() );
    SimpleTestClass* myObject_SimpleTestClass = myObjects_SimpleTestClass.back();
    myObject_SimpleTestClass->data = i;

    // Creating the persistent shape.
    Guid guid = pool::DbReflex::guid(class_SimpleTestClass);
    const pool::Shape* shape_SimpleTestClass = 0;
    if ( storSvc->getShape( fd, guid, shape_SimpleTestClass ) == pool::IStorageSvc::SHAPE_NOT_AVAILIBLE ) {
      storSvc->createShape( fd, container, guid, shape_SimpleTestClass );
    }
    if ( ! shape_SimpleTestClass ) {
      throw std::runtime_error( "Could not create a persistent shape." );
    }
  
    // Writing the object.
    Token* token_SimpleTestClass;
    if ( ! ( storSvc->allocate( fd,
				container, pool::ROOTKEY_StorageType.type(),
				myObject_SimpleTestClass, shape_SimpleTestClass, token_SimpleTestClass ).isSuccess() ) ) {
      throw std::runtime_error( "Could not write an object" );
    }
    //token_SimpleTestClass->setClassID( guid_SimpleTestClass );
    delete token_SimpleTestClass;


    // The second class
    myObjects_TestClassPrimitives.push_back( new TestClassPrimitives() );
    TestClassPrimitives* myObject_TestClassPrimitives = myObjects_TestClassPrimitives.back();
    myObject_TestClassPrimitives->setNonZero();

    // Creating the persistent shape.
    Guid guidp = pool::DbReflex::guid(class_TestClassPrimitives);
    const pool::Shape* shape_TestClassPrimitives = 0;
    if ( storSvc->getShape( fd, guidp, shape_TestClassPrimitives ) == pool::IStorageSvc::SHAPE_NOT_AVAILIBLE ) {
      storSvc->createShape( fd, container, guidp, shape_TestClassPrimitives );
    }
    if ( ! shape_TestClassPrimitives ) {
      throw std::runtime_error( "Could not create a persistent shape." );
    }
  
    // Writing the object.
    Token* token_TestClassPrimitives;
    if ( ! ( storSvc->allocate( fd,
				container, pool::ROOTKEY_StorageType.type(),
				myObject_TestClassPrimitives, shape_TestClassPrimitives, token_TestClassPrimitives ).isSuccess() ) ) {
      throw std::runtime_error( "Could not write an object" );
    }
    //token_TestClassPrimitives->setClassID( guid_TestClassPrimitives );
    delete token_TestClassPrimitives;

  }

  // Closing the transaction.
  if ( ! ( storSvc->endTransaction( connection, pool::Transaction::TRANSACT_COMMIT ).isSuccess() ) ) {
    throw std::runtime_error( "Could not end a transaction." );
  }

  // Clearing the cache
  for ( std::vector< SimpleTestClass* >::iterator iObject_SimpleTestClass = myObjects_SimpleTestClass.begin();
	iObject_SimpleTestClass != myObjects_SimpleTestClass.end(); ++iObject_SimpleTestClass ) delete *iObject_SimpleTestClass;
  myObjects_SimpleTestClass.clear();

  for ( std::vector< TestClassPrimitives* >::iterator iObject_TestClassPrimitives = myObjects_TestClassPrimitives.begin();
	iObject_TestClassPrimitives != myObjects_TestClassPrimitives.end(); ++iObject_TestClassPrimitives ) delete *iObject_TestClassPrimitives;
  myObjects_TestClassPrimitives.clear();

  // Disconnecting
  if ( ! ( storSvc->disconnect( fd ).isSuccess() ) ) {
    throw std::runtime_error( "Could not disconnect." );
  }
  if ( ! ( storSvc->endSession( sessionHandle ).isSuccess() ) ) {
    throw std::runtime_error( "Could not end correctly the session." );
  }
  storSvc->release();
}


void
TestDriver::testReadingParallelSameContainer()
{
  pool::IStorageSvc* storSvc = pool::createStorageSvc("StorageSvc");
  if ( ! storSvc ) {
    throw std::runtime_error( "Could not create a StorageSvc object" );
  }
  storSvc->addRef();
  void* pVoid = 0;
  pool::DbStatus sc = storSvc->queryInterface( pool::IStorageExplorer::interfaceID(), &pVoid );
  pool::IStorageExplorer* storageExplorer = (pool::IStorageExplorer*)pVoid;
  if ( !( sc == pool::DbStatus::Success && storageExplorer ) ) {
    storSvc->release();
    throw std::runtime_error( "Could not retrieve a IStorageExplorer interface" );
  }

  pool::Session* sessionHandle = 0;
  if ( ! ( storSvc->startSession( pool::READ, pool::ROOT_StorageType.type(), sessionHandle ).isSuccess() ) ) {
    throw std::runtime_error( "Could not start a session." );
  }

  pool::FileDescriptor* fd = new pool::FileDescriptor( file, file );
  sc = storSvc->connect( sessionHandle, pool::READ, *fd );
  if ( sc != pool::DbStatus::Success ) {
    throw std::runtime_error( "Could not start a connection." );
  }

  pool::DatabaseConnection* connection = fd->dbc();

  // Fetch the containers
  std::vector<const Token*> containerTokens;
  storageExplorer->containers( *fd, containerTokens );
  if ( containerTokens.size() != 1 ) {
    throw std::runtime_error( "Unexpected number of containers" );
  }
  const Token* containerToken = containerTokens.front();
  const std::string containerName = containerToken->contID();
  if ( containerName != container ) {
    throw std::runtime_error( "Container name read is different from the container name written" );
  }

  // Fetch the objects in the container (Initialize the iterators)
  pool::DbSelect selectionObject_SimpleTestClass("");
  sc = storageExplorer->select( *fd, containerToken->contID(), selectionObject_SimpleTestClass );
  if ( ! sc.isSuccess() ) {
    throw std::runtime_error( "Could not start an implicit collection iteration" );
  }
  pool::DbSelect selectionObject_TestClassPrimitives("");
  sc = storageExplorer->select( *fd, containerToken->contID(), selectionObject_TestClassPrimitives );
  if ( ! sc.isSuccess() ) {
    throw std::runtime_error( "Could not start an implicit collection iteration" );
  }

  // Start the parallel iteration
  int iObject = 0;
  while ( iObject < nObjects ) {

    // First iterator
    Token* objectToken_SimpleTestClass = 0;
    if ( ! ( storageExplorer->next( selectionObject_SimpleTestClass, objectToken_SimpleTestClass ).isSuccess() ) ) {
      throw std::runtime_error( "Could not retrieve the object token" );
    }

    const pool::Shape* shape_SimpleTestClass = 0;
    if ( storSvc->getShape( *fd, objectToken_SimpleTestClass->classID(), shape_SimpleTestClass ) != pool::IStorageSvc::IS_PERSISTENT_SHAPE ) {
      throw std::runtime_error( "Could not fetch the persistent shape for SimpleTestClass" );
    }
    void* ptr_SimpleTestClass = 0;
    if ( ! ( storSvc->read( *fd, *objectToken_SimpleTestClass, shape_SimpleTestClass, &ptr_SimpleTestClass ) ).isSuccess() ) {
      throw std::runtime_error( "failed to read a SimpleTestClass object back from the persistency" );
    }
    
    if ( shape_SimpleTestClass->shapeID().toString() != "4E1F4DBB-1973-1974-1999-204F37331A01" ) {
      throw std::runtime_error( std::string("read wrong class type: ") + shape_SimpleTestClass->shapeID().toString());
    }

    SimpleTestClass* object_SimpleTestClass = reinterpret_cast< SimpleTestClass* >(ptr_SimpleTestClass);
    if ( object_SimpleTestClass->data != iObject ) {
      throw std::runtime_error( "Object read different from object written" );
    }
    delete object_SimpleTestClass;
    objectToken_SimpleTestClass->release();

    // Skip the next object for this iterator
    objectToken_SimpleTestClass = 0;
    if ( ! ( storageExplorer->next( selectionObject_SimpleTestClass, objectToken_SimpleTestClass ).isSuccess() ) ) {
      throw std::runtime_error( "Could not retrieve the object token" );
    }
    objectToken_SimpleTestClass->release();


    // Second iterator
    // Skip the next object for this iterator
    Token* objectToken_TestClassPrimitives = 0;
    if ( ! ( storageExplorer->next( selectionObject_TestClassPrimitives, objectToken_TestClassPrimitives ).isSuccess() ) ) {
      throw std::runtime_error( "Could not retrieve the object token" );
    }
    objectToken_TestClassPrimitives->release();

    // Read the next object for this iterator
    objectToken_TestClassPrimitives = 0;
    if ( ! ( storageExplorer->next( selectionObject_TestClassPrimitives, objectToken_TestClassPrimitives ).isSuccess() ) ) {
      throw std::runtime_error( "Could not retrieve the object token" );
    }

    const pool::Shape* shape_TestClassPrimitives = 0;
    if ( storSvc->getShape( *fd, objectToken_TestClassPrimitives->classID(), shape_TestClassPrimitives ) != pool::IStorageSvc::IS_PERSISTENT_SHAPE ) {
      throw std::runtime_error( "Could not fetch the persistent shape for TestClassPrimitives" );
    }
    void* ptr_TestClassPrimitives = 0;
    if ( ! ( storSvc->read( *fd, *objectToken_TestClassPrimitives, shape_TestClassPrimitives, &ptr_TestClassPrimitives ) ).isSuccess() ) {
      throw std::runtime_error( "failed to read a TestClassPrimitives object back from the persistency" );
    }
    
    if ( shape_TestClassPrimitives->shapeID().toString() != "4E1F4DBB-1973-1974-1999-204F37331A00" ) {
      throw std::runtime_error( std::string("read wrong class type: ") + shape_TestClassPrimitives->shapeID().toString());
    }

    TestClassPrimitives referenceObject;
    referenceObject.setNonZero();

    TestClassPrimitives* object_TestClassPrimitives = reinterpret_cast< TestClassPrimitives* >(ptr_TestClassPrimitives);
    if ( *object_TestClassPrimitives != referenceObject ) {
      throw std::runtime_error( "Object read different from object written" );
    }
    delete object_TestClassPrimitives;
    objectToken_TestClassPrimitives->release();

    // Continue the iteration
    ++iObject;
  }


  if ( iObject != nObjects ) {
    throw std::runtime_error( "Objects read different from objects written" );
  }

  // Closing the transaction.
  if ( ! ( storSvc->endTransaction( connection, pool::Transaction::TRANSACT_COMMIT ).isSuccess() ) ) {
    throw std::runtime_error( "Could not end a transaction." );
  }

  if ( ! ( storSvc->disconnect( *fd ).isSuccess() ) ) {
    throw std::runtime_error( "Could not disconnect." );
  }
  delete fd;

  std::cout << "Closing the session" << std::endl;
  if ( ! ( storSvc->endSession( sessionHandle ).isSuccess() ) ) {
    throw std::runtime_error( "Could not end correctly the session." );
  }
  storageExplorer->release();
  storSvc->release();
}
