/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TestDriver.h"
#include "SimpleTestClass.h"

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


static const std::string file1 = "pool_test1.root";
static const std::string file2 = "pool_test2.root";
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

  pool::FileDescriptor fd( file1, file1 );
  if ( ! ( storSvc->connect( sessionHandle, pool::CREATE, fd ).isSuccess() ) ) {
    throw std::runtime_error( "Could not start a connection." );
  }
  pool::DatabaseConnection* connection = fd.dbc();

  // Retrieve the dictionary
  RootType class_SimpleTestClass  ( "SimpleTestClass" );
  if ( ! class_SimpleTestClass ) {
    throw std::runtime_error( "Could not retrieve the dictionary for class SimpleTestClass" );
  }
  //std::string guid_nam = class_SimpleTestClass->propertyList()->getProperty( "ClassID" );
  //if ( guid_nam.empty() ) {
  //  std::ostringstream error;
  //  error << "There is no ClassID property for class \"SimpleTestClass\"" << std::ends;
  //  throw std::runtime_error( error.str() );
  //}
  //Guid guid(guid_nam);

  std::vector< SimpleTestClass* > myObjects;
  for ( int i = 0; i < nObjects; ++i ) {
    myObjects.push_back( new SimpleTestClass() );
    SimpleTestClass* myObject = myObjects.back();
    myObject->data = i;

    // Creating the persistent shape.
    Guid guid = pool::DbReflex::guid(class_SimpleTestClass);
    const pool::Shape* shape = 0;
    if ( storSvc->getShape( fd, guid, shape ) == pool::IStorageSvc::SHAPE_NOT_AVAILIBLE ) {
      storSvc->createShape( fd, container, guid, shape );
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
  for ( std::vector< SimpleTestClass* >::iterator iObject = myObjects.begin();
	iObject != myObjects.end(); ++iObject ) delete *iObject;
  myObjects.clear();

  if ( ! ( storSvc->disconnect( fd ).isSuccess() ) ) {
    throw std::runtime_error( "Could not disconnect." );
  }
  if ( ! ( storSvc->endSession( sessionHandle ).isSuccess() ) ) {
    throw std::runtime_error( "Could not end correctly the session." );
  }
  storSvc->release();
}


void
TestDriver::testParallelReadWrite()
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
  if ( ! ( storSvc->startSession( pool::UPDATE, pool::ROOT_StorageType.type(), sessionHandle ).isSuccess() ) ) {
    throw std::runtime_error( "Could not start a session." );
  }


  // Open the file to read
  pool::FileDescriptor fd1( file1, file1 );
  sc = storSvc->connect( sessionHandle, pool::READ, fd1 );
  if ( sc != pool::DbStatus::Success ) {
    throw std::runtime_error( "Could not start a connection." );
  }

  // Open the file to write
  pool::FileDescriptor fd2( file2, file2 );
  if ( ! ( storSvc->connect( sessionHandle, pool::CREATE, fd2 ).isSuccess() ) ) {
    throw std::runtime_error( "Could not start a connection." );
  }

  // Fetch the containers
  std::vector<const Token*> containerTokens;
  storageExplorer->containers( fd1, containerTokens );
  if ( containerTokens.size() != 1 ) {
    throw std::runtime_error( "Unexpected number of containers" );
  }
  const Token* containerToken = containerTokens.front();
  const std::string containerName = containerToken->contID();
  if ( containerName != container ) {
    throw std::runtime_error( "Container name read is different from the container name written" );
  }


  // Retrieve the dictionary
  RootType class_SimpleTestClass  ( "SimpleTestClass" );
  if ( ! class_SimpleTestClass ) {
    throw std::runtime_error( "Could not retrieve the dictionary for class SimpleTestClass" );
  }
  //std::string guid_nam = class_SimpleTestClass->propertyList()->getProperty( "ClassID" );
  //if ( guid_nam.empty() ) {
  //  std::ostringstream error;
  //  error << "There is no ClassID property for class \"SimpleTestClass\"" << std::ends;
  //  throw std::runtime_error( error.str() );
  //}
  //Guid guidw(guid_nam);

  std::vector< SimpleTestClass* > myObjects;


  // Fetch the objects in the container.
  pool::DbSelect selectionObject("");
  sc = storageExplorer->select( fd1, containerToken->contID(), selectionObject );
  int iObject = 0;
  if ( sc.isSuccess() ) {
    Token* objectToken = 0; \
    while ( storageExplorer->next( selectionObject, objectToken ).isSuccess() ) {

      // Read the object from one file
      const Guid& guid = objectToken->classID();
      //const seal::reflect::Class* classType = seal::reflect::Class::forGuid( guid.toString() );
      const pool::Shape* shape = 0;
      if ( storSvc->getShape( fd1, guid, shape ) != pool::IStorageSvc::IS_PERSISTENT_SHAPE ) {
	throw std::runtime_error( "Could not fetch the persistent shape" );
      }
      RootType classType = pool::DbReflex::forGuid(guid);
      if(!classType){
	throw std::runtime_error( "Could not resolve the class by guid" );
      }
      void* ptr = 0;
      if ( ! ( storSvc->read( fd1, *objectToken, shape, &ptr ) ).isSuccess() ) {
	throw std::runtime_error( "failed to read an object back from the persistency" );
      }

      if ( shape->shapeID().toString() != "4E1F4DBB-1973-1974-1999-204F37331A01" ) {
        throw std::runtime_error( std::string("read wrong class type: ") + shape->shapeID().toString());
      }
      SimpleTestClass* object = reinterpret_cast< SimpleTestClass* >(ptr);
      if ( object->data != iObject ) {
	throw std::runtime_error( "Object read different from object written" );
      }
      objectToken->release();



      // Write a new object into the other file.
      myObjects.push_back( new SimpleTestClass() );
      SimpleTestClass* myObject = myObjects.back();
      myObject->data = object->data;

      // Creating the persistent shape.
      Guid guidw = pool::DbReflex::guid(class_SimpleTestClass);
      const pool::Shape* shapew = 0;
      if ( storSvc->getShape( fd2, guidw, shapew ) == pool::IStorageSvc::SHAPE_NOT_AVAILIBLE ) {
	storSvc->createShape( fd2, container, guidw, shapew );
      }
      if ( ! shapew ) {
	throw std::runtime_error( "Could not create a persistent shape." );
      }
      
      // Writing the object.
      Token* tokenw;
      if ( ! ( storSvc->allocate( fd2,
				  container, pool::ROOTTREE_StorageType.type(),
				  myObject, shapew, tokenw ).isSuccess() ) ) {
	throw std::runtime_error( "Could not write an object" );
      }
      tokenw->setClassID( guidw );
      tokenw->release();

      delete object;
      ++iObject;
    }
  }
  if ( iObject != nObjects ) {
    throw std::runtime_error( "Objects read different from objects written" );
  }


  // Closing the transaction.
  if ( ! ( storSvc->endTransaction( fd1.dbc(), pool::Transaction::TRANSACT_COMMIT ).isSuccess() ) ) {
    throw std::runtime_error( "Could not end a transaction." );
  }
  if ( ! ( storSvc->endTransaction( fd2.dbc(), pool::Transaction::TRANSACT_COMMIT ).isSuccess() ) ) {
    throw std::runtime_error( "Could not end a transaction." );
  }

  // Clearing the cache
  for ( std::vector< SimpleTestClass* >::iterator iObject = myObjects.begin();
	iObject != myObjects.end(); ++iObject ) delete *iObject;
  myObjects.clear();


  if ( ! ( storSvc->disconnect( fd1 ).isSuccess() ) ) {
    throw std::runtime_error( "Could not disconnect." );
  }

  if ( ! ( storSvc->disconnect( fd2 ).isSuccess() ) ) {
    throw std::runtime_error( "Could not disconnect." );
  }

  std::cout << "Closing the session" << std::endl;
  if ( ! ( storSvc->endSession( sessionHandle ).isSuccess() ) ) {
    throw std::runtime_error( "Could not end correctly the session." );
  }
  storageExplorer->release();
  storSvc->release();
}




void
TestDriver::testReading()
{
  pool::IStorageSvc* storSvc = pool::createStorageSvc("StorageSvc");
  if ( ! storSvc ) {
    throw std::runtime_error( "Could not create a StorageSvc object" );
  }
  void* pVoid = 0;
  pool::DbStatus sc = storSvc->queryInterface( pool::IStorageExplorer::interfaceID(), &pVoid );
  pool::IStorageExplorer* storageExplorer = (pool::IStorageExplorer*)pVoid;
  if ( !( sc == pool::DbStatus::Success && storageExplorer ) ) {
    storSvc->release();
    throw std::runtime_error( "Could not retrieve a IStorageExplorer interface" );
  }
  storageExplorer->addRef();

  pool::Session* sessionHandle = 0;
  if ( ! ( storSvc->startSession( pool::READ, pool::ROOT_StorageType.type(), sessionHandle ).isSuccess() ) ) {
    throw std::runtime_error( "Could not start a session." );
  }

  pool::FileDescriptor fd( file2, file2 );
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
      //const seal::reflect::Class* classType = seal::reflect::Class::forGuid( guid.toString() );
      const pool::Shape* shape = 0;
      if ( storSvc->getShape( fd, guid, shape ) != pool::IStorageSvc::IS_PERSISTENT_SHAPE ) {
	throw std::runtime_error( "Could not fetch the persistent shape" );
      }
      RootType classType = pool::DbReflex::forGuid(guid);
      if(!classType){
	throw std::runtime_error( "Could not resolve the class by guid" );
      }      
      void* ptr = 0;
      if ( ! ( storSvc->read( fd, *objectToken, shape, &ptr ) ).isSuccess() ) {
	throw std::runtime_error( "failed to read an object back from the persistency" );
      }

      if ( shape->shapeID().toString() != "4E1F4DBB-1973-1974-1999-204F37331A01" ) {
        throw std::runtime_error( std::string("read wrong class type: ") + shape->shapeID().toString());
      }
      SimpleTestClass* object = reinterpret_cast< SimpleTestClass* >(ptr);
      if ( object->data != iObject ) {
	throw std::runtime_error( "Object read different from object written" );
      }
      delete object;
      ++iObject;
      objectToken->release();
    }
  }
  if ( iObject != nObjects ) {
    throw std::runtime_error( "Objects read different from objects written" );
  }

  if ( ! ( storSvc->endTransaction( fd.dbc(), pool::Transaction::TRANSACT_COMMIT ).isSuccess() ) ) {
    throw std::runtime_error( "Could not end a transaction." );
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
