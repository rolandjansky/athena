/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TestDriver.h"
#include "TestClassWithTransients.h"
#include "TestClassTransient.h"

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
#include "StorageSvc/DbType.h"

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
  RootType class_TestClassWithTransients  ( "TestClassWithTransients" );
  if ( ! class_TestClassWithTransients ) {
    throw std::runtime_error( "Could not retrieve the dictionary for class TestClassWithTransients" );
  }
  //std::string guid_nam = class_TestClassWithTransients->propertyList()->getProperty( "ClassID" );
  //if ( guid_nam.empty() ) {
  //  std::ostringstream error;
  //  error << "There is no ClassID property for class \"TestClassWithTransients\"" << std::ends;
  //  throw std::runtime_error( error.str() );
  //}
  //Guid guid(guid_nam);

  std::vector< TestClassWithTransients* > myObjects;
  std::vector< TestClassTransient* > myTransientObjects;

  for ( int i = 0; i < nObjects; ++i ) {
    myObjects.push_back( new TestClassWithTransients() );
    TestClassWithTransients* myObject = myObjects.back();
    myObject->data1 = i;
    for ( int j = 0; j < 4; ++j ) {
      myObject->transientData[j].data1 = 4*i + j;
      myObject->transientData[j].data2 = 4*i + j + 0.5;
      myObject->transientData[j].data3 = (4*i + j)%99;
    }
    myTransientObjects.push_back( new TestClassTransient() );
    myObject->transientPointer = myTransientObjects.back();
    myObject->transientPointer->data = 3*i;
    myObject->transientObject.data1 = 1;
    myObject->transientObject.data2 = 2;
    myObject->data2 = 2*i + 0.3;

    // Creating the persistent shape.
    Guid guidWT = pool::DbReflex::guid(class_TestClassWithTransients);
    const pool::Shape* shape = 0;
    if ( storSvc->getShape( fd, guidWT, shape ) == pool::IStorageSvc::SHAPE_NOT_AVAILIBLE ) {
      storSvc->createShape( fd, container, guidWT, shape );
    }
    if ( ! shape ) {
      throw std::runtime_error( "Could not create a persistent shape." );
    }
  
    // Writing the object.
    Token* token;
    if ( ! ( storSvc->allocate( fd,
				container, pool::ROOTTREE_StorageType.type(),
				myObject, shape, token ).isSuccess() ) ) {
      throw std::runtime_error( "Could not write an object" );
    }
    //token->setClassID( guid );
    delete token;
  }

  // Closing the transaction.
  if ( ! ( storSvc->endTransaction( connection, pool::Transaction::TRANSACT_COMMIT ).isSuccess() ) ) {
    throw std::runtime_error( "Could not end a transaction." );
  }

  // Clearing the cache
  for ( std::vector< TestClassWithTransients* >::iterator iObject = myObjects.begin();
	iObject != myObjects.end(); ++iObject ) delete *iObject;
  myObjects.clear();

  for ( std::vector< TestClassTransient* >::iterator iObject = myTransientObjects.begin();
	iObject != myTransientObjects.end(); ++iObject ) delete *iObject;
  myTransientObjects.clear();

  if ( ! ( storSvc->disconnect( fd ).isSuccess() ) ) {
    throw std::runtime_error( "Could not disconnect." );
  }
  if ( ! ( storSvc->endSession( sessionHandle ).isSuccess() ) ) {
    throw std::runtime_error( "Could not end correctly the session." );
  }
  storSvc->release();
}


void
TestDriver::testReading()
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

  pool::FileDescriptor fd( file, file );
  sc = storSvc->connect( sessionHandle, pool::READ, fd );
  if ( sc != pool::DbStatus::Success ) {
    throw std::runtime_error( "Could not start a connection." );
  }

  // Fetch the containers
  std::vector<const Token*> containerTokens;
  storageExplorer->containers( fd, containerTokens );
  if ( containerTokens.size() != 1 ) {
    throw std::runtime_error( "Unexpected number of containers" );
  }
  const Token* containerToken = containerTokens.front();
  const std::string containerName = containerToken->contID();
  if ( containerName != container ) {
    throw std::runtime_error( "Container name read is different from the container name written" );
  }

  // Fetch the objects in the container.
  pool::DbSelect selectionObject("");
  sc = storageExplorer->select( fd, containerToken->contID(), selectionObject );
  int iObject = 0;
  if ( sc.isSuccess() ) {
    Token* objectToken = 0;
    while ( storageExplorer->next( selectionObject, objectToken ).isSuccess() ) {
      const Guid& guid = objectToken->classID();
      const pool::Shape* shape = 0;
      if ( storSvc->getShape( fd, guid, shape ) != pool::IStorageSvc::IS_PERSISTENT_SHAPE ) {
	throw std::runtime_error( "Could not fetch the persistent shape" );
      }
      void* ptr = 0;
      if ( ! ( storSvc->read( fd, *objectToken, shape, &ptr ) ).isSuccess() ) {
	throw std::runtime_error( "failed to read an object back from the persistency" );
      }

      if ( shape->shapeID().toString() != "A1111111-B111-C111-D111-E22111111121" ) {
        throw std::runtime_error( std::string("read wrong class type: ") + shape->shapeID().toString());
      }
      TestClassWithTransients* object = reinterpret_cast< TestClassWithTransients* >(ptr);
      int expectedData1 = iObject;
      if ( object->data1 != expectedData1 ) {
	std::ostringstream error;
	error << "data1 expected: " << expectedData1 << " and got " << object->data1 << std::endl;
	throw std::runtime_error( error.str() );
      }

      for ( int j = 0; j < 4; ++j ) {
	if ( object->transientData[j].data1 != 0 ||
	     object->transientData[j].data2 != 0 ||
	     object->transientData[j].data3 != 0 ) {
	  throw std::runtime_error( "Object (transientData) read different from object written" );
	}
      }

      if ( object->transientPointer != 0 ) {
	throw std::runtime_error( "Object (transient pointer) read different from object written" );
      }

      if ( object->transientObject.data1 != 0 ||
	   object->transientObject.data2 != 0 ) {
	throw std::runtime_error( "Object (transient object) read different from object written" );
      }

      double expectedData2 = 2*iObject + 0.3;
      if ( object->data2 != expectedData2 ) {
	std::ostringstream error;
	error << "data2 expected: " << expectedData2 << " and got " << object->data2 << std::endl;
	throw std::runtime_error( error.str() );
      }

      delete object;
      ++iObject;
      objectToken->release();
    }
  }

  if ( iObject != nObjects ) {
    throw std::runtime_error( "Objects read different from objects written" );
  }

  if ( ! ( storSvc->disconnect( fd ).isSuccess() ) ) {
    throw std::runtime_error( "Could not disconnect." );
  }

  std::cout << "Closing the session" << std::endl;
  if ( ! ( storSvc->endSession( sessionHandle ).isSuccess() ) ) {
    throw std::runtime_error( "Could not end correctly the session." );
  }
  storageExplorer->release();
  storSvc->release();
}
