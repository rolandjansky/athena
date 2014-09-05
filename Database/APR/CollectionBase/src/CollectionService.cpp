/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CollectionBase/CollectionService.h"
#include "CollectionBase/CollectionFactory.h"
#include "CollectionBase/CollectionDescription.h"
#include "CollectionBase/ICollectionCursor.h"

#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/FCEntry.h"

#include "CoralBase/MessageStream.h"

namespace pool { class ISession; }

#include "POOLCore/Exception.h"

#include "POOLCore/boost_tokenizer_headers.h"
#include <cstring>

#include <iostream>
using namespace std;
using namespace pool;

static std::string	moduleName("CollectionService");

pool::CollectionService::CollectionService( ) 
{
   pool::CollectionFactory::get()->setOutputLevel();
}


pool::CollectionService::~CollectionService()
{
}


pool::ICollection*
pool::CollectionService::create( const pool::ICollectionDescription& description, bool overwrite )
{
   if( description.name().empty() ) {
      std::string errorMsg = "Must specify name of collection in description input argument.";
      throw pool::Exception( errorMsg,
			     "CollectionService::create", 
			     "CollectionService" );
   }

   if( description.type().empty() ) {
      std::string errorMsg = "Must specify type of collection in description input argument.";
      throw pool::Exception( errorMsg,
			     "CollectionService::create", 
			     "CollectionService" );
   }

   if ( description.type() == "ImplicitCollection" )  {
      std::string errorMsg = 
         "Can only open a collection of type 'ImplicitCollection' for read transtions.";
      throw pool::Exception( errorMsg,
			     "CollectionService::create", 
			     "CollectionService" );
   }

   pool::ICollection::OpenMode openMode = overwrite? pool::ICollection::CREATE_AND_OVERWRITE : pool::ICollection::CREATE;
   return  pool::CollectionFactory::get()->create( description, openMode );
}


pool::ICollection*
pool::CollectionService::createAndRegister( const pool::ICollectionDescription& description,
                                            bool overwrite,
                                            std::string logicalName,
                                            MetaDataEntry* metadata )
{
   if( description.name().empty() )  {
      std::string errorMsg = "Must specify name of collection in description input argument.";
      throw pool::Exception( errorMsg,
			     "CollectionService::createAndRegister", 
			     "CollectionService" );
   }

   if( description.type().empty() )  {
      std::string errorMsg = "Must specify type of collection in description input argument.";
      throw pool::Exception( errorMsg,
			     "CollectionService::createAndRegister", 
			     "CollectionService" );
   }

   if ( description.type() == "ImplicitCollection" )  {
      std::string errorMsg = "Cannot register a collection of type 'ImplicitCollection' in a collection catalog.";
      throw pool::Exception( errorMsg,
			     "CollectionService::createAndRegister", 
			     "CollectionService" );
   }

   return CollectionFactory::get()->createAndRegister( description,
						       0,
						       overwrite,
						       logicalName,
						       metadata );
}


bool
pool::CollectionService::registerExisting( const std::string& name,
                                           const std::string& type,
                                           std::string connection,
                                           std::string logicalName,
                                           MetaDataEntry* metadata )
{
   if( name.empty() )  {
    std::string errorMsg = "Must specify name of collection as input.";
    throw pool::Exception( errorMsg,
                           "CollectionService::registerExisting", 
                           "CollectionService" );
  }

   if( type.empty() ) {
    std::string errorMsg = "Must specify type of collection as input.";
    throw pool::Exception( errorMsg,
                           "CollectionService::registerExisting", 
                           "CollectionService" );
  }

  if( type == "ImplicitCollection" )  {
    std::string errorMsg = "Cannot register a collection of type 'ImplicitCollection' in a collection catalog.";
    throw pool::Exception( errorMsg,
                           "CollectionService::registerExisting",
                           "CollectionService" );
  }

  CollectionDescription description( name, type, connection );

  return CollectionFactory::get()->registerExisting( description,
						     0,
						     logicalName,
						     metadata );
}



bool
pool::CollectionService::registerExisting( ICollection* collection,
					   bool overwrite,
                                           std::string logicalName,
                                           MetaDataEntry* metadata )
{
  return CollectionFactory::get()->registerExisting( collection,
						     overwrite,
						     0,
						     logicalName,
						     metadata );
}


pool::ICollection* 
pool::CollectionService::handle( const std::string& name,
                                 const std::string& type,
                                 std::string connection,
                                 bool readOnly,
                                 pool::ISession* session ) const
{
   if( ( type == "ImplicitCollection ") && (! readOnly ) )  {
      std::string errorMsg = "Cannot open a collection of type 'ImplicitCollection' for updates.";
      throw pool::Exception( errorMsg,
                             "CollectionService::handle",
                             "CollectionService" );
   }
   pool::CollectionDescription description( name, type, connection );
   pool::ICollection::OpenMode openMode = readOnly? ICollection::READ : ICollection::UPDATE;
   return CollectionFactory::get()->create( description, openMode, session );
}


pool::ICollection* 
pool::CollectionService::openWithPhysicalName( const std::string& physicalName, ICollection::OpenMode openMode )
{
  return pool::CollectionFactory::get()->openWithPhysicalName( physicalName, 0, openMode );
}


pool::ICollection* 
pool::CollectionService::openWithLogicalName( const std::string& logicalName, bool readOnly )
{
  return pool::CollectionFactory::get()->openWithLogicalName( logicalName, 0, readOnly );
}


pool::ICollection* 
pool::CollectionService::openWithGuid( const std::string& guid, bool readOnly )
{
   return pool::CollectionFactory::get()->openWithGuid( guid, 0, readOnly );
}


bool
pool::CollectionService::exists( const std::string& name,
                                 const std::string& type,
                                 std::string connection,
                                 bool setForUpdate,
                                 bool checkChildFragments ) const
{
   ICollection* collection = handle( name, type, connection );
   bool res = collection->exists( name, setForUpdate, checkChildFragments );
   delete collection;
   return res;
}


bool
pool::CollectionService::drop( const std::string& name,
                               const std::string& type,
                               std::string connection,
                               bool dropChildFragments,
                               bool ignoreExternalDependencies )
{
   ICollection* collection = handle( name, type, connection, false );
   bool res = collection->drop( collection->description().name(),  dropChildFragments, ignoreExternalDependencies );
   delete collection;
   return res;
}


bool
pool::CollectionService::rename( const std::string& oldName,
                                 const std::string& newName,
                                 const std::string& type,
                                 std::string connection )
{
   return this->handle( oldName, type, connection, false )->rename( oldName, newName );
}


bool
pool::CollectionService::grantToUser( const std::string& userName,
                                      pool::ICollection::Privilege privilege,
                                      const std::string& name,
                                      const std::string& type,
                                      std::string connection,
                                      bool grantForChildFragments )
{
   return this->handle( name, type, connection, false )->grantToUser( userName, 
                                                                      privilege, 
                                                                      name, 
                                                                      grantForChildFragments );
}


bool
pool::CollectionService::revokeFromUser( const std::string& userName,
                                         pool::ICollection::Privilege privilege,
                                         const std::string& name,
                                         const std::string& type,
                                         std::string connection,
                                         bool revokeForChildFragments )
{
  return this->handle( name, type, connection, false )->revokeFromUser( userName, 
                                                                        privilege, 
                                                                        name, 
                                                                        revokeForChildFragments );
}


bool
pool::CollectionService::grantToPublic( const std::string& name,
                                        const std::string& type,
                                        std::string connection,
                                        bool grantForChildFragments )
{
  return this->handle( name, type, connection, false )->grantToPublic( name, grantForChildFragments );
}


bool
pool::CollectionService::revokeFromPublic( const std::string& name,
                                           const std::string& type,
                                           std::string connection,
                                           bool revokeForChildFragments )
{
  return this->handle( name, type, connection, false )->revokeFromPublic( name, revokeForChildFragments );
}



bool
pool::CollectionService::setDefaultCatalog()
{
   CollectionFactory::get()->setDefaultCatalog(0);  // reset so get() gets the default
   return (CollectionFactory::get()->getDefaultCatalog() != 0);  // reset so get() gets the default
}



bool 
pool::CollectionService::setWriteCatalog( const std::string& name )
{
   CollectionFactory::get()->setWriteCatalog( name ); 
   return true;
}


bool
pool::CollectionService::addReadCatalog( const std::string& name )
{
   IFileCatalog *fc = CollectionFactory::get()->getDefaultCatalog();
   fc->disconnect();
   fc->addReadCatalog( name );
   fc->connect();
   return true;
}


pool::IFileCatalog*
pool::CollectionService::getCatalog( )
{
   return CollectionFactory::get()->getDefaultCatalog();
}




void
pool::CollectionService::setCatalog( IFileCatalog* collCat )
{
   CollectionFactory::get()->setDefaultCatalog( collCat );   
}



pool::ICollectionCursor*
pool::CollectionService::getCatalogCollectionCursor( std::string collectionLevelQuery,
	                                             std::string rowLevelQuery,
	                                             std::vector<std::string>* tokenOutputList,
	                                             std::vector<std::string>* attribOutputList,
	                                             int rowCacheSize ) const
{
   return pool::CollectionFactory::get()->getCatalogCollectionCursor( 0,
								      collectionLevelQuery,
								      rowLevelQuery,
								      tokenOutputList,
								      attribOutputList,
								      rowCacheSize );
}
