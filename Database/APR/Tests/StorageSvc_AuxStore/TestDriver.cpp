/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include "TestDriver.h"
#include "AuxStoreTest.h"

#include "libname.h"

#include "PersistentDataModel/Guid.h"
#include "PersistentDataModel/Token.h"

#include "StorageSvc/Shape.h"
#include "StorageSvc/poolDb.h"
#include "StorageSvc/IStorageSvc.h"
#include "StorageSvc/IStorageExplorer.h"
#include "StorageSvc/DbSelect.h"
#include "StorageSvc/DbReflex.h"
#include "StorageSvc/DatabaseConnection.h"
#include "StorageSvc/FileDescriptor.h"
#include "StorageSvc/DbType.h"
#include "StorageSvc/DbPrint.h"

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <memory>

#include "AthContainers/AuxTypeRegistry.h"

using namespace std;

int numTokenInstances();

static const std::string file = "pool_test.root";
static const std::string container = "CollectionTree(container Aux.)";
static int nObjects = 10;
// hardcode here in case we only test reading, remember to keep in sync with selection.xml
static string   testTypeID = "A2222222-B111-C111-D111-E22134511121";


TestDriver::TestDriver()
{
   pool::DbPrintLvl::setLevel( pool::DbPrintLvl::Info );
}

TestDriver::~TestDriver()
{
  std::cout << "[OVAL] Number of floating tokens : " << numTokenInstances() << std::endl;
}


inline int getVal1(int objn, int idx) { return objn*1000 + idx; }
inline int getVal2(int objn, int idx) { return objn*5000 + (idx<<1); }


void
TestDriver::testWriting()
{
   cout << "createStorageSvc" << endl;
   pool::IStorageSvc* storSvc = pool::createStorageSvc( 0, "StorageSvc");
   if ( ! storSvc ) {
      throw std::runtime_error( "Could not create a StorageSvc object" );
   }
   storSvc->addRef();
   cout << "startSession" << endl;
   pool::Session* sessionHandle = 0;
   if ( ! ( storSvc->startSession( poolDb::CREATE, pool::ROOT_StorageType.type(), sessionHandle ).isSuccess() ) ) {
      throw std::runtime_error( "Could not start a session." );
   }

   cout << "Session connect" << endl;
   pool::FileDescriptor* fd = new pool::FileDescriptor( file, file );
   if ( ! ( storSvc->connect( sessionHandle, poolDb::CREATE, *fd ).isSuccess() ) ) {
      throw std::runtime_error( "Could not start a connection." );
   }
   pool::DatabaseConnection* connection = fd->dbc();
   pool::Transaction* transaction = 0;
   cout << "startTransaction" << endl;
   if ( ! ( storSvc->startTransaction( connection, transaction ).isSuccess() ) ) {
      throw std::runtime_error( "Could not start a transaction." );
   }

      
   SG::auxid_t ityp1 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt");
   SG::auxid_t ityp2 = SG::AuxTypeRegistry::instance().getAuxID<int> ("int2");
 
   cout << "Creating objects" << endl;
   vector<APRTest::AuxStore*>  objs;   
   for( int objn=0; objn <nObjects; objn++ ) {
      APRTest::AuxStore   *object = new APRTest::AuxStore();
      object->m_data = 123456;
      const int intN = 10;
      int* i1 = reinterpret_cast<int*>( object->getData(ityp1, intN, 20) );
      int* i2 = reinterpret_cast<int*>( object->getData(ityp2, intN, 20) );
      assert (object->size() == intN);
      for( int i=0; i<intN; i++ ) {
         i1[i] = getVal1(objn, i);
         i2[i] = getVal2(objn, i);
      } 
      objs.push_back( object );
   }
   cout << "****  m_data offset=" << (char*)&(objs[0]->m_data) - (char*)objs[0] << endl;

   
   // Creating the persistent shape.
   cout << "Creating Shape" << endl;
   // Retrieve the dictionary
   RootType testtype ( "APRTest::AuxStore" );
   if ( ! testtype ) {
      throw std::runtime_error( "Could not retrieve the dictionary for class APRTest::AuxStore" );
   }
   Guid guid = pool::DbReflex::guid(testtype);
   const pool::Shape* shape = 0;
   if ( storSvc->getShape( *fd, guid, shape ) == pool::IStorageSvc::SHAPE_NOT_AVAILIBLE ) {
      //storSvc->createShape( *fd, container, guid, shape, objs[0] );
      storSvc->createShape( *fd, container, guid, shape);
   }
   if ( ! shape ) {
      throw std::runtime_error( "Could not create a persistent shape." );
   }
   testTypeID = shape->shapeID().toString();

   // Writing the objects.
   cout << "Writing objects" << endl;
   for( int objn=0; objn <nObjects; objn++ ) {
      Token* token;
      if( ! ( storSvc->allocate( transaction, *fd,
                                 container, pool::ROOTTREE_StorageType.type(),
                                 objs[objn], shape, token ).isSuccess() ) ) {
         throw std::runtime_error( "Could not write an object" );
      }
      delete token;
   }

   // Closing the transaction.
   cout << "Committing transaction" << endl;
   if ( ! ( storSvc->endTransaction( transaction, pool::Transaction::TRANSACT_COMMIT ).isSuccess() ) ) {
      throw std::runtime_error( "Could not end a transaction." );
   }

// Clearing the cache
   for( int objn=0; objn <nObjects; objn++ ) 
      delete objs[objn];
   
   if ( ! ( storSvc->disconnect( *fd ).isSuccess() ) ) {
      throw std::runtime_error( "Could not disconnect." );
   }
   delete fd;
   if ( ! ( storSvc->endSession( sessionHandle ).isSuccess() ) ) {
      throw std::runtime_error( "Could not end correctly the session." );
   }
   storSvc->release();
}


void
TestDriver::testReading()
{
  pool::IStorageSvc* storSvc = pool::createStorageSvc( 0, "StorageSvc");
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
  if ( ! ( storSvc->startSession( poolDb::READ, pool::ROOT_StorageType.type(), sessionHandle ).isSuccess() ) ) {
    throw std::runtime_error( "Could not start a session." );
  }

  pool::FileDescriptor* fd = new pool::FileDescriptor( file, file );
  sc = storSvc->connect( sessionHandle, poolDb::READ, *fd );
  if ( sc != pool::DbStatus::Success ) {
    throw std::runtime_error( "Could not start a connection." );
  }

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

  // Fetch the objects in the container.
  pool::DbSelect selectionObject("");
  sc = storageExplorer->select( *fd, containerToken->contID(), selectionObject );
  int iObject = 0;
  if ( sc.isSuccess() ) {
    Token* objectToken = 0;
    while ( storageExplorer->next( selectionObject, objectToken ).isSuccess() ) {
      const Guid& guid = objectToken->classID();
      const pool::Shape* shape = 0;
      if ( storSvc->getShape( *fd, guid, shape ) != pool::IStorageSvc::IS_PERSISTENT_SHAPE ) {
	throw std::runtime_error( "Could not fetch the persistent shape" );
      }
      RootType classType = pool::DbReflex::forGuid(guid);
      if(!classType){
	throw std::runtime_error( "Could not resolve the class by guid" );
      }
      void* ptr = 0;
      if ( ! ( storSvc->read( *fd, *objectToken, shape, &ptr ) ).isSuccess() ) {
	throw std::runtime_error( "failed to read an object back from the persistency" );
      }

      if ( shape->shapeID().toString() != testTypeID ) {
	throw std::runtime_error( std::string("read wrong class type: ") + shape->shapeID().toString()
                                  + "  while expecting: " + testTypeID );
      }

      APRTest::AuxStore   * object = reinterpret_cast< APRTest::AuxStore* >( ptr );
      if( object->m_data != 123456 ) {
         throw std::runtime_error( "Object m_data read different from object written" );
      }

      SG::auxid_t ityp1 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt");
      SG::auxid_t ityp2 = SG::AuxTypeRegistry::instance().getAuxID<int> ("int2");
      const int intN = 10;
      const int* i1 = reinterpret_cast<const int*>( object->getData(ityp1) );
      const int* i2 = reinterpret_cast<const int*>( object->getData(ityp2) );
      assert (object->size() == intN);
      for( int i=0; i<intN; i++ ) {
         cout << i << ": " << i1[i] << "   : " << i2[i] << endl;
         if( i1[i] != getVal1(iObject, i) || i2[i] != getVal2(iObject,i) )
            throw std::runtime_error( "Objects AUX data read different from objects written" );
      }
      
      ++iObject;
      delete object; 
      objectToken->release();
    }
  }
  cout << "Objects read: " << iObject << endl;
  if ( iObject != nObjects ) {
    throw std::runtime_error( "Objects read different from objects written" );
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
