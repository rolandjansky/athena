/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CollectionBase/CollectionFactory.h"
#include "CollectionBase/CollectionDescription.h"
#include "CollectionBase/ICollectionCursor.h"
#include "CollectionBase/CatalogCollectionCursor.h"
#include "CollectionBase/ICollectionMetadata.h"
#include "CollectionBase/CollectionBaseNames.h"

#include "FileCatalog/FCEntry.h"
#include "FileCatalog/IFCContainer.h"
#include "FileCatalog/IFCAction.h"
#include "FileCatalog/FCException.h"
#include "FileCatalog/IFileCatalog.h"

#include "Gaudi/PluginService.h"

#include "PersistencySvc/ISession.h"
#include "POOLCore/Exception.h"
#include "POOLCore/boost_tokenizer_headers.h"

#include "CoralBase/MessageStream.h"

#include "PersistentDataModel/Guid.h"
#include <cstring>


using namespace std;
using namespace pool;

pool::CollectionFactory pool::CollectionFactory::s_instance;
static string	thisModule( "POOLCollFactory" );

const std::string pool::CollectionFactory::c_fileType = "PoolCollection";


pool::CollectionFactory::CollectionFactory()
      : m_myOwnCatalog( false ),
        m_catalog( 0 ) 
{ }


pool::CollectionFactory::~CollectionFactory()
{
   if( m_myOwnCatalog ) delete m_catalog; 
}


pool::CollectionFactory* pool::CollectionFactory::get()
{
  return &s_instance;
}



pool::ICollection*
pool::CollectionFactory::create_callPlugin( const pool::ICollectionDescription& description,
					    pool::ICollection::OpenMode openMode,
					    pool::ISession* session ) const
{
   std::string type( description.type() );
   ICollection *coll = Gaudi::PluginService::Factory<ICollection*, const ICollectionDescription*, ICollection::OpenMode, ISession*>::create( type, &description, openMode, session );
   if( !coll ) {
      std::string errorMsg = "APR::CollectionFactory::create(" + type + "," + description.name() + ") FAILED!  Plugin for that collection technology could not be loaded.";
      if( type == "MemoryCollection" ) {
         errorMsg = "Collection type <MemoryCollection> is not supported";
      }
      throw pool::Exception( errorMsg,
                             "CollectionFactory::create",
                             "CollectionBase" );
   }
   return coll;
}
   


// resolve PHYSICAL_NAME, LOGICAL_NAME and GUID notation (using catalog)
void
CollectionFactory::resolveDescription( CollectionDescription& description, bool readOnly ) const
{
   string type = description.type();
   string name = description.name();
   if( type == "LOGICAL_NAME" ) {
      description = descFromLogicalName( name, 0, readOnly);
   } else if( type == "GUID" ) {
      description = descFromGuid( name, 0, readOnly );
   } else if( type == "PHYSICAL_NAME" ) {
      CollectionDescription	tmpdesc( descFromPhysicalName( name, 0, readOnly ) );
      // the original description content is needed for CREATE 
      description.setName( tmpdesc.name() );
      description.setType( tmpdesc.type() ); 
      description.setConnection( tmpdesc.connection() ); 
   }
}

         

pool::ICollection*
pool::CollectionFactory::create( const ICollectionDescription& _description,
                                 ICollection::OpenMode openMode,
                                 ISession* session ) const
{
   // local copy of the description to allow modifications
   CollectionDescription	description( _description );
   // resolve PHYSICAL_NAME, LOGICAL_NAME and GUID notation (using catalog)
   resolveDescription( description, openMode==ICollection::READ );

   ICollection *coll = create_callPlugin( description, openMode, session );

   if( coll && (openMode == ICollection::CREATE ||
                openMode == ICollection::CREATE_AND_OVERWRITE ) ) {      
      // generate and add collection ID
      coral::MessageStream log( thisModule ); 
      Guid	coll_id;
      Guid::create( coll_id );
      string	guid_str( coll_id.toString() );
      log << coral::Debug << "Generated new ID for collection " <<  description.name()
	  << " GUID=" << guid_str << endl
	  << coral::MessageStream::endmsg;
      coll->metadata().setValueForKey( CollectionBaseNames::CollIDMdataKey(), guid_str );
      coll->commit( true );
   }
   return coll;
}





pool::ICollection*
pool::CollectionFactory::createAndRegister( const pool::ICollectionDescription& _description,
                                            pool::IFileCatalog* collectionCatalog,
                                            bool overwrite,
                                            std::string logicalName,
                                            pool::MetaDataEntry* metadata,
                                            pool::ISession* session ) const
{
  if( !_description.hasEventReferenceColumn() )  {
    std::string errorMsg = "Cannot register a collection fragment in a collection catalog. Must add an event reference columnto collection fragment `" + _description.name() + "' to make it a collection.";
    throw pool::Exception( errorMsg,
                           "CollectionFactory::createAndRegister",
                           "CollectionBase" );
  }

  std::string physicalName;
  CollectionDescription	description( _description );
  if( description.type() == "PHYSICAL_NAME" ) {
     physicalName = description.name(); // should be already a type|connection|name
     std::string type, connection, name;     
     extract( physicalName, type, connection, name );
     description.setName(name);
     description.setType(type); 
     description.setConnection(connection); 
  } else {
     physicalName = description.type() + "|" + description.connection() + "|" + description.name();
  }
  
  if( !collectionCatalog) collectionCatalog = getDefaultCatalog();
  pool::FileCatalog::FileID guid;
  std::string fileType;
  pool::IFCAction fcAction;
  collectionCatalog->setAction( fcAction );
  collectionCatalog->start();
  fcAction.lookupFileByPFN( physicalName, guid, fileType );
  collectionCatalog->commit();
  
  if( guid.length() )  {
     if( overwrite )    {
	if( !isUnique( guid, *collectionCatalog ) ) {
	   std::string errorMsg = "Cannot overwrite collection `" + description.name() + "' because it has been replicated.";
	   throw pool::Exception( errorMsg,
				  "CollectionFactory::createAndRegister", 
				  "CollectionBase" );
	}
     } 
     else {
	std::string errorMsg = "User did not authorize overwrite of registered collection `" + description.name() + "'.";
	throw pool::Exception( errorMsg,
			       "CollectionFactory::createAndRegister", 
			       "CollectionBase" );
     }
  }  

  ICollection::OpenMode openMode = overwrite ? ICollection::CREATE_AND_OVERWRITE : ICollection::CREATE;

  pool::ICollection* collection = create_callPlugin( description, openMode, session );
  
  pool::FCregister fcRegister;
  collectionCatalog->setAction( fcRegister );
  collectionCatalog->start();
  if( !guid.length() ) {
     fcRegister.registerPFN( physicalName, c_fileType, guid );
  }
  if( logicalName.length() ) {
     try {
        fcRegister.registerLFN( physicalName, logicalName );
     } 
     catch( const pool::FCduplicateLFNException& )
     {}
  }
  if( metadata ) {
     fcRegister.registerMetaData( guid, *metadata );
  }
  // add collection ID
  collection->metadata().setValueForKey( CollectionBaseNames::CollIDMdataKey(), guid );
  collection->commit( true ); 
  collectionCatalog->commit();

  return collection;
}


bool 
pool::CollectionFactory::registerExisting( const pool::ICollectionDescription& description,
                                           pool::IFileCatalog* collectionCatalog,
                                           std::string logicalName,
                                           pool::MetaDataEntry* metadata,
                                           pool::ISession* session ) const
{
   if( !description.hasEventReferenceColumn() )  {
      std::string errorMsg = "Cannot register a collection fragment in a collection catalog.";
      throw pool::Exception( errorMsg,
                             "CollectionFactory::registerExisting",
                             "CollectionBase" );
   }

   if( !collectionCatalog) collectionCatalog = getDefaultCatalog();
   std::string physicalName = 
      description.type() + "|" + description.connection() + "|" + description.name();

   coral::MessageStream log( thisModule ); 
   log << coral::Debug << "Registering collection PFN=" << physicalName
       << ", LFN=" << logicalName  << coral::MessageStream::endmsg;

   pool::ICollection* collection = openWithPhysicalName( physicalName,
                                                         collectionCatalog,
                                                         pool::ICollection::READ,
                                                         session );

   if( !collection || !collection->exists( description.name(), false, true ) )  {
      std::string errorMsg = "Collection with physical name `" + physicalName + 
         "' does not exist. Cannot register it in collection catalog.";
      throw pool::Exception( errorMsg,
                             "CollectionFactory::registerExisting", 
                             "CollectionBase" );
   }
   bool	has_id = collection->metadata().existsKey( CollectionBaseNames::CollIDMdataKey() );
   if( !has_id ) {
      // no GUID in this collection (old collection?)
      // see if we can open it for update
      try {
	 pool::ICollection* collection_upd = openWithPhysicalName( physicalName,
								   collectionCatalog,
								   pool::ICollection::UPDATE,
								   session );
	 collection->close();
	 delete collection;
	 collection = collection_upd;
      }
      catch( pool::Exception& e) {
	 // update mode did not work
	 coral::MessageStream log( thisModule ); 
	 log << coral::Warning << "Could not open collection " << physicalName
	     << " for update, to store the new GUID. The error was: "  << e.what()
	     << coral::MessageStream::endmsg;
      }
   }

   bool overwrite( true );
   bool rc = registerExisting( collection, overwrite, collectionCatalog, logicalName, metadata, session );
   delete collection;   collection = 0;
   return rc;
}


bool 
pool::CollectionFactory::registerExisting( pool::ICollection* collection,
					   bool overwrite,
                                           pool::IFileCatalog* collectionCatalog,
                                           std::string logicalName,
                                           pool::MetaDataEntry* metadata,
                                           pool::ISession* ) const
{
   if( !collectionCatalog) collectionCatalog = getDefaultCatalog();
   pool::FileCatalog::FileID guid;
   std::string fileType;
   pool::IFCAction fcAction;
   std::string physicalName = collection->description().type() + "|" + collection->description().connection() + "|" + collection->description().name();
  
   coral::MessageStream log( thisModule ); 
   log << coral::Debug << "Registering existing collection PFN=" << physicalName
       << ", LFN=" << logicalName  << coral::MessageStream::endmsg;

   collectionCatalog->setAction( fcAction );
   collectionCatalog->start();
   fcAction.lookupFileByPFN( physicalName, guid, fileType );
   collectionCatalog->commit();

   if( !collection->isOpen() )
      collection->open();
   bool	has_id = collection->metadata().existsKey( CollectionBaseNames::CollIDMdataKey() );
   string coll_id;
   if( has_id ) {
      coll_id = collection->metadata().getValueForKey( CollectionBaseNames::CollIDMdataKey() );
   }

   log << coral::Debug << " ---  found catalog guid=" << guid << coral::MessageStream::endmsg;
   log << coral::Debug << " ---     collection guid=" << coll_id << coral::MessageStream::endmsg;
   
   if( guid.length() )  {
      if( guid == coll_id )
	 // already in the catalog, nothing to do
	 return true;

      if( !overwrite ) {
	 std::string errorMsg = "Collection with physical name `" + physicalName + "' is already registered in collection catalog.";
	 throw pool::Exception( errorMsg,
				"CollectionFactory::registerExisting",
				"CollectionBase" );
      }
      // attempt to remove the existing GUID from the catalog (no GUID reuse)
      log << coral::Info  << " -- removing " << guid << " from the catalog " <<  coral::MessageStream::endmsg;
      FCAdmin	fcAction;
      collectionCatalog->setAction( fcAction );
      collectionCatalog->start();
      fcAction.deleteEntry( guid );
      collectionCatalog->commit();
   }

   // if the collection has a GUID already, we will register it
   // - if not, the catalog will generate one
   guid = coll_id;
   
   pool::FCregister fcRegister;
   collectionCatalog->setAction( fcRegister );
   collectionCatalog->start();
   fcRegister.registerPFN( physicalName, c_fileType, guid );
   if( logicalName.length() )  {
      try    {
	 fcRegister.registerLFN( physicalName, logicalName );
      } 
      catch( const pool::FCduplicateLFNException& ){};
   }
   if( metadata )  {
      fcRegister.registerMetaData( guid, *metadata );
   }
   collectionCatalog->commit();
   if( !has_id && collection->openMode() != ICollection::READ ) try {
      // try to update the collection ID with the one generated by the catalog
      collection->metadata().setValueForKey( CollectionBaseNames::CollIDMdataKey(), guid );
      collection->commit();
   } catch( pool::Exception& e) {
      log << coral::Warning << "Failed to update collection ID for collection " << collection->description().name() <<  ".  The error was: " << e.what() <<  coral::MessageStream::endmsg;
      // return false;
   }
   
  return true;
}


// --   Open a collection using catalog ---

// PHYSICAL_NAME does not require a catalog entry because all info is provided

pool::ICollection* 
pool::CollectionFactory::openWithPhysicalName( const std::string& physicalName,
                                               pool::IFileCatalog* collectionCatalog,
                                               pool::ICollection::OpenMode openMode,
                                               pool::ISession* session ) const
{
   CollectionDescription description( descFromPhysicalName( physicalName, collectionCatalog, openMode==ICollection::READ) );
   return create_callPlugin( description, openMode, session );
}


pool::ICollection* 
pool::CollectionFactory::openWithLogicalName( const std::string& logicalName,
                                              pool::IFileCatalog* collectionCatalog,
                                              bool readOnly,
                                              pool::ISession* session ) const
{
   CollectionDescription description( descFromLogicalName( logicalName, collectionCatalog, readOnly ) );
   ICollection::OpenMode openMode = readOnly? ICollection::READ : ICollection::UPDATE;
   return create_callPlugin( description, openMode, session );
}



pool::ICollection* 
pool::CollectionFactory::openWithGuid( const pool::FileCatalog::FileID& guid,
                                       pool::IFileCatalog* collectionCatalog,
                                       bool readOnly,
                                       pool::ISession* session ) const
{
  CollectionDescription description( descFromGuid(guid, collectionCatalog, readOnly) ); 
  ICollection::OpenMode openMode = readOnly? ICollection::READ : ICollection::UPDATE;
  return create_callPlugin( description, openMode, session );
}



CollectionDescription
CollectionFactory::descFromPhysicalName( const std::string& physicalName,
                                         pool::IFileCatalog* collectionCatalog,
                                         bool readOnly ) const
{
   if( !collectionCatalog ) collectionCatalog = getDefaultCatalog();
   FileCatalog::FileID guid;
   string       fileType;
   IFCAction    fcAction;
   collectionCatalog->setAction( fcAction );
   collectionCatalog->start();
   fcAction.lookupFileByPFN( physicalName, guid, fileType );
   collectionCatalog->commit();

   if( guid.length() )  {
      return descFromGuid( guid, collectionCatalog, readOnly );
   }

   std::string type, connection, name;
   extract( physicalName, type, connection, name );  
   return CollectionDescription(name, type, connection);
}  

      
CollectionDescription
CollectionFactory::descFromLogicalName( const std::string& logicalName,
                                        pool::IFileCatalog* collectionCatalog,
                                        bool readOnly ) const
{
  if( !collectionCatalog ) collectionCatalog = getDefaultCatalog();
  pool::FileCatalog::FileID guid;
  pool::IFCAction fcAction;
  collectionCatalog->setAction( fcAction );
  collectionCatalog->start();
  fcAction.lookupFileByLFN( logicalName, guid );
  collectionCatalog->commit();
  if( !guid.length() )  {
     std::string errorMsg = "No collection registered with logical name `" + logicalName + "' in  collection catalog.";
     throw pool::Exception( errorMsg,
                            "CollectionFactory::openWithLogicalName",
                            "CollectionBase" );
  }
  return descFromGuid( guid, collectionCatalog, readOnly );
}
                                        


CollectionDescription
CollectionFactory::descFromGuid( const pool::FileCatalog::FileID& guid,
                                 pool::IFileCatalog* collectionCatalog,
                                 bool readOnly ) const
{
  if( !collectionCatalog ) collectionCatalog = getDefaultCatalog();

  if( !readOnly && !isUnique( guid, *collectionCatalog ) ) {
     throw pool::Exception( "Cannot open collection with GUID `" + guid + "' for updates because it has been replicated.",
			    "CollectionFactory::openWithGuid", 
			    "CollectionBase" );
  }

  pool::PFNContainer pfns( collectionCatalog );
  pool::FClookup fcLookup;
  collectionCatalog->setAction( fcLookup );
  collectionCatalog->start();
  fcLookup.lookupPFN( guid, pfns );
  bool found_PFN = pfns.hasNext();
  collectionCatalog->commit();
  if( !found_PFN )  {
     throw pool::Exception( "No collection registered with GUID `" + guid + "' in collection catalog.",
                           "CollectionFactory::openWithGuid", 
                           "CollectionBase" );
  }


  std::string physicalName;
  std::string fileType;
  pool::IFCAction fcAction;
  collectionCatalog->setAction( fcAction );
  collectionCatalog->start();
  fcAction.lookupBestPFN( guid,
                          readOnly ? pool::FileCatalog::READ : pool::FileCatalog::UPDATE,
                          pool::FileCatalog::SEQUENTIAL, // ...?
                          physicalName,
                          fileType );
  collectionCatalog->commit();
  
  if ( fileType != c_fileType )  {
     std::string errorMsg = "Entries in collection catalog must be of type `" + c_fileType + "'.";
     throw pool::Exception( errorMsg,
			    "CollectionFactory::openWithGuid",
			    "CollectionBase" );
  }
  
  std::string name, type, connection;
  extract( physicalName, type, connection, name );  
  return CollectionDescription( name, type, connection ); 
}



pool::ICollectionCursor* 
pool::CollectionFactory::getCatalogCollectionCursor( pool::IFileCatalog* collectionCatalog,
                                                     std::string collectionLevelQuery,
                                                     std::string rowLevelQuery,
                                                     std::vector<std::string>* tokenOutputList,
                                                     std::vector<std::string>* attribOutputList,
                                                     int rowCacheSize,
                                                     pool::ISession* session ) const
{
  if( !collectionCatalog) collectionCatalog = getDefaultCatalog();
  return new pool::CatalogCollectionCursor( *collectionCatalog,
                                            collectionLevelQuery,
                                            rowLevelQuery,
                                            tokenOutputList,
                                            attribOutputList,
                                            rowCacheSize,
                                            session );
}


bool 
pool::CollectionFactory::isUnique( const pool::FileCatalog::FileID& guid,
                                   pool::IFileCatalog& collectionCatalog ) const
{
  pool::PFNContainer pfns( &collectionCatalog );
  pool::FClookup fcLookup;
  collectionCatalog.setAction( fcLookup );
  collectionCatalog.start();
  fcLookup.lookupPFN( guid, pfns );
  if( !pfns.hasNext() )
  {
    collectionCatalog.commit();
    std::string errorMsg = "A Collection with GUID `" + guid + "' is not registered in the collection catalog.";
    throw pool::Exception( errorMsg,
                           "CollectionFactory::isUnique", 
                           "CollectionBase" );
  }
  PFNEntry x =   pfns.Next();
  //cout << "FCLookup for:" << guid << endl;
  //cout << " *****  PFN= " << x.pfname() << "  guid=" << x.guid() << endl;
  bool result = !pfns.hasNext();

  /*
  while( pfns.hasNext() ) {
     x = pfns.Next();
     cout << " *****  extra PFN= " << x.pfname()  << "  guid=" << x.guid() <<  endl;
  }
  */
  collectionCatalog.commit();

  return result;
}


void 
pool::CollectionFactory::extract( const std::string physicalName,
                                  std::string& type,
                                  std::string& connection,
                                  std::string& name ) const
{
  typedef boost::tokenizer<boost::char_separator<char> > Tizer;
  boost::char_separator<char> sep( "|", "", boost::keep_empty_tokens );
  Tizer tizer( physicalName, sep);
  Tizer::iterator token=tizer.begin(); 
  type = *token;
  connection = *(++token);
  name = *(++token);
}




void 
pool::CollectionFactory::setDefaultCatalog( pool::IFileCatalog* catalog)
{
   if( m_catalog && m_myOwnCatalog ) {
      m_catalog->disconnect();
      delete m_catalog;
   }
   m_catalog = catalog; 
   m_myOwnCatalog=false;  
}


void 
pool::CollectionFactory::setWriteCatalog( const std::string &cat )
{
   if( !m_catalog ) {
      m_catalog = new IFileCatalog();
      m_myOwnCatalog = true;
   }
   m_catalog->setWriteCatalog( cat );
}
  

pool::IFileCatalog* 
pool::CollectionFactory::getDefaultCatalog() const
{  
   if( m_catalog )
      return m_catalog;
  
   m_catalog = new IFileCatalog();
   m_myOwnCatalog = true;
   
   bool ok = false;  
   const char* chr = getenv("POOL_COLLECTION_WRITE_CATALOG");
   if(chr && strlen(chr)){
      coral::MessageStream log( thisModule );
      log << coral::Debug << "setting write collection catalog: " << chr
	  << coral::MessageStream::endmsg;
      m_catalog->setWriteCatalog( chr );
      ok = true;
   }
   chr = getenv("POOL_COLLECTION_READ_CATALOGS");
   if(chr && strlen(chr)){
      coral::MessageStream log( thisModule );
      std::string str = chr;
      typedef boost::tokenizer<boost::char_separator<char> > Tizer;
      boost::char_separator<char> sep(" ,");
      Tizer tizer( str, sep);
      for( Tizer::iterator token=tizer.begin();
	   token!=tizer.end();
	   ++token){
	 log << coral::Debug << "adding read collection catalog: " << *token
	     << coral::MessageStream::endmsg;
	 m_catalog->addReadCatalog( *token );
      }
      ok = true;
   }
   if(!ok)
      m_catalog->setWriteCatalog("xmlcatalog_file:CollectionCatalog.xml");
  
   m_catalog->connect();

   return m_catalog;
}



void
pool::CollectionFactory::setOutputLevel() const
{
   const char* out_levl = getenv("POOL_OUTMSG_LEVEL");
   if( out_levl ) {
      coral::MsgLevel       mLvL = coral::Info;
      std::string outLvl( out_levl );
      for( size_t i = 0; i < outLvl.size(); i++ )
         outLvl[i] = tolower( outLvl[i] );
      if( outLvl == "verbose" ) {
         mLvL = coral::Verbose;
      } else
         if( outLvl == "debug" ) {
            mLvL = coral::Debug;
         } else
            if( outLvl == "info" ) {
               mLvL = coral::Info;
            } else
               if( outLvl == "warning" ) {
                  mLvL = coral::Warning;
               } else
                  if( outLvl == "error" ) {
                     mLvL = coral::Error;
                  } else
                     if( outLvl == "fatal" ) {
                        mLvL = coral::Fatal;
                     } else
                        if( outLvl == "always" ) {
                           mLvL = coral::Always;
                        }
      coral::MessageStream::setMsgVerbosity( mLvL );
   }
}
