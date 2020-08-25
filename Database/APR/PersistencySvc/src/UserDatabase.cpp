/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "UserDatabase.h"
#include "DatabaseHandler.h"
#include "TechnologyDispatcher.h"
#include "MicroSessionManager.h"
#include "DatabaseRegistry.h"
#include "PersistencySvc/PersistencySvcException.h"
#include "PersistencySvc/DatabaseConnectionPolicy.h"
#include "PersistencySvc/ITransaction.h"
#include "StorageSvc/DbType.h"
#include "StorageSvc/pool.h"
#include "FileCatalog/IFileCatalog.h"
#include "POOLCore/DbPrint.h"


static const std::string& emptyString = "";

pool::PersistencySvc::UserDatabase::UserDatabase( pool::PersistencySvc::TechnologyDispatcher& technologyDispatcher,
                                                  const pool::DatabaseConnectionPolicy& policy,
                                                  IFileCatalog& catalog,
                                                  pool::ITransaction& transaction,
                                                  pool::PersistencySvc::DatabaseRegistry& registry,
                                                  const std::string& name,
                                                  pool::DatabaseSpecification::NameType nameType ):
  m_technologyDispatcher( technologyDispatcher ),
  m_policy( policy ),
  m_catalog( catalog ),
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
  this->connectForRead( m_policy );
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
        if( m_nameType == DatabaseSpecification::LFN ) {
           // fid() found the PFN and FID but no tech
           // retry assuming the name was really an LFN - need to clear the_fid for that
           m_the_fid.clear();
           connectForRead( policy );
           return;
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
  this->connectForWrite( m_policy );
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
         m_databaseHandler->reconnect( pool::UPDATE );
         m_openMode = pool::IDatabase::UPDATE;
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
	  m_catalog.registerPFN( m_the_pfn.substr(0, m_the_pfn.find("?")), dbTypeMajor.storageName(), m_the_fid );
          DbPrint log("PersistencySvc::UserDB::connectForWrite()" );
          log << DbPrintLvl::Debug << "registered PFN: " << m_the_pfn << " with FID:" << m_the_fid << endmsg;
	  dbRegistered = true;
	  if( policy.writeModeForExisting() == pool::DatabaseConnectionPolicy::OVERWRITE ) {
	     accessMode = pool::CREATE;
	  }
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
	    m_catalog.deleteFID( m_the_fid );
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


const std::string&
pool::PersistencySvc::UserDatabase::fid() const
{
  if ( m_databaseHandler ) return m_databaseHandler->fid();
  else {
    if ( m_nameType == pool::DatabaseSpecification::FID ) return m_name;
    else if ( ! m_the_fid.empty() ) return m_the_fid;
    else {
      if ( m_nameType == pool::DatabaseSpecification::PFN ) {
         std::string technology;
         m_catalog.lookupFileByPFN( m_name.substr(0, m_name.find("?")), m_the_fid, technology );
         DbPrint log("PersistencySvc::UserDB::fid()" );
         log << DbPrintLvl::Debug << "lookupPFN: " << m_name << " returned FID: '" << m_the_fid << "'"
             << " tech=" << technology << endmsg;
         if ( ! m_the_fid.empty() ) {
            if( technology.empty() ) {
               m_nameType = DatabaseSpecification::LFN;
               log << DbPrintLvl::Debug << "Retrying 'connect' using assumed PFN " << m_name
                   << " as LFN (no tech found in PFC)" << endmsg;
               return m_the_fid;
            }
            m_the_pfn = m_name;
            this->setTechnologyIdentifier( technology );
            m_alreadyConnected = true;
         }
         else {
           if( m_transaction.type() != pool::ITransaction::UPDATE ) { // Fetch the FID from the db itself !
              if( !m_technologySet ) {
                 log << DbPrintLvl::Debug << "Opening database '" << m_name
                     << "' with no catalog entry and no technology set - assuming ROOT storage" << endmsg;
                 m_technology = pool::ROOT_StorageType.type();
                 m_technologySet = true;
              }
              pool::PersistencySvc::MicroSessionManager& sessionManager = m_technologyDispatcher.microSessionManager( m_technology );
              m_the_fid = sessionManager.fidForPfn( m_name );
              if( ! m_the_fid.empty() ) {
                 // sanity check - verify that the FID is not registered in PFC under a different name
                 std::string  pfn, tech;
                 m_catalog.getFirstPFN( m_the_fid, pfn, tech );
                 if( !pfn.empty() ) {
                    log << DbPrintLvl::Warning << "Opening file '" << m_name << "' which is already registered in the Catalog as '" << pfn <<"' (GUID " << m_the_fid << ") - this is not supported and may even lead to a crash!" << endmsg;
                 }
                 m_the_pfn = m_name;
                 m_alreadyConnected = true;
              }
           }
        }
      } /* not PFN */
      else if ( m_nameType == pool::DatabaseSpecification::LFN ) {
         m_the_fid = m_catalog.lookupLFN( m_name );
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
  if( m_databaseHandler )  return m_databaseHandler->pfn();
  if( m_nameType == pool::DatabaseSpecification::PFN )  return m_name;
  if( ! m_the_pfn.empty() )  return m_the_pfn;
  
  if( m_nameType == pool::DatabaseSpecification::LFN ) {
     m_the_fid = m_catalog.lookupLFN( m_name );
     if ( ! m_the_fid.empty() ) {
        m_name = m_the_fid;
        m_nameType = pool::DatabaseSpecification::FID;
        // go from FID=>PFN in the next step
     }
  }
  if( m_nameType == pool::DatabaseSpecification::FID ) {
     std::string technology;
     m_catalog.getFirstPFN( m_name, m_the_pfn, technology );
     if( !m_the_pfn.empty() ) {
        m_the_fid = m_name;
        setTechnologyIdentifier( technology );
        m_alreadyConnected = true;
     }
     return m_the_pfn; 
  }
  return emptyString;
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
