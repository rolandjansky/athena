/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ImplicitCollection.h"
#include "ImplicitCollectionIterator.h"
#include "ImplicitCollectionMetadata.h"

#include "PersistencySvc/ISession.h"
#include "PersistencySvc/IDatabase.h"
#include "PersistencySvc/IContainer.h"
#include "PersistencySvc/PersistencySvcException.h"

#include "POOLCore/Exception.h"
#include "CoralBase/MessageStream.h"

#include "StorageSvc/DbType.h"

#include <sstream>
#include <memory>


namespace pool {

   ImplicitCollection::
   ImplicitCollection( ISession* session,
                       std::string connection,
                       std::string name,
                       ICollection::OpenMode mode )
         :
         m_container( 0 ),
         m_description( name,"ImplicitCollection", connection ),
         m_metadata( 0 )
   {
      open( mode, session );
   }


   ImplicitCollection::
   ImplicitCollection( const ICollectionDescription* description,
                       ICollection::OpenMode mode,
                       ISession* session )
         :
         m_container( 0 ),
         m_description( *description ),
         m_metadata( 0 )
   {
      open( mode, session );
   }


   void
   ImplicitCollection::
   open( ICollection::OpenMode mode,
         ISession* session )
   {
      coral::MessageStream log( "ImplicitCollection");

      if ( mode != ICollection::READ ) {
         log << coral::Error << "An implicit collection can be opened only in READ mode" << coral::MessageStream::endmsg;
         throw Exception( "An implicit collection can be opened only in READ mode",
                          "ImplicitCollection::ImplicitCollection",
                          "ImplicitCollection" );
      }

      DatabaseSpecification::NameType dbNameType = DatabaseSpecification::UNDEFINED;

      // parse the connection string
      const std::string& connection = m_description.connection();
      std::string::size_type pos = connection.find( ":" );
      if ( pos == std::string::npos ) {
         log << coral::Error << "Badly formed connection string : \"" << connection << "\"" << coral::MessageStream::endmsg;
         throw Exception( "Badly formed connection string",
                          "ImplicitCollection::ImplicitCollection",
                          "ImplicitCollection" );
      }

      const std::string dbType = connection.substr( 0, pos );

      if ( dbType == "PFN" ) dbNameType = DatabaseSpecification::PFN;
      else if ( dbType == "LFN" ) dbNameType = DatabaseSpecification::LFN;
      else if ( dbType == "FID" ) dbNameType = DatabaseSpecification::FID;
      else {
         log << coral::Error << "Unrecognizable database name type : \"" << dbType << "\"" << coral::MessageStream::endmsg;
         throw Exception( "Unrecognizable database name type : " + dbType,
                          "ImplicitCollection::ImplicitCollection",
                          "ImplicitCollection" );
      }

      std::string dbName = "";
      std::string technologyName = "";
      std::istringstream is( connection.substr( pos + 1 ).c_str() );
      is >> dbName >> technologyName;

      if ( dbName.empty() ) {
         log << coral::Error << "Invalid database name " << coral::MessageStream::endmsg;
         throw Exception( "Invalid database name",
                          "ImplicitCollection::ImplicitCollection",
                          "ImplicitCollection" );
      }
  
      if( !session ) {
         throw Exception( "session object not set",
                          "ImplicitCollection::ImplicitCollection",
                          "ImplicitCollection" );
      }
  
      IDatabase* database = session->databaseHandle( dbName, dbNameType );
      if( !database ) {
         throw Exception( "Could not retrieve a database handle",
                          "ImplicitCollection::ImplicitCollection",
                          "ImplicitCollection" );
      }

      if ( database->openMode() == IDatabase::CLOSED ) {
         // The following fix was added to allow the reading of an implicit 
         // collection in the absence of a POOL file catalog. For now, it assumes 
         // a ROOT persistency storage type if no other type can be found. 
         try {
            database->connectForRead();
         }
         catch ( PersistencySvcException& /* exception */ ) {
	    // use provided tech name or assume ROOT
            DbType theDbType = (technologyName != "") ? DbType::getType( technologyName ) : ROOT_StorageType;
	    // setting tech will make connectForRead work without a catalog
            database->setTechnology( theDbType.type() );
            database->connectForRead();
         }
      }

      const std::string& name = m_description.name();
      std::vector< std::string > containers = database->containers();
      for( std::vector< std::string >::const_iterator iContainer = containers.begin();
           iContainer != containers.end(); ++iContainer ) {
         if( name == *iContainer ) {
            m_container = database->containerHandle( name );
            break;
         }
      }
      delete database;

      if( !m_container ) {
         throw Exception( "Could not open the container " + name,
                          "ImplicitCollection::ImplicitCollection",
                          "ImplicitCollection" );
      }

      log << coral::Info << "Opened the implicit collection with connection string \""
          << connection << "\"" << coral::MessageStream::endmsg
          << "and a name \"" << name << "\"" << coral::MessageStream::endmsg;
   }


   
   ImplicitCollection::~ImplicitCollection()
   {
      delete m_container;
      delete m_metadata;
   }

   

   ICollection::OpenMode 
   ImplicitCollection::openMode() const{
      return ICollection::READ;
   }


   // old method implemented for backward compatibility
   // and maybe also for the ease of use?
   ImplicitCollectionIterator*
   ImplicitCollection::select( std::string primaryQuery,
                               std::string,
                               std::string )
   {
      // iterator object supporting the collection query interface
      std::auto_ptr<ImplicitCollectionIterator>
	 iterquery( new ImplicitCollectionIterator( *m_container, m_description ) );
      iterquery->setCondition( primaryQuery );
      iterquery->execute();
      return iterquery.release();
   }



   void
   ImplicitCollection::commit(bool)
   {
   }


   void
   ImplicitCollection::rollback()
   {
   }


   void
   ImplicitCollection::close()
   {
      // can't be closed
   }


   void
   ImplicitCollection::open()
   {
      // hmm, no-op at the moment  //MN
   }

   bool
   ImplicitCollection::isOpen() const
   {
      return true;
   }

   const ICollectionDescription& ImplicitCollection::description() const
   {
      return m_description;
   }

      
   ICollectionQuery* ImplicitCollection::newQuery() const
   {
      return new ImplicitCollectionIterator( *m_container, m_description ); 
   }

   ICollectionSchemaEditor& ImplicitCollection::schemaEditor()
   {
      std::string errorMsg = "Cannot modify an ImplicitCollection.";
      throw Exception( errorMsg,
                       "ImplicitCollection::schemaEditor",
                       "ImplicitCollection" );
      // NOT REACHED
   }

   ICollectionDataEditor& ImplicitCollection::dataEditor()
   {
      std::string errorMsg = "Cannot modify an ImplicitCollection.";
      throw Exception( errorMsg,
                       "ImplicitCollection::dataEditor",
                       "ImplicitCollection" );
      // NOT REACHED
   }

   ICollectionMetadata& ImplicitCollection::metadata()
   {
      if( !m_metadata ) {
         m_metadata = new ImplicitCollectionMetadata();
      }
      return *m_metadata;
   } 

   void methodNotImplemented(const std::string &method)
   {
      throw Exception( std::string("method not implemented"),
                       std::string("ImplicitCollection::") + method,
                       "ImplicitCollection" );
   }

   bool
   ImplicitCollection::exists( const std::string& /*name*/, bool, bool ) const
   {
      methodNotImplemented("exists");
      return false;
   }
      
   bool
   ImplicitCollection::drop( const std::string&, bool, bool)
   {
      methodNotImplemented("drop");
      return false;
   }
      
   bool
   ImplicitCollection::rename( const std::string&, const std::string&)
   {
      methodNotImplemented("rename");
      return false;
   }
      
   bool
   ImplicitCollection::grantToUser( const std::string&,
                                ICollection::Privilege,
                                const std::string&,
                                bool)
   {
      methodNotImplemented("grantToUser");
      return false;
   }
      
   bool
   ImplicitCollection::revokeFromUser( const std::string&,
                                   ICollection::Privilege,
                                   const std::string&,
                                   bool)
   {
      methodNotImplemented("revokeFromUser");
      return false;
   }
      
   bool
   ImplicitCollection::grantToPublic( const std::string&, bool)
   {
      methodNotImplemented("grantToPublic");
      return false;
   }
            
   bool
   ImplicitCollection::revokeFromPublic( const std::string&, bool )
   {
      methodNotImplemented("revokeFromPublic");
      return false;
   }      
}
