/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "UserDatabase.h"
#include "DatabaseHandler.h"
#include "TechnologyDispatcher.h"
#include "MicroSessionManager.h"
#include "DatabaseRegistry.h"
#include "PersistencySvcConfiguration.h"
#include "PersistencySvc/PersistencySvcException.h"
#include "PersistencySvc/DatabaseConnectionPolicy.h"
#include "PersistencySvc/ITransaction.h"
#include "StorageSvc/DbType.h"
#include "StorageSvc/pool.h"
#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/IFCAction.h"
#include "CoralBase/MessageStream.h"

static const std::string& emptyString = "";

pool::PersistencySvc::UserDatabase::UserDatabase( pool::PersistencySvc::TechnologyDispatcher& technologyDispatcher,
                                                  pool::PersistencySvc::PersistencySvcConfiguration& configuration,
                                                  pool::ITransaction& transaction,
                                                  pool::PersistencySvc::DatabaseRegistry& registry,
                                                  const std::string& name,
                                                  pool::DatabaseSpecification::NameType nameType ):
  m_technologyDispatcher( technologyDispatcher ),
  m_configuration( configuration ),
  m_transaction( transaction ),
  m_registry( registry ),
  m_name( name ),
  m_nameType( nameType ),
  m_technology( 0 ),
  m_technologySet( false ),
  m_databaseHandler( 0 ),
  m_openMode( pool::IDatabase::CLOSED ),
  m_alreadyConnected( false ),
  m_the_fid( "" ),
  m_the_pfn( "" )
{
  this->checkInRegistry();
}


pool::PersistencySvc::UserDatabase::~UserDatabase()
{}


pool::PersistencySvc::DatabaseHandler&
pool::PersistencySvc::UserDatabase::databaseHandler()
{
  return *m_databaseHandler;
}

void
pool::PersistencySvc::UserDatabase::connectForRead()
{
  this->connectForRead( m_configuration.databaseConnectionPolicy() );
}

void
pool::PersistencySvc::UserDatabase::connectForRead( const pool::DatabaseConnectionPolicy& policy )
{
  if ( ( ! m_databaseHandler ) &&
       m_transaction.isActive() ) {
    // Check if the database is already connected
    if ( ! this->checkInRegistry() ) {

      // It is not. Connect !
      switch( m_nameType ) {
      case pool::DatabaseSpecification::PFN:
        if ( this->fid().empty() ) {
          throw pool::PersistencySvcException( "PFN \"" + m_name + "\" is not existing",
                                               "PersistencySvc::UserDatabase::connectForRead" );
        }
        break;

      case pool::DatabaseSpecification::FID:
        if ( this->pfn().empty() ) {
          throw pool::PersistencySvcException( "FID \"" + m_name + "\" is not existing in the catalog",
                                               "PersistencySvc::UserDatabase::connectForRead" );
        }
        break;
      case pool::DatabaseSpecification::LFN:
        {
          std::string lfn = m_name;
          if ( this->fid().empty() ) {
            throw pool::PersistencySvcException( "LFN \"" + m_name + "\" is not existing in the catalog",
                                                 "PersistencySvc::UserDatabase::connectForRead" );
          }
          this->connectForRead( policy );
          m_registry.registerDatabaseHandler( m_databaseHandler, lfn );
        }
      return;
      break;
      default:
        throw pool::PersistencySvcException( "Unknown database name type",
                                             "PersistencySvc::UserDatabase::connectForRead" );
      };

      // Now we have all the usefull information to open the file.
      pool::PersistencySvc::MicroSessionManager& sessionManager = m_technologyDispatcher.microSessionManager( m_technology );
      long accessMode = pool::READ;
      if ( m_transaction.type() == pool::ITransaction::UPDATE &&
           policy.readMode() == pool::DatabaseConnectionPolicy::UPDATE ) {
        accessMode = pool::UPDATE;
      }
      m_databaseHandler = sessionManager.connect( m_the_fid, m_the_pfn, accessMode );
      if ( m_databaseHandler ) {
        m_openMode = ( ( accessMode == pool::READ ) ? pool::IDatabase::READ : pool::IDatabase::UPDATE );
      }
      else {
        throw pool::PersistencySvcException( "Could not connect to the file",
                                             "PersistencySvc::UserDatabase::connectForRead" );
      }
    }
  }
}


void
pool::PersistencySvc::UserDatabase::connectForWrite()
{
  this->connectForWrite( m_configuration.databaseConnectionPolicy() );
}

void
pool::PersistencySvc::UserDatabase::connectForWrite( const pool::DatabaseConnectionPolicy& policy )
{
  if ( ( ! m_databaseHandler ) &&
       m_transaction.isActive() ) {
    if ( m_transaction.type() != pool::ITransaction::UPDATE ) {
      throw pool::PersistencySvcException( "Could not open a database for write outside an update transaction.",
                                           "PersistencySvc::UserDatabase::connectForWrite" );
    }

    if ( this->checkInRegistry() ) {
      if ( m_openMode == pool::IDatabase::READ ) {
        this->revertMode( pool::IDatabase::UPDATE );
      }
    }
    else { // The database is not yet connected.
      long accessMode = pool::UPDATE;
      bool dbRegistered = false;
      switch( m_nameType ) {
      case pool::DatabaseSpecification::PFN:
        m_the_pfn = m_name;
        if ( this->fid().empty() ) {
          if( policy.writeModeForNonExisting() == pool::DatabaseConnectionPolicy::RAISE_ERROR ) {
            throw pool::PersistencySvcException( "Could not find the PFN \"" + m_name + "\" in the file catalog",
                                                 "PersistencySvc::UserDatabase::connectForWrite" );
          }
	  // Check if the technology is already set
	  if( ! m_technologySet ) {
	     throw pool::PersistencySvcException( "The back end technology has not been specified",
						  "PersistencySvc::UserDatabase::connectForWrite" );
	  }
	  // register in the catalog 
	  pool::DbType dbType( m_technology );
	  pool::DbType dbTypeMajor( dbType.majorType() );
	  pool::IFileCatalog& fileCatalog = m_configuration.fileCatalog();
	  pool::IFCAction action;
	  fileCatalog.setAction( action );
	  action.registerPFN( m_the_pfn, dbTypeMajor.storageName(), m_the_fid );
	  dbRegistered = true;
	  accessMode = pool::CREATE;
        }
        else {
	   if( policy.writeModeForExisting() == pool::DatabaseConnectionPolicy::RAISE_ERROR ) {
	      throw pool::PersistencySvcException( "The PFN \"" + m_name + "\" already exists",
						   "PersistencySvc::UserDatabase::connectForWrite" );
	   }
	}
	// MN: change - set overwrite mode even if it the databas was not registered in a catalog
	if( policy.writeModeForExisting() == pool::DatabaseConnectionPolicy::OVERWRITE ) {
	   accessMode = pool::RECREATE;
	}
        break;
      case pool::DatabaseSpecification::FID:
        m_the_fid = m_name;
        if ( this->pfn().empty() ) {
          throw pool::PersistencySvcException( "Could not find the FID \"" + m_name + "\" in the file catalog",
                                               "PersistencySvc::UserDatabase::connectForWrite" );
        }
        if ( policy.writeModeForExisting() == pool::DatabaseConnectionPolicy::RAISE_ERROR ) {
          throw pool::PersistencySvcException( "The FID \"" + m_name + "\" already exists",
                                               "PersistencySvc::UserDatabase::connectForWrite" );
        }
        else if ( policy.writeModeForExisting() == pool::DatabaseConnectionPolicy::OVERWRITE ) {
          accessMode = pool::RECREATE;
        }
        break;
      case pool::DatabaseSpecification::LFN:
        {
          std::string lfn = m_name;
          if ( this->fid().empty() ) {
            throw pool::PersistencySvcException( "Could not find the LFN \"" + m_name  + "\" in the file catalog",
                                                 "PersistencySvc::UserDatabase::connectForWrite" );
          }
          if ( policy.writeModeForExisting() == pool::DatabaseConnectionPolicy::RAISE_ERROR ) {
            throw pool::PersistencySvcException( "The LFN \"" + m_name + "\" already exists",
                                                 "PersistencySvc::UserDatabase::connectForWrite" );
          }
          this->connectForWrite( policy );
          m_registry.registerDatabaseHandler( m_databaseHandler, lfn );
        }
      return;
      break;
      default:
        throw pool::PersistencySvcException( "Unknown database name type",
                                             "PersistencySvc::UserDatabase::connectForWrite" );
      };

      m_databaseHandler = m_technologyDispatcher.microSessionManager( m_technology ).connect( m_the_fid, m_the_pfn, accessMode );
      if( !m_databaseHandler ) {
	 if( dbRegistered ) {
	    // creation failed, remove entry from the in-memory catalog
	    pool::FCAdmin action;
	    m_configuration.fileCatalog().setAction( action );
	    action.deleteEntry( m_the_fid );
	 }
	 throw pool::PersistencySvcException( "Could not connect to the file",
					      "PersistencySvc::UserDatabase::connectForWrite" );
      }
      m_openMode = pool::IDatabase::UPDATE;
      
    } // Connection established

  } // Database handler retrieved
}


void
pool::PersistencySvc::UserDatabase::disconnect()
{
  if ( m_databaseHandler ) {
    m_technologyDispatcher.microSessionManager( m_technology ).disconnect( m_databaseHandler );
    m_openMode = pool::IDatabase::CLOSED;
  }
}


pool::IDatabase::OpenMode
pool::PersistencySvc::UserDatabase::openMode() const
{
  return m_openMode;
}


void
pool::PersistencySvc::UserDatabase::revertMode( pool::IDatabase::OpenMode mode )
{
  if ( m_databaseHandler ) {
    switch ( mode ){
    case pool::IDatabase::CLOSED:
      this->disconnect();
      break;
    case pool::IDatabase::READ:
      if ( m_openMode == pool::IDatabase::UPDATE ) {
        m_databaseHandler->reconnect( pool::READ );
        m_openMode = pool::IDatabase::READ;
      }
      break;
    case pool::IDatabase::UPDATE:
      if ( m_openMode == pool::IDatabase::READ ) {
        m_databaseHandler->reconnect( pool::UPDATE );
        m_openMode = pool::IDatabase::UPDATE;
      }
      break;
    default:
      break;
    };
  }
}


const std::string&
pool::PersistencySvc::UserDatabase::fid() const
{
  if ( m_databaseHandler ) return m_databaseHandler->fid();
  else {
    if ( m_nameType == pool::DatabaseSpecification::FID ) return m_name;
    else if ( ! m_the_fid.empty() ) return m_the_fid;
    else {
      pool::IFileCatalog& fileCatalog = m_configuration.fileCatalog();
      if ( m_nameType == pool::DatabaseSpecification::PFN ) {
        std::string technology;
        pool::IFCAction action;
        fileCatalog.setAction( action );
        action.lookupFileByPFN( m_name, m_the_fid, technology );
        if ( ! m_the_fid.empty() ) {
          m_the_pfn = m_name;
          this->setTechnologyIdentifier( technology );
          m_alreadyConnected = true;
        }
        else {
           if( m_transaction.type() != pool::ITransaction::UPDATE ) { // Fetch the FID from the db itself !
              if( !m_technologySet ) {
                 coral::MessageStream log( "PersistencySvc::UserDatabase::fid()" );
                 log << coral::Debug << "Opening database '" << m_name
                     << "' with no catalog entry and no technology set - assuming ROOT storage"
                     << coral::MessageStream::endmsg;
                 m_technology = pool::ROOT_StorageType.type();
                 m_technologySet = true;
              }
              pool::PersistencySvc::MicroSessionManager& sessionManager = m_technologyDispatcher.microSessionManager( m_technology );
              m_the_fid = sessionManager.fidForPfn( m_name );
              if ( ! m_the_fid.empty() ) {
                 m_the_pfn = m_name;
                 m_alreadyConnected = true;
              }
           }
        }
      } /* not PFN */
      else if ( m_nameType == pool::DatabaseSpecification::LFN ) {
        pool::IFCAction action;
        fileCatalog.setAction( action );
        action.lookupFileByLFN( m_name, m_the_fid );
      }
      if ( ! m_the_fid.empty() ) {
        m_name = m_the_fid;
        m_nameType = pool::DatabaseSpecification::FID;
      }
      return m_the_fid;
    }
    return emptyString;
  }
}


const std::string&
pool::PersistencySvc::UserDatabase::pfn() const
{
  if ( m_databaseHandler ) return m_databaseHandler->pfn();
  else {
    if ( m_nameType == pool::DatabaseSpecification::PFN ) return m_name;
    else if ( ! m_the_pfn.empty() ) return m_the_pfn;
    else {
      pool::IFileCatalog& fileCatalog = m_configuration.fileCatalog();
      if ( m_nameType == pool::DatabaseSpecification::FID ) {
        std::string technology;
        pool::IFCAction action;
        fileCatalog.setAction( action );
        action.lookupBestPFN( m_name, pool::FileCatalog::READ,
                              pool::FileCatalog::SEQUENTIAL,
                              m_the_pfn, technology );
        if ( ! m_the_pfn.empty() ) {
          m_the_fid = m_name;
          this->setTechnologyIdentifier( technology );
          m_alreadyConnected = true;
        }
      }
      else if ( m_nameType == pool::DatabaseSpecification::LFN ) {
        pool::IFCAction action;
        fileCatalog.setAction( action );
        action.lookupFileByLFN( m_name, m_the_fid );
        if ( ! m_the_fid.empty() ) {
          std::string technology;
          action.lookupBestPFN( m_the_fid, pool::FileCatalog::READ,
                                pool::FileCatalog::SEQUENTIAL,
                                m_the_pfn, technology );
          if ( ! m_the_pfn.empty() ) {
            this->setTechnologyIdentifier( technology );
            m_alreadyConnected = true;
            m_name = m_the_fid;
            m_nameType = pool::DatabaseSpecification::FID;
          }
        }
      }
      return m_the_pfn;
    }
    return emptyString;
  }
}


bool
pool::PersistencySvc::UserDatabase::setTechnology( long technology )
{
  if ( m_alreadyConnected ) return false;
  else {
    pool::DbType dbType( technology );
    m_technology = dbType.majorType();
    m_technologySet = true;
    return true;
  }
}


long
pool::PersistencySvc::UserDatabase::technology() const
{
  return m_technology;
}


std::vector< std::string >
pool::PersistencySvc::UserDatabase::containers()
{
  std::vector< std::string > containers;
  if ( m_databaseHandler ) {
    containers = m_databaseHandler->containers();
  }
  return containers;
}


pool::IContainer*
pool::PersistencySvc::UserDatabase::containerHandle( const std::string& name )
{
  pool::IContainer* container = 0;
  if ( m_databaseHandler ) {
    container = m_databaseHandler->container( name );
  }
  return container;
}


bool
pool::PersistencySvc::UserDatabase::checkInRegistry()
{
  // Check first if the database is already connected.
  switch( m_nameType ) {
  case pool::DatabaseSpecification::PFN:
    m_databaseHandler = m_registry.lookupByPFN( m_name );
    break;
  case pool::DatabaseSpecification::FID:
    m_databaseHandler = m_registry.lookupByFID( m_name );
    break;
  case pool::DatabaseSpecification::LFN:
    m_databaseHandler = m_registry.lookupByLFN( m_name );
    break;
  default:
    throw pool::PersistencySvcException( "Only PFN, LFN and FID database types are currently supported",
                                         "PersistencySvc::UserDatabase::checkInRegistry" );
  };
  if ( m_databaseHandler ) {
    m_alreadyConnected = true;
    m_technology = m_databaseHandler->technology();
    if ( m_databaseHandler->accessMode() == pool::READ ) {
      m_openMode = pool::IDatabase::READ;
    }
    else {
      m_openMode = pool::IDatabase::UPDATE;
    }
    m_the_fid = m_name = m_databaseHandler->fid();
    m_the_pfn = m_databaseHandler->pfn();
    m_nameType = pool::DatabaseSpecification::FID;
    return true;
  }
  else return false;
}


void
pool::PersistencySvc::UserDatabase::setTechnologyIdentifier( const std::string& sTechnology ) const
{
  m_technology = pool::DbType::getType( sTechnology ).majorType();
}


const pool::ITechnologySpecificAttributes&
pool::PersistencySvc::UserDatabase::technologySpecificAttributes() const
{
  return static_cast< const pool::ITechnologySpecificAttributes& >( *this );
}


pool::ITechnologySpecificAttributes&
pool::PersistencySvc::UserDatabase::technologySpecificAttributes()
{
  return static_cast< pool::ITechnologySpecificAttributes& >( *this );
}


bool
pool::PersistencySvc::UserDatabase::attributeOfType( const std::string& attributeName,
                                                     void* data,
                                                     const std::type_info& typeInfo,
                                                     const std::string& option ) const
{
  if ( ! m_databaseHandler ) return false;
  else return m_databaseHandler->attribute( attributeName, data, typeInfo, option );
}


bool
pool::PersistencySvc::UserDatabase::setAttributeOfType( const std::string& attributeName,
                                                        const void* data,
                                                        const std::type_info& typeInfo,
                                                        const std::string& option )
{
  if ( ! m_databaseHandler ) return false;
  else return m_databaseHandler->setAttribute( attributeName, data, typeInfo, option );
}


const pool::IDatabaseParameters&
pool::PersistencySvc::UserDatabase::parameters() const
{
  return static_cast< const pool::IDatabaseParameters& >( *this );
}

pool::IDatabaseParameters&
pool::PersistencySvc::UserDatabase::parameters()
{
  return static_cast< pool::IDatabaseParameters& >( *this );
}


std::set< std::string >
pool::PersistencySvc::UserDatabase::parameterNames() const
{
  if ( ! m_databaseHandler ) return std::set< std::string >();
  else return m_databaseHandler->parameters();
}

std::string
pool::PersistencySvc::UserDatabase::value( const std::string& name ) const
{
  if ( ! m_databaseHandler ) return std::string( "" );
  else return m_databaseHandler->parameterValue( name );
}

void
pool::PersistencySvc::UserDatabase::addParameter( const std::string& name, const std::string& value )
{
  if ( m_databaseHandler )
    m_databaseHandler->addParameter( name, value );
}
