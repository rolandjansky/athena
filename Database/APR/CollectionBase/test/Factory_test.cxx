/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdio> // For sprintf on gcc45
#include <exception>
#include <iostream>
#include <string>

#include "PersistentDataModel/Token.h"

#include "FileCatalog/IFileCatalog.h"
#include "POOLCore/SystemTools.h"

#include "CollectionBase/ICollection.h"
#include "CollectionBase/CollectionDescription.h"
#include "CollectionBase/CollectionFactory.h"
#include "CxxUtils/checker_macros.h"


ATLAS_NO_CHECK_FILE_THREAD_SAFETY;


using namespace std;
using namespace pool;


int main(int argc, char** )
{
  SystemTools::initGaudi();
  
  bool crude = argc>1;

  (void)remove ("CollectionCatalog0.xml");
  (void)remove ("CollectionCatalog1.xml");
  (void)remove ("CollectionCatalog2.xml");
   
//  const string collConnection = "./";
  const string collConnection = "";
  const string collType = "RootCollection";
  MetaDataEntry* collMetaData = 0;
  
  Token* token = new Token();
  token->fromString("[DB=AEC1DFE2-010B-D811-9832-000347F31C25]"
                    "[CNT=TestContainer]"
                    "[CLID=12345678-1234-42AF-9CB6-567A94781A78]"
                    "[TECH=00000202]"
                    "[OID=00000003-00000002]");
  
//  AttributeList attList;
  CollectionFactory* factory = CollectionFactory::get();
  
  try{
    
    cout << "Create 3 collections and register them in 3 separate catalogs" << endl;
    
    for(int i=0; i<3 ; ++i){
      char catalogName[200];
      sprintf( catalogName, "xmlcatalog_file:CollectionCatalog%i.xml", i);
      setenv( "POOL_COLLECTION_WRITE_CATALOG", catalogName, 1);
      
      // reset the default catalog of the factory
      factory->setDefaultCatalog(0);
      
      char collName[200];
      sprintf( collName, "Collection%i", i); 
      
      char logicalName[200];
      sprintf( logicalName, "logicalName%i",i);
      
      bool authorizeReplace = true;
      
      ICollection* collection = 0;
      if(crude){
        string physicalName = collType+"|"+collConnection+"|"+collName; 
        // factory->create( type, session, connection, name, openMode);
	CollectionDescription	desc( physicalName, collType, collConnection);
        collection = factory->create( /*"PHYSICAL_NAME",*/
                                      desc,		
                                      ICollection::CREATE_AND_OVERWRITE);
      }else {
	 cout << "createAndRegister collection " << i << endl;
	 CollectionDescription	desc( collName, collType, collConnection );
	 collection = factory->createAndRegister( desc, 0, authorizeReplace, logicalName, collMetaData );
      }
      
//      collection->add(*token,attList);
      collection->commit();
      delete collection;
    }
    
    factory->setDefaultCatalog(0);

    // retrieve the guid of collection 1 and register a replica for it.
    FileCatalog::FileID guid;   // FileCatalog::FileID is a std::string
    {
      string logicalName = "logicalName1";
      string physicalName = collType+"|"+collConnection+"|"+"Collection1";
      IFileCatalog catalog;
      catalog.setWriteCatalog("xmlcatalog_file:CollectionCatalog1.xml");
      catalog.connect();
      catalog.start();
      guid = catalog.lookupLFN( logicalName );
      catalog.addReplicaPFN( physicalName, physicalName+"REPLICA");
      catalog.commit();
      catalog.disconnect();
    }
    
    setenv( "POOL_COLLECTION_WRITE_CATALOG", "xmlcatalog_file:CollectionCatalog0.xml", 1);
    setenv( "POOL_COLLECTION_READ_CATALOGS", "xmlcatalog_file:CollectionCatalog1.xml xmlcatalog_file:CollectionCatalog2.xml", 1);
    
    ICollection* collection = 0;
    
    
    cout << "Open collection 0 with physical name .... ";
    string physicalName =  collType+"|"+collConnection+"|"+"Collection0";
    if(crude) {
       CollectionDescription	desc(physicalName, "" );
       collection = factory->create(/*"PHYSICAL_NAME",*/ desc, ICollection::READ);
    } else {
       collection = factory->openWithPhysicalName(physicalName, 0);
    }
    cout << (collection ? "OK":"KO") << endl;
    delete collection;
    collection = nullptr;


    cout << "Open collection 1 with guid .... ";
    if(crude) {
       CollectionDescription	desc( guid, "" ); 
       collection = factory->create(/*"GUID",*/ desc, ICollection::READ);
    } else
       collection = factory->openWithGuid(guid, 0);
    cout << (collection ? "OK":"KO") << endl;
    delete collection;
    collection = nullptr;
    

    cout << "Open collection 2 with logical name .... ";
    if(crude) {
       CollectionDescription	desc( "logicalName2", "" ); 
       collection = factory->create(/*"LOGICAL_NAME",*/ desc, ICollection::READ);
    } else
       collection = factory->openWithLogicalName("logicalName2", 0);
    cout << (collection ? "OK":"KO") << endl;
    delete collection;
    collection = nullptr;
        
 
    cout << "Try to create the existing collection 0 ... ";
    physicalName =  collType+"|"+collConnection+"|"+"Collection0";
    try{
       if(crude) {
	  CollectionDescription	desc( physicalName, "" ); 
	  collection = factory->create(/*"PHYSICAL_NAME",*/ desc, ICollection::CREATE);
       } else {
	  CollectionDescription	desc( "Collection0", collType, collConnection ); 
	  collection = factory->createAndRegister( desc, 0 );
       }
      cout << "ERROR: this shouldn't work" << endl;
    }catch( std::exception&){
      cout << "OK, exception has been thrown" << endl;
    }
    
    

    cout << "Try to replace the replicated collection 1 ... ";
    physicalName =  collType+"|"+collConnection+"|"+"Collection1";
    try{
       if(crude) {
   	  CollectionDescription	desc( physicalName, "" ); 
	  collection = factory->create(/*"PHYSICAL_NAME",*/ desc, ICollection::CREATE_AND_OVERWRITE);
       } else {
    	  CollectionDescription	desc( "Collection1", collType, collConnection ); 
	  collection = factory->createAndRegister( desc, 0, true, "", collMetaData );
       }
       cout << "ERROR: this shouldn't work" << endl;
    }catch( std::exception&){
      cout << "OK, exception has been thrown" << endl;
    }
    
    
    
    cout << "Try to replace unique collection 0 ... ";
    physicalName =  collType+"|"+collConnection+"|"+"Collection0";
    try{
       if(crude) {
   	  CollectionDescription	desc( physicalName, "" ); 
	  collection = factory->create(/*"PHYSICAL_NAME",*/ desc, ICollection::CREATE_AND_OVERWRITE);
       } else {
     	  CollectionDescription	desc( "Collection0", collType, collConnection ); 
	  collection = factory->createAndRegister( desc, 0, true, "", collMetaData );
       }
       delete collection;
       collection = nullptr;
       cout << "OK, this works" << endl;
    }catch( std::exception& e){
       cout << "ERROR, this should work" << endl;
       std::cout << "Exception: "<<e.what()<<std::endl;
    }
    

    cout << "Try to update the NON replicated collection 0 ... ";
    physicalName =  collType+"|"+collConnection+"|"+"Collection0";
    try{
       if(crude) {
    	  CollectionDescription	desc( physicalName, "" ); 
	  collection = factory->create(/*"PHYSICAL_NAME",*/ desc, ICollection::UPDATE);
       } else 
	  collection = factory->openWithPhysicalName( physicalName, 0, 
						       ICollection::UPDATE );
       cout << "OK, this works" << endl;
       delete collection;
       collection = nullptr;
    }catch( std::exception&e ){
       cout << "ERROR, this should work" << endl;
       std::cout << "Exception: "<<e.what()<<std::endl; 
    }
   
    cout << "Try to update the replicated collection 1 ... ";
    try{
       if(crude) {
    	  CollectionDescription	desc( "logicalName1", "" ); 
	  collection = factory->create(/*"LOGICAL_NAME",*/ desc, ICollection::UPDATE);
       } else 
	  collection = factory->openWithLogicalName( "logicalName1", 0, // logical name 
						     false );        // readOnly
       cout << "ERROR: this shouldn't work" << endl;
    }catch( std::exception&){
      cout << "OK, exception has been thrown" << endl;
    }
    
    
    
    cout << "Try to update unique collection 0 ... ";
    try{
       if(crude) {
	  CollectionDescription	desc( "logicalName0", "" ); 
	  collection = factory->create(/*"LOGICAL_NAME",*/ desc, ICollection::UPDATE);
       } else
	  collection = factory->openWithLogicalName( "logicalName0", 0,// logical name 
						     false );        // readOnly
       cout << "OK it works" << endl;
       delete collection;
       collection = nullptr;
    }catch( std::exception&){
      cout << "ERROR, this should work" << endl;
    }
    
    
  }catch( std::exception& e ){
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
  cout << "releasing token" << endl;
  token->release();
  cout << "exiting" << endl;
  return 0;
}
