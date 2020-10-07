/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RelationalCollection.h"
#include "RelationalCollectionNames.h"
#include "RelationalCollectionBindVariables.h"
#include "RelationalCollectionSchemaEditor.h"
#include "RelationalCollectionDataEditor.h"
#include "RelationalCollectionQuery.h"
#include "RelationalCollectionMetadata.h"
#include "DBLock.h"
#include "GUIDQuery.h"

#include "PersistentDataModel/Token.h"

#include "CollectionBase/ICollectionFragment.h"
#include "CollectionBase/ICollectionIndex.h"
#include "CollectionBase/ICollectionColumn.h"
#include "CollectionBase/ICollectionUniqueConstraint.h"
#include "CollectionBase/CollectionRowBuffer.h"
#include "CollectionBase/CollectionBaseNames.h"
#include "CollectionBase/ICollectionMetadata.h"

#include "POOLCore/Exception.h"

#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ISessionProperties.h"
#include "RelationalAccess/ConnectionService.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/TableDescription.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/IView.h"
#include "RelationalAccess/ITablePrivilegeManager.h"
#include "RelationalAccess/IConnectionServiceConfiguration.h"
#include "RelationalAccess/ITableDataEditor.h"

#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeSpecification.h"

#include <memory>
#include <iostream>
#include <cstdio> // For sprintf (bug #69271)
using namespace std;
using namespace pool;


pool::RelationalCollection::RelationalCollection::
RelationalCollection( const pool::ICollectionDescription* description,
                      pool::ICollection::OpenMode mode,
                      pool::ISession* )
      : m_description( *description ),
        m_mode( mode ),
        m_session( 0 ),
        m_name( description->name() ),
        m_schemaEditor( 0 ),
        m_dataEditor( 0 ),
        m_metadata( 0 ),
        m_whereClauseForCollectionNameInHeadersTable( RelationalCollectionBindVariables::whereClauseForCollectionNameInHeadersTable() ),
        m_whereClauseForChildCollectionNameInHeadersTable( RelationalCollectionBindVariables::whereClauseForChildCollectionNameInHeadersTable() ),
        m_whereClauseForCollectionNameInDescriptionsTable( RelationalCollectionBindVariables::whereClauseForCollectionNameInDescriptionsTable() ),
        m_whereClauseForCollectionNameInIndexDescriptionsTable( RelationalCollectionBindVariables::whereClauseForCollectionNameInIndexDescriptionsTable() ),
        m_whereClauseForIndexNameInIndexDescriptionsTable( RelationalCollectionBindVariables::whereClauseForIndexNameInIndexDescriptionsTable() ),
        m_whereDataForCollectionNameInHeadersTable( new coral::AttributeList ),
        m_whereDataForChildCollectionNameInHeadersTable( new coral::AttributeList ),
        m_whereDataForCollectionNameInDescriptionsTable( new coral::AttributeList ),
        m_whereDataForCollectionNameInIndexDescriptionsTable( new coral::AttributeList ),
        m_whereDataForIndexNameInIndexDescriptionsTable( new coral::AttributeList ),
        m_dataTableNameForCollectionFragmentName(),
        m_linksTableNameForCollectionFragmentName(),
        m_tableTokenColumnPrefixForCollectionTokenColumnName(),
        m_tableAttributeColumnNameForCollectionAttributeColumnName(),
        m_tableColumnNameForCollectionColumnName(),
        m_collectionColumnNameForTableColumnPosition(),
        m_tableAttributeList( new coral::AttributeList ),
        m_collectionRowBuffer(),
        m_dataTableRowBufferForCollectionFragmentName(),
        m_linksTableRowBuffer( new coral::AttributeList ),
        m_mapOfLinkIdForTokenKeyMaps(),
        m_mapOfTokenKeyForLinkIdMaps(),
        m_mapOfWhereClausesForOID1InDataTable(),
        m_mapOfWhereDataForOID1InDataTable(),
        m_initialized( false )
		    //, m_poolOut( "RelationalCollection", coral::Debug )

{
   // Define bind variable type used in where clause to find collection fragment name in collection headers table.
   m_whereDataForCollectionNameInHeadersTable->extend<std::string>( RelationalCollectionNames::collectionNameVariableInCollectionHeadersTable() );

   // Define bind variable type used in where clause to find child collection fragment name in collection headers table.
   m_whereDataForChildCollectionNameInHeadersTable->extend<std::string>( RelationalCollectionNames::childCollectionNameVariableInCollectionHeadersTable() );

   // Define bind variable type used in where clause to find collection fragment name in collection descriptions table.
   m_whereDataForCollectionNameInDescriptionsTable->extend<std::string>( RelationalCollectionNames::collectionNameVariableInCollectionDescriptionsTable() );

   // Define bind variable type used in where clause to find collection fragment name in collection index descriptions table.
   m_whereDataForCollectionNameInIndexDescriptionsTable->extend<std::string>( RelationalCollectionNames::collectionNameVariableInCollectionIndexDescriptionsTable() );

   // Define bind variable types used in where clause to find index name in collection index descriptions table.
   m_whereDataForIndexNameInIndexDescriptionsTable->extend<std::string>( RelationalCollectionNames::indexNameVariableInCollectionIndexDescriptionsTable() );
   m_whereDataForIndexNameInIndexDescriptionsTable->extend<std::string>( RelationalCollectionNames::collectionNameVariableInCollectionIndexDescriptionsTable() );

   // Initialize and open collection.
   open();
}


pool::RelationalCollection::RelationalCollection::~RelationalCollection()
{
  if( isOpen() ) close();

  delete m_schemaEditor;  m_schemaEditor = 0;
  delete m_dataEditor;  m_dataEditor = 0;
  delete m_metadata;   m_metadata = 0;
  
  delete m_whereDataForCollectionNameInHeadersTable;
  delete m_whereDataForChildCollectionNameInHeadersTable;
  delete m_whereDataForCollectionNameInDescriptionsTable;
  delete m_whereDataForCollectionNameInIndexDescriptionsTable;
  delete m_whereDataForIndexNameInIndexDescriptionsTable;
  delete m_tableAttributeList;
  delete m_linksTableRowBuffer;

  for ( std::map< std::string, coral::AttributeList* >::iterator iBuffer =
        m_dataTableRowBufferForCollectionFragmentName.begin(); iBuffer != 
        m_dataTableRowBufferForCollectionFragmentName.end(); ++iBuffer )
  {
    delete iBuffer->second;
  }

  for ( std::map< std::string, std::map< std::string, unsigned >* >::iterator iMap =
        m_mapOfLinkIdForTokenKeyMaps.begin(); iMap != m_mapOfLinkIdForTokenKeyMaps.end(); ++iMap )
  {
    delete iMap->second;
  }

  for ( std::map< std::string, std::map< unsigned, std::string >* >::iterator iMap =
        m_mapOfTokenKeyForLinkIdMaps.begin(); iMap != m_mapOfTokenKeyForLinkIdMaps.end(); ++iMap )
  {
    delete iMap->second;
  }

  for ( std::map< std::string, coral::AttributeList* >::iterator iList =
        m_mapOfWhereDataForOID1InDataTable.begin(); iList != 
        m_mapOfWhereDataForOID1InDataTable.end(); ++iList )
  {
    delete iList->second;
  }

//  m_poolOut << "Collection destructor finished" << coral::MessageStream::endmsg;
}


pool::ICollection::OpenMode
pool::RelationalCollection::RelationalCollection::openMode() const
{
  return m_mode;
}


void
pool::RelationalCollection::RelationalCollection::open()
{
  if( isOpen() ) return;

  // Get connection service.
  unique_ptr<coral::ConnectionService> connectionService( new coral::ConnectionService() );
  coral::MessageStream log( "pool::RelationalCollection::open" );
  // Retrieve a connection handle.
  bool readOnly = ( m_mode == pool::ICollection::READ );
  log << coral::Info << " Opening relational collection " << (readOnly? "in READ mode" : "") << coral::MessageStream::endmsg;
  try {
     coral::IConnectionServiceConfiguration& coralConf( connectionService->configuration() );

     const char *env = getenv("POOL_RCOLL_CONN_RETRY");
     int        retry = env? atoi(env) : 0;
     env = getenv("POOL_RCOLL_CONN_TIMEOUT");
     int        timeout = env? atoi(env) : 0;

     if( retry > 0 || timeout > 0 ) {
        if( retry > 0 ) {
           log << coral::Info
                     << "Setting Coral DB connection retry period to " << retry << " seconds" << coral::MessageStream::endmsg;
           coralConf.setConnectionRetrialPeriod( retry );
        }
        if( timeout > 0 ) {
           log << coral::Info
                     << "Setting Coral DB connection timeout " << timeout << " seconds" << coral::MessageStream::endmsg;
           coralConf.setConnectionRetrialTimeOut( timeout );
        }
     }


     m_session = connectionService->connect( m_description.connection(),
                                             (readOnly? coral::ReadOnly : coral::Update) );

     // Start a transaction.
     coral::ITransaction& transaction = m_session->transaction();
     if ( !transaction.isActive() ) {
        transaction.start( readOnly );
     }

     // Initialize collection if necessary.
     if( !m_initialized ) try {
        initialize();  
     } catch( pool::Exception& ) {
        if( m_session->transaction().isActive() )
           m_session->transaction().rollback();
        delete m_session;  m_session = 0;
        throw;
     }
     else {
        // reinitialize the pointer to the database schema
        m_dataEditor->m_schema = &m_session->nominalSchema();
     }
  
  } catch( coral::Exception &e ) {
     log << coral::Error << "  ***** Exception " << e.what() 
               << coral::MessageStream::endmsg;
     throw pool::Exception( e.what(),
                            "RelationalCollection::open",
                            "RelationalCollection" );
  }
}


bool
pool::RelationalCollection::RelationalCollection::isOpen() const
{
  return ( m_session != 0 );
}


void
pool::RelationalCollection::RelationalCollection::commit( bool restartTransaction )
{
   coral::ITransaction& transaction = m_session->transaction();
   if( !transaction.isActive() )
      return;
   if( m_dataEditor )
      m_dataEditor->flushCachedRows();
   transaction.commit();
   if( restartTransaction ) {
      bool readOnly = ( m_mode == pool::ICollection::READ );
      transaction.start( readOnly );
      if( m_metadata ) {
	 m_metadata->reinitialize( m_session );
      }
   }
}


void
pool::RelationalCollection::RelationalCollection::rollback()
{
   coral::ITransaction& transaction = m_session->transaction();
   if( !transaction.isActive() )
      return;
   m_dataEditor->deleteRowCache();
   transaction.rollback();

   bool readOnly = ( m_mode == pool::ICollection::READ );
   transaction.start( readOnly );
   m_dataEditor->createRowCache();
}


void
pool::RelationalCollection::RelationalCollection::close()
{
  if( m_session )  {
     coral::ITransaction& transaction = m_session->transaction();
     if( transaction.isActive() ) {
        m_dataEditor->deleteRowCache();
        transaction.rollback();
     }
     delete m_session;     m_session = 0;
  }
  delete m_metadata;  m_metadata = 0;
}


bool
pool::RelationalCollection::RelationalCollection::
exists( const std::string& fragmentName,
        bool setForUpdate,
        bool checkChildFragments ) const
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  // Check top level collection fragment by default.
  int minId = m_description.collectionFragment( fragmentName.size()? fragmentName : m_name ).id();
  int maxId = minId;
  if ( checkChildFragments )  {
    maxId = m_description.numberOfCollectionFragments() - 1;
  }

  for( int i = minId; i <= maxId; ++i )  {
    coral::IQuery* query = nominalSchema.tableHandle( RelationalCollectionNames::nameOfCollectionHeadersTable() ).newQuery();

    // Get collection fragment name.
    std::string fname = m_description.collectionFragment( i ).name();

    // Lock existing row in collection headers table for this collection fragment for duration of transaction.
    if( setForUpdate ) {
      query->setForUpdate();
    }

    // Set predicates and execute query.
    m_whereDataForCollectionNameInHeadersTable->begin()->data<std::string>() = fname;
    query->setCondition( m_whereClauseForCollectionNameInHeadersTable, *m_whereDataForCollectionNameInHeadersTable );
    query->limitReturnedRows( 1, 0 );
    coral::ICursor& cursor = query->execute();
    bool exists = cursor.next();
    delete query;

    if ( ! exists )    {
      coral::MessageStream log( "pool::RelationalCollection::RelationalCollection::exists()" );
      log << coral::Debug
          << "Collection fragment `" + fname + "' does not exist." << coral::MessageStream::endmsg;
      return false;
    }
  }

  return true;
}


bool
pool::RelationalCollection::RelationalCollection::
drop( const std::string& _fragmentName,
      bool dropChildFragments,     
      bool ignoreExternalDependencies )
{
   bool  docommit( true );
   return drop_impl( _fragmentName, dropChildFragments, ignoreExternalDependencies, docommit );
}


bool
pool::RelationalCollection::RelationalCollection::
drop_impl( const std::string& _fragmentName,
           bool dropChildFragments,
           bool ignoreExternalDependencies,
           bool do_commit )

{
   coral::ISchema& nominalSchema = m_session->nominalSchema();
   coral::MessageStream log( "pool::RelationalCollection::RelationalCollection::drop()" );

   // Drop top level collection fragment by default.
   string fragmentName = ( _fragmentName.size()? _fragmentName : m_name );
   int minId = m_description.collectionFragment( fragmentName ).id();
   int maxId = minId;
   if( dropChildFragments )  {
      maxId = m_description.numberOfCollectionFragments() - 1;
   }

   for ( int i = minId; i <= maxId; ++i )  {
      std::string fname = m_description.collectionFragment( i ).name();

      // Continue if collection fragment does not exist.
      if( !exists( fname, true ) ) {
         log << coral::Warning 
             << "Collection fragment `" + fname + "' does not exist."
             << coral::MessageStream::endmsg;
         continue;
      }

      // Drop foreign key constraint from child collection fragment if such constraint exists.
      dropChildForeignKeyConstraint( fname );

      // Find any dependencies on parent fragments not defined in the collection description.
      std::vector< std::string > parentFragmentNames;
      coral::ITable& headersTable = nominalSchema.tableHandle( RelationalCollectionNames::nameOfCollectionHeadersTable() );
      coral::IQuery* query = headersTable.newQuery();
      m_whereDataForChildCollectionNameInHeadersTable->begin()->data<std::string>() = fname;
      query->addToOutputList( RelationalCollectionNames::collectionNameVariableInCollectionHeadersTable() );
      query->setCondition( m_whereClauseForChildCollectionNameInHeadersTable, 
                           *m_whereDataForChildCollectionNameInHeadersTable );
      coral::ICursor& cursor = query->execute();
      while ( cursor.next() )  {
         const coral::AttributeList& row = cursor.currentRow();
         parentFragmentNames.push_back( row[0].data<std::string>() );
      }
      delete query;

      // If external dependencies are not be ignored stop dropping fragments after first one is found.
      if( parentFragmentNames.size() && !ignoreExternalDependencies ) {
         return false;
      }
    
      // If collection fragment is referenced by a foreign key, drop the foreign key.
      std::string foreignKeyName = m_description.collectionFragment( i ).foreignKeyName();
      if( foreignKeyName.size() )    {
         std::string childFragmentName = m_description.collectionFragment( i ).childCollectionFragmentName();
         std::string childDataTableName = ( m_dataTableNameForCollectionFragmentName.find( childFragmentName ) )->second;
         nominalSchema.tableHandle( childFragmentName ).schemaEditor().dropForeignKey( foreignKeyName );
      }

      // If collection fragment has parent fragments, update parent fragment rows in collection headers table.
      for (const std::string& name : parentFragmentNames )
      {
         m_whereDataForCollectionNameInHeadersTable->begin()->data<std::string>() = name;
         std::string setClause = RelationalCollectionNames::childCollectionNameVariableInCollectionHeadersTable() + " = " + "\'\'" + " , " + RelationalCollectionNames::foreignKeyNameVariableInCollectionHeadersTable() + " = " + "\'\'";
         nominalSchema.tableHandle( RelationalCollectionNames::nameOfCollectionHeadersTable() ).dataEditor().updateRows(
            setClause,
            m_whereClauseForCollectionNameInHeadersTable, 
            *m_whereDataForCollectionNameInHeadersTable );
      }

      // Drop data and links tables of collection.
      string &tableName = m_dataTableNameForCollectionFragmentName[ fname ];
      if( nominalSchema.existsTable( tableName) )
         nominalSchema.dropTable( tableName);
      tableName = m_linksTableNameForCollectionFragmentName[ fname ];
      if( nominalSchema.existsTable( tableName) )
         nominalSchema.dropTable( tableName );

      // Delete corresponding rows in collection descriptions table.
      m_whereDataForCollectionNameInDescriptionsTable->begin()->data<std::string>() = fname;
      nominalSchema.tableHandle( RelationalCollectionNames::nameOfCollectionDescriptionsTable() ).dataEditor().deleteRows( 
         m_whereClauseForCollectionNameInDescriptionsTable, *m_whereDataForCollectionNameInDescriptionsTable );

      // Delete corresponding row in collection headers table.
      m_whereDataForCollectionNameInHeadersTable->begin()->data<std::string>() = fname;
      nominalSchema.tableHandle( RelationalCollectionNames::nameOfCollectionHeadersTable() ).dataEditor().deleteRows( 
         m_whereClauseForCollectionNameInHeadersTable, *m_whereDataForCollectionNameInHeadersTable );

      // Drop collection fragment from collection description.
      if( m_schemaEditor ) {
         if( fragmentName != m_name ) {
            // do not call for the main fragment - it will not work
            // assume that we want to delete everything in such case and do not even touch the description
            m_schemaEditor->dropCollectionFragment( fname );
         }
      }
   }

   // Delete metadata
   metadata();   // force metadata initialization //hack around no delete in IMetadata.  MN
   int  metadata_deleted = m_metadata->deleteCollectionMetadata();
   log << coral::Debug
       << "Deleted " << metadata_deleted << " metadata entries for collection `" + fragmentName + "'."
       << coral::MessageStream::endmsg;

   if( do_commit ) commit( true );
   return true;
}


bool
pool::RelationalCollection::RelationalCollection::rename( const std::string& oldName,
                                                          const std::string& newName )
{
  if ( oldName == m_name )  {
    m_name = newName;
  }

  m_schemaEditor->renameCollectionFragment( oldName, newName );

  return true;
}


bool
pool::RelationalCollection::RelationalCollection::grantToUser( const std::string& userName,
                                                               pool::ICollection::Privilege privilege,
                                                               const std::string& fragmentName,
                                                               bool grantForChildFragments )
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  // Get data table privilege. Default is to grant select privilege.
  coral::ITablePrivilegeManager::Privilege dataTablePrivilege;
  switch ( privilege )
  {
    case UPDATE_ACCESS:
         dataTablePrivilege = coral::ITablePrivilegeManager::Update;
         break;
    case WRITE_ACCESS:
         dataTablePrivilege = coral::ITablePrivilegeManager::Insert;
         break;
    case DELETE_ACCESS:
         dataTablePrivilege = coral::ITablePrivilegeManager::Delete;
         break;
    default:
         dataTablePrivilege = coral::ITablePrivilegeManager::Select;
  }

  // Grant privilege for top level collection fragment by default.
  int minId = m_description.collectionFragment( fragmentName.size()? fragmentName : m_name ).id();
  int maxId = minId;
  if ( grantForChildFragments )  {
    maxId = m_description.numberOfCollectionFragments() - 1;
  }

  // Loop over requested collection fragments.
  for ( int i = minId; i <= maxId; ++i )  {
    // Get name of data table for this collection fragment.
    std::string dataTableName = ( m_dataTableNameForCollectionFragmentName.find( 
      m_description.collectionFragment( i ).name() ) )->second;

    // Grant privilege for this collection fragment.
    nominalSchema.tableHandle( dataTableName ).privilegeManager().grantToUser( userName, dataTablePrivilege );
  }

  return true;
}


bool
pool::RelationalCollection::RelationalCollection::revokeFromUser( const std::string& userName,
                                                                  pool::ICollection::Privilege privilege,
                                                                  const std::string& fragmentName,
                                                                  bool revokeForChildFragments )
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  // Get data table privilege. Default is to revoke delete privilege.
  coral::ITablePrivilegeManager::Privilege dataTablePrivilege;
  switch ( privilege) 
  {
    case READ_ACCESS:
         dataTablePrivilege = coral::ITablePrivilegeManager::Select;
         break;
    case UPDATE_ACCESS:
         dataTablePrivilege = coral::ITablePrivilegeManager::Update;
         break;
    case WRITE_ACCESS:
         dataTablePrivilege = coral::ITablePrivilegeManager::Insert;
         break;
    default:
         dataTablePrivilege = coral::ITablePrivilegeManager::Delete;
  }

  // Revoke privilege for top level collection fragment by default.
  int minId = m_description.collectionFragment( fragmentName.size()? fragmentName : m_name ).id();
  int maxId = minId;
  if ( revokeForChildFragments )
  {
    maxId = m_description.numberOfCollectionFragments() - 1;
  }

  // Loop over requested collection fragments.
  for ( int i = minId; i <= maxId; ++i )
  {
    // Get name of data table for this collection fragment.
    std::string dataTableName = ( m_dataTableNameForCollectionFragmentName.find( 
      m_description.collectionFragment( i ).name() ) )->second;

    // Revoke privilege for this collection fragment.
    nominalSchema.tableHandle( dataTableName ).privilegeManager().revokeFromUser( userName, dataTablePrivilege );
  }

  return true;
}


bool
pool::RelationalCollection::RelationalCollection::grantToPublic( const std::string& fragmentName, bool grantForChildFragments )
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  // Grant privilege for top level collection fragment by default.
  int minId = m_description.collectionFragment( fragmentName.size()? fragmentName : m_name ).id();
  int maxId = minId;
  if ( grantForChildFragments )
  {
    maxId = m_description.numberOfCollectionFragments() - 1;
  }

  // Loop over requested collection fragments.
  for ( int i = minId; i <= maxId; ++i )
  {
    // Get name of data table for this collection fragment.
    std::string dataTableName = ( m_dataTableNameForCollectionFragmentName.find( 
      m_description.collectionFragment( i ).name() ) )->second;

    // Grant privilege for this collection fragment.
    nominalSchema.tableHandle( dataTableName ).privilegeManager().grantToPublic( coral::ITablePrivilegeManager::Select );
  }

  return true;
}


bool
pool::RelationalCollection::RelationalCollection::revokeFromPublic( const std::string& fragmentName, bool revokeForChildFragments )
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  // Revoke privilege for top level collection fragment by default.
  int minId = m_description.collectionFragment( fragmentName.size()? fragmentName : m_name ).id();
  int maxId = minId;
  if ( revokeForChildFragments )
  {
    maxId = m_description.numberOfCollectionFragments() - 1;
  }

  // Loop over requested collection fragments.
  for ( int i = minId; i <= maxId; ++i )
  {
    // Get name of data table for this collection fragment.
    std::string dataTableName = ( m_dataTableNameForCollectionFragmentName.find( 
      m_description.collectionFragment( i ).name() ) )->second;

    // Revoke privilege for this collection fragment.
    nominalSchema.tableHandle( dataTableName ).privilegeManager().revokeFromPublic( coral::ITablePrivilegeManager::Select );
  }

  return true;
}


const pool::ICollectionDescription&
pool::RelationalCollection::RelationalCollection::description() const
{
  return m_description;
}
    

pool::ICollectionSchemaEditor&
pool::RelationalCollection::RelationalCollection::schemaEditor()
{
   if ( m_mode == pool::ICollection::READ )   {
      std::string errorMsg = "Cannot modify the schema of a collection in READ open mode.";
      throw pool::Exception( errorMsg,
                           "RelationalCollection::schemaEditor",
                           "RelationalCollection" );
   }
 
   return ( *m_schemaEditor );
}
    

pool::ICollectionDataEditor&
pool::RelationalCollection::RelationalCollection::dataEditor()
{
  if ( m_mode == pool::ICollection::READ )
  {
    std::string errorMsg = "Cannot modify the data of a collection in READ open mode.";
    throw pool::Exception( errorMsg,
                           "RelationalCollection::dataEditor",
                           "RelationalCollection" );
  }

  return ( *m_dataEditor );
}


pool::ICollectionQuery*
pool::RelationalCollection::RelationalCollection::newQuery() const
{
   return new RelationalCollectionQuery(
      *m_session,
      m_description,
      m_dataTableNameForCollectionFragmentName,
      m_linksTableNameForCollectionFragmentName,
      m_tableTokenColumnPrefixForCollectionTokenColumnName,
      m_tableAttributeColumnNameForCollectionAttributeColumnName,
      *m_tableAttributeList,
      m_mapOfLinkIdForTokenKeyMaps,
      m_mapOfTokenKeyForLinkIdMaps );
}


pool::ICollectionMetadata&
pool::RelationalCollection::RelationalCollection::metadata()
{
   {
      if( !m_metadata ) {
         m_metadata = new RelationalCollectionMetadata();
         m_metadata->initialize( m_description.name(), *m_session, m_mode );
      }
      return *m_metadata;
   }   
}


void
pool::RelationalCollection::RelationalCollection::
initialize()
{
  // debug stuff to clean up later
   std::string     OpenModeNames[10];
   OpenModeNames[pool::ICollection::READ] = "read";
   OpenModeNames[pool::ICollection::UPDATE] = "update";
   OpenModeNames[pool::ICollection::CREATE] = "create";
   OpenModeNames[pool::ICollection::CREATE_AND_OVERWRITE] = "create-and-overwrite";
   OpenModeNames[pool::ICollection::CREATE_MISSING_FRAGMENTS] = "create-missing-fragments";

   coral::MessageStream log( "pool::RelationalCollection::init" );
   log << coral::Info << "Opening collection `" << m_name << "' in " << OpenModeNames[m_mode] << " mode " << coral::MessageStream::endmsg;
   bool        inCreateMode =
      m_mode == pool::ICollection::CREATE  ||
      m_mode == pool::ICollection::CREATE_MISSING_FRAGMENTS ||
      m_mode == pool::ICollection::CREATE_AND_OVERWRITE;

   DBLock       dbLock( *m_session, m_description.name() );
   if( m_mode !=  pool::ICollection::READ ) {
      bool      XTransaction(true);   //doing PERM lock, TEMP did not work... MN
      dbLock.lock(XTransaction);
   }

   coral::ISchema& nominalSchema = m_session->nominalSchema();
   
   bool        headerTableExists =
      nominalSchema.existsTable( RelationalCollectionNames::nameOfCollectionHeadersTable() )
      || nominalSchema.existsView( RelationalCollectionNames::nameOfCollectionHeadersTable() );
   bool        descriptionTableExists =
      nominalSchema.existsTable( RelationalCollectionNames::nameOfCollectionDescriptionsTable() )
      || nominalSchema.existsView( RelationalCollectionNames::nameOfCollectionDescriptionsTable() );
   bool        indexDescTableExists =
      nominalSchema.existsTable( RelationalCollectionNames::nameOfCollectionIndexDescriptionsTable() )
      || nominalSchema.existsView( RelationalCollectionNames::nameOfCollectionIndexDescriptionsTable() );

   string        missingTables;
   if( !headerTableExists )
      missingTables += RelationalCollectionNames::nameOfCollectionHeadersTable() + " ";
   if( !descriptionTableExists )
      missingTables += RelationalCollectionNames::nameOfCollectionDescriptionsTable() + " ";
   if( !indexDescTableExists )
      missingTables += RelationalCollectionNames::nameOfCollectionIndexDescriptionsTable() + " ";

   if( !inCreateMode
       && (!headerTableExists || !descriptionTableExists || !indexDescTableExists) ) {
      std::string errorMsg = "Table(s): " + missingTables + " not existing. Cannot open collection `" + m_name + "' in UPDATE or READ mode.";
      throw pool::Exception( errorMsg,
                             "RelationalCollection::initialize",
                             "RelationalCollection" );
   }

   if( !(headerTableExists && descriptionTableExists && indexDescTableExists) ) {
      if( headerTableExists || descriptionTableExists || indexDescTableExists ) {
         // some talbes are missing - do not try to create them so to not mess up some older schema
         std::string errorMsg = "Inconsistent database schema: tables: " + missingTables + " are missing. Cannot open collection `" + m_name + "'";
         throw pool::Exception( errorMsg,
                                "RelationalCollection::initialize",
                                "RelationalCollection" );
      }

      log << coral::Info << "Creating new database schema for collection `"
          << m_name << "'" << coral::MessageStream::endmsg;
      createHeadersTable();
      createDescriptionsTable();
      createIndexDescriptionsTable();
   }         

   
  // delete old collection fragments if requested
  if( m_mode == pool::ICollection::CREATE_AND_OVERWRITE ) {
     for( int i = 0; i < m_description.numberOfCollectionFragments(); i++ )  {
        std::string fragmentName = m_description.collectionFragment( i ).name();
        if( exists( fragmentName, false ) ) {
           retrieveFragmentTableNames( fragmentName );
           bool  dont_commit(false);
           drop_impl( fragmentName, false, true, dont_commit ); 
        }
     }
  }

  // For READ or UPDATE open mode retrieve descriptions of collection fragments.
  if( m_mode == pool::ICollection::UPDATE || m_mode == pool::ICollection::READ )  {
     retrieveCollectionFragmentDescriptions();

     for( int i = 0; i < m_description.numberOfCollectionFragments(); i++ )  {
        std::string fragmentName = m_description.collectionFragment( i ).name();
        if( exists( fragmentName, false ) ) {
           // something is wrong with the data model if I need to dyn_cast  //MN.
           retrieveColumnDescriptions( fragmentName,
                                       dynamic_cast<CollectionDescription*>(&m_description) );
           retrieveFragmentTableNames( fragmentName );
        } else {
           std::string errorMsg = "Collection fragment `" + fragmentName + 
              "' does not exist. Cannot open collection `" + m_name + "' in UPDATE or READ mode.";
           throw pool::Exception( errorMsg,
                                  "RelationalCollection::initialize",
                                  "RelationalCollection" );
        }
     }
  }

  // Create new schema editor object for collection.
  m_schemaEditor = new pool::RelationalCollection::RelationalCollectionSchemaEditor( 
                             *m_session,
                             m_description,
                             m_dataTableNameForCollectionFragmentName,
                             m_linksTableNameForCollectionFragmentName,
                             m_tableTokenColumnPrefixForCollectionTokenColumnName,
                             m_tableAttributeColumnNameForCollectionAttributeColumnName,
                             m_tableColumnNameForCollectionColumnName,
                             m_collectionColumnNameForTableColumnPosition,
                             *m_tableAttributeList,
                             m_collectionRowBuffer,
                             m_dataTableRowBufferForCollectionFragmentName,
                             m_mapOfWhereClausesForOID1InDataTable,
                             m_mapOfWhereDataForOID1InDataTable );

  // Initialize private maps and data row buffers for all collection fragments.
  for( int i = 0; i < m_description.numberOfCollectionFragments(); i++ )  {
     initializeMapsAndDataRowBuffers( m_description.collectionFragment( i ).name() ); 
  }

  // Create tables
  if( inCreateMode ) {
     // Loop over all collection fragments and create new tables 
     for( int i = 0; i < m_description.numberOfCollectionFragments(); i++ )  {
        std::string fragmentName = m_description.collectionFragment( i ).name();

        // If the fragment does not exist create it
        if( !exists( fragmentName, true ) ) {
           createDataAndLinksTables( fragmentName );
        } else {
           if( m_mode == pool::ICollection::CREATE_MISSING_FRAGMENTS )  {
              if( this->newSchemaEqualsExistingSchema( fragmentName ) ) {
                 retrieveFragmentTableNames( fragmentName );
                 // Update row for parent collection fragment in collection headers table.
                 if( fragmentName != m_name ) {
                    m_whereDataForCollectionNameInHeadersTable->begin()->data<std::string>()
                       = m_description.collectionFragment( fragmentName ).parentCollectionFragmentName();
                    std::string setClause = RelationalCollectionNames::childCollectionNameVariableInCollectionHeadersTable() + " = " + fragmentName;
                    nominalSchema.tableHandle( RelationalCollectionNames::nameOfCollectionHeadersTable() ).dataEditor().updateRows(
                       setClause,
                       m_whereClauseForCollectionNameInHeadersTable, 
                       *m_whereDataForCollectionNameInHeadersTable );
                 }

                 // Drop all foreign keys applied by collection fragment except that defined in collection description.
                 this->dropParentForeignKeyConstraints( fragmentName, true );
              } else {
                 std::string errorMsg = "Schema of existing collection fragment `" + fragmentName + "' differs from new one. " + "Cannot open collection `" + m_name + "' in open mode CREATE_MISSING_FRAGMENTS.";
                 throw pool::Exception( errorMsg,
                                        "RelationalCollection::initialize",
                                        "RelationalCollection" );
              }
           } else {
              std::string errorMsg = "Cannot overwrite existing collection fragment `" + fragmentName + "' of collection `" + m_name + "' in CREATE open mode.";
              throw pool::Exception( errorMsg,
                                     "RelationalCollection::initialize",
                                     "RelationalCollection" );
           }
        }
     }
  }
 
  for( int i = 0; i < m_description.numberOfCollectionFragments(); i++ )  {
     createLinkIdToTokenKeyMaps( m_description.collectionFragment( i ).name() );
  }

  // Create row buffer for links table schema.
  createLinksTableRowBuffer();

  // Create any indices, unique constraints or foreign key constraints defined by collection description.
  if( inCreateMode ) {
     for( int i = 0; i < m_description.numberOfIndices(); i++ )  {
        this->createIndex( m_description.index( i ).name(),
                           m_description.index( i ).columnNames(),
                           m_description.index( i ).isUnique() );
     }
     for( int i = 0; i < m_description.numberOfUniqueConstraints(); i++ )  {
        this->setUniqueConstraint( m_description.uniqueConstraint( i ).name(),
                                   m_description.uniqueConstraint( i ).columnNames() );
     }
     for( int i = 0; i < m_description.numberOfCollectionFragments(); i++ ) {
        this->setForeignKeyConstraint( m_description.collectionFragment( i ).name() );
     }
  }
  else  {
    for( int i = 0; i < m_description.numberOfCollectionFragments(); i++ )    {
      std::string fragmentName = m_description.collectionFragment( i ).name();
      this->retrieveIndexDescriptions( fragmentName );
      this->retrieveUniqueConstraintDescriptions( fragmentName );
    }
  }

  // For UPDATE mode, drop all foreign key constraints on parent collection fragments not defined in collection description.
  if( m_mode == pool::ICollection::UPDATE )  {
    for( int i = 0; i < m_description.numberOfCollectionFragments(); i++ ) {
       dropParentForeignKeyConstraints( m_description.collectionFragment( i ).name(), true );
    }
  }

  if( inCreateMode ) {
     if( m_session->properties().flavorName() == "Oracle" ) {
        const string init_procedure = "POOL_COLLECTION_INIT";
        bool procedureExists( false );
        try {
           std::unique_ptr<coral::IQuery> query( m_session->schema("SYS").newQuery() );
           coral::AttributeList        output;
           output.extend( "count(*)", "int");
           query->addToOutputList("count(*)");
           query->defineOutput( output );
           query->addToTableList("USER_OBJECTS");
           coral::AttributeList        variables;
           variables.extend( "OBJECT_NAME", "string" );
           variables[0].data<string>() = init_procedure;
           variables.extend( "OBJECT_TYPE", "string" );
           variables[1].data<string>() = "PROCEDURE";
           string query_conditions = "OBJECT_NAME = :" + variables[0].specification().name()
              + " AND OBJECT_TYPE = :" + variables[1].specification().name();
           query->setCondition( query_conditions, variables );

           coral::ICursor& result = query->execute();
           if( result.next() ) {
              if( result.currentRow()[0].data<int>() > 0 ) {
                 procedureExists = true;
              }
           }
           if( !procedureExists) {
              log << coral::Debug
                  << "Database procedure " << init_procedure << " not found." << coral::MessageStream::endmsg;
           } 
        } catch( std::exception &e ) {
           log << coral::Warning
               << "Lookup for database procedure " << init_procedure
               << " returned error: " << e.what() <<  coral::MessageStream::endmsg;
        }
        if( procedureExists ) {
           // commit all schema tables for a new collection before insertion
           bool        restartTrans( true );
           commit( restartTrans );

           log << coral::Info
               << "Executing database procedure " << init_procedure  << "(" << m_name << ") "
               << coral::MessageStream::endmsg;
           try {
              coral::AttributeList        params;
              params.extend( "CollName", "string" );
              params.begin()->data<string>() = m_name;
              nominalSchema.callProcedure( init_procedure, params ); 
              log << coral::Debug
                  << "Database procedure " << init_procedure
                  << "(" << m_name << ") executed." << coral::MessageStream::endmsg;
           }
           catch( std::exception &e ) {
              std::string errorMsg = "Error executing database procedure " + init_procedure + "(" + m_name + ") : " + e.what();
              log << coral::Error << errorMsg << coral::MessageStream::endmsg;
              if( getenv("POOL_RCOLL_LOCKONERROR") ) {
                 dbLock.setAutorelease( false );   // leave the database locked
                 log << coral::Warning << "LOCKING database - all collection creation tasks will wait!"  << coral::MessageStream::endmsg;
              }
              log << coral::Info << "Removing collection " << m_name << coral::MessageStream::endmsg;
              drop( m_name, true, true );
              throw pool::Exception( errorMsg,
                                     "RelationalCollection::initialize",
                                     "RelationalCollection" );
           }
        }
     }
  }

  if( m_mode !=  pool::ICollection::READ ) {
     dbLock.unlock(); 
     // commit away central locks before insertion
     commit(); 
  }
 
  // Create new data editor object for collection.
  m_dataEditor = new RelationalCollectionDataEditor(
                           *m_session,
                           m_description,
                           m_dataTableNameForCollectionFragmentName,
                           m_linksTableNameForCollectionFragmentName,
                           m_fragmentDataMap,
                           m_tableTokenColumnPrefixForCollectionTokenColumnName,
                           m_tableAttributeColumnNameForCollectionAttributeColumnName,
                           *m_tableAttributeList,
                           m_collectionRowBuffer,
                           m_dataTableRowBufferForCollectionFragmentName,
                           *m_linksTableRowBuffer,
                           m_mapOfLinkIdForTokenKeyMaps,
                           m_mapOfTokenKeyForLinkIdMaps,
                           m_mapOfWhereClausesForOID1InDataTable,
                           m_mapOfWhereDataForOID1InDataTable );

  m_initialized = true;
}





void
pool::RelationalCollection::RelationalCollection::createHeadersTable()
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  coral::TableDescription description( RelationalCollectionNames::collectionTypeName() );
  description.setName( RelationalCollectionNames::nameOfCollectionHeadersTable() );

  bool  fixedSize = true;
  int   columnSize = 400;
  description.insertColumn( 
     RelationalCollectionNames::collectionNameVariableInCollectionHeadersTable(),
     coral::AttributeSpecification::typeNameForType<std::string>(), columnSize, !fixedSize );
  description.insertColumn( 
     RelationalCollectionNames::dataTableNameVariableInCollectionHeadersTable(),
     coral::AttributeSpecification::typeNameForType<std::string>(), columnSize, !fixedSize );
  description.insertColumn( 
     RelationalCollectionNames::linksTableNameVariableInCollectionHeadersTable(),
     coral::AttributeSpecification::typeNameForType<std::string>(), columnSize, !fixedSize );
  description.insertColumn( 
     RelationalCollectionNames::writtenRecordsVariableInCollectionHeadersTable(),
     coral::AttributeSpecification::typeNameForType<int>() );
  description.insertColumn( 
     RelationalCollectionNames::deletedRecordsVariableInCollectionHeadersTable(),
     coral::AttributeSpecification::typeNameForType<int>() );
  description.insertColumn( 
     RelationalCollectionNames::childCollectionNameVariableInCollectionHeadersTable(),
     coral::AttributeSpecification::typeNameForType<std::string>(), columnSize, !fixedSize );
  description.insertColumn( 
     RelationalCollectionNames::foreignKeyNameVariableInCollectionHeadersTable(),
     coral::AttributeSpecification::typeNameForType<std::string>(), columnSize, !fixedSize );

  description.setPrimaryKey(  
     RelationalCollectionNames::collectionNameVariableInCollectionHeadersTable() );

  nominalSchema.createTable( description ); //.privilegeManager().grantToPublic( coral::ITablePrivilegeManager::Select );
}


void
pool::RelationalCollection::RelationalCollection::createDescriptionsTable()
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  coral::TableDescription description( RelationalCollectionNames::collectionTypeName() );
  description.setName( RelationalCollectionNames::nameOfCollectionDescriptionsTable() );

  bool  fixedSize = true;
  int   columnSize = 400;
  description.insertColumn( 
     RelationalCollectionNames::collectionNameVariableInCollectionDescriptionsTable(),
     coral::AttributeSpecification::typeNameForType<std::string>(), columnSize, !fixedSize );
  description.insertColumn( 
     RelationalCollectionNames::collectionVariableNameVariableInCollectionDescriptionsTable(),
     coral::AttributeSpecification::typeNameForType<std::string>(), columnSize, !fixedSize );
  description.insertColumn( 
     RelationalCollectionNames::collectionVariableTypeVariableInCollectionDescriptionsTable(),
     coral::AttributeSpecification::typeNameForType<std::string>(), columnSize, !fixedSize );
  description.insertColumn( 
     RelationalCollectionNames::collectionVariableMaxSizeVariableInCollectionDescriptionsTable(),
     coral::AttributeSpecification::typeNameForType<int>() );
  description.insertColumn( 
     RelationalCollectionNames::collectionVariableSizeIsFixedVariableInCollectionDescriptionsTable(),
     coral::AttributeSpecification::typeNameForType<std::string>(), 5, !fixedSize );
  description.insertColumn( 
     RelationalCollectionNames::collectionVariablePositionVariableInCollectionDescriptionsTable(),
     coral::AttributeSpecification::typeNameForType<int>() );
  description.insertColumn(
     RelationalCollectionNames::collectionVariableAnnotationVariableInCollectionDescriptionsTable(),
     coral::AttributeSpecification::typeNameForType<std::string>() );
//  description.insertColumn( 
//     pool::RelationalCollection::RelationalCollectionNames::collectionVariableUniqueConstraintNameVariableInCollectionDescriptionsTable(),
//     coral::AttributeSpecification::typeNameForType<std::string>() );

  nominalSchema.createTable( description ); //.privilegeManager().grantToPublic( coral::ITablePrivilegeManager::Select );
}


void
pool::RelationalCollection::RelationalCollection::createIndexDescriptionsTable()
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  coral::TableDescription description( pool::RelationalCollection::RelationalCollectionNames::collectionTypeName() );
  description.setName( pool::RelationalCollection::RelationalCollectionNames::nameOfCollectionIndexDescriptionsTable() );

  description.insertColumn( RelationalCollectionNames::indexNameVariableInCollectionIndexDescriptionsTable(),
     coral::AttributeSpecification::typeNameForType<std::string>() );
  description.insertColumn( RelationalCollectionNames::uniqueConstraintNameVariableInCollectionIndexDescriptionsTable(),
     coral::AttributeSpecification::typeNameForType<std::string>() );
  description.insertColumn( RelationalCollectionNames::collectionVariableNameVariableInCollectionIndexDescriptionsTable(),
     coral::AttributeSpecification::typeNameForType<std::string>() );
  description.insertColumn( RelationalCollectionNames::collectionNameVariableInCollectionIndexDescriptionsTable(),
     coral::AttributeSpecification::typeNameForType<std::string>() );

  nominalSchema.createTable( description ); //.privilegeManager().grantToPublic( coral::ITablePrivilegeManager::Select );
}


bool
pool::RelationalCollection::RelationalCollection::newSchemaEqualsExistingSchema( const std::string& fragmentName )
{
   CollectionDescription        existingDesc( fragmentName, m_description.type() );
   retrieveColumnDescriptions( fragmentName, &existingDesc );

   return existingDesc.equals( m_description );
}


void
pool::RelationalCollection::RelationalCollection::
createDataAndLinksTables( const std::string& fragmentName )
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  // Find next available names for data and links tables of new collection fragment and update private maps.
  std::string dataTableName, linksTableName;
  for( int i = 0; ; ++i )   {
     dataTableName = RelationalCollectionNames::nameOfCollectionDataTable( i );
     linksTableName = RelationalCollectionNames::nameOfCollectionLinksTable( i );
     if( ! ( nominalSchema.existsTable( dataTableName ) ||
             nominalSchema.existsView( dataTableName )  ||
             nominalSchema.existsTable( linksTableName ) ||
             nominalSchema.existsView( linksTableName ) ) )
        break;
  }
  m_dataTableNameForCollectionFragmentName[fragmentName] = dataTableName;
  m_linksTableNameForCollectionFragmentName[fragmentName] = linksTableName;
  m_fragmentDataMap[fragmentName].setCurrentRowId( 0 );

   // Create table description for data table of new collection fragment.
  coral::TableDescription dataTableDescription( RelationalCollectionNames::collectionTypeName() );
  dataTableDescription.setName( dataTableName );

  // Add primary key column.
  dataTableDescription.insertColumn( 
     RelationalCollectionNames::recordIdVariableInCollectionDataTable(),
     coral::AttributeSpecification::typeNameForType<int>() );

  // Add columns for event reference if this is the top level collection fragment and an event reference was specified.
  int        addedEventRefColumn = 0;
  if( fragmentName == m_name && m_description.hasEventReferenceColumn() )  {
     addedEventRefColumn = 1;
     dataTableDescription.insertColumn( 
        RelationalCollectionNames::oid_1_variableInCollectionDataTable(),
        coral::AttributeSpecification::typeNameForType<int>() );
     dataTableDescription.insertColumn( 
        RelationalCollectionNames::oid_2_variableInCollectionDataTable(),
        coral::AttributeSpecification::typeNameForType<int>() );
  }

  // Add Token columns to data table schema.
  for( int i = addedEventRefColumn; i < m_description.numberOfTokenColumns( fragmentName ); i++ )  {
     std::string tableColumnPrefix = m_tableColumnNameForCollectionColumnName[ m_description.tokenColumn( i, fragmentName ).name() ]; 
     std::string oid1ColumnName = tableColumnPrefix + RelationalCollectionNames::oid_1_variableInCollectionDataTable();
     std::string oid2ColumnName = tableColumnPrefix + RelationalCollectionNames::oid_2_variableInCollectionDataTable();
     dataTableDescription.insertColumn( oid1ColumnName, coral::AttributeSpecification::typeNameForType<int>() );
     dataTableDescription.insertColumn( oid2ColumnName, coral::AttributeSpecification::typeNameForType<int>() );
  }

  // Add Attribute columns to data table schema.
  for( int i = 0; i < m_description.numberOfAttributeColumns( fragmentName ); i++ )  {
     const ICollectionColumn &column = m_description.attributeColumn( i, fragmentName );
     std::string tableColumnName = m_tableColumnNameForCollectionColumnName[ column.name() ];
     dataTableDescription.insertColumn( tableColumnName, column.type() );
  }

  // Set primary key for data table.
  dataTableDescription.setPrimaryKey( RelationalCollectionNames::recordIdVariableInCollectionDataTable() );

  // Create data table and define user privileges.
  nominalSchema.createTable( dataTableDescription ); //.privilegeManager().grantToPublic( coral::ITablePrivilegeManager::Select );

  // Create description object for links table of new collection fragment.
  coral::TableDescription linksTableDescription( RelationalCollectionNames::collectionTypeName() );
  linksTableDescription.setName( linksTableName );

  // Add link table ID and Token key information to links table schema:
  linksTableDescription.insertColumn( 
    RelationalCollectionNames::linkIdVariableInCollectionLinksTable(),
    coral::AttributeSpecification::typeNameForType<int>() );
  linksTableDescription.insertColumn( 
    RelationalCollectionNames::databaseIdVariableInCollectionLinksTable(),
    coral::AttributeSpecification::typeNameForType<std::string>() );
  linksTableDescription.insertColumn( 
    RelationalCollectionNames::containerIdVariableInCollectionLinksTable(),
    coral::AttributeSpecification::typeNameForType<std::string>() );
  linksTableDescription.insertColumn( 
    RelationalCollectionNames::classIdVariableInCollectionLinksTable(),
    coral::AttributeSpecification::typeNameForType<std::string>() );
  linksTableDescription.insertColumn( 
    RelationalCollectionNames::technologyIdVariableInCollectionLinksTable(),
    coral::AttributeSpecification::typeNameForType<int>() );

  // Set primary key for links table of new collection fragment.
  linksTableDescription.setPrimaryKey( RelationalCollectionNames::linkIdVariableInCollectionLinksTable() );

  // Create links table for new collection fragment and define user privileges.
  nominalSchema.createTable( linksTableDescription ); //.privilegeManager().grantToPublic( coral::ITablePrivilegeManager::Select );

  // Add rows for new collection fragment to headers table.
  initializeHeadersTable( fragmentName, dataTableName, linksTableName );

  // Add rows for new collection fragment to descriptions table.
  initializeDescriptionsTable( fragmentName );
}



void
pool::RelationalCollection::RelationalCollection::
initializeHeadersTable( const std::string& fragmentName,
                        const std::string& dataTableName,
                        const std::string& linksTableName )
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  // Add row for new collection fragment into collection headers table.
  coral::AttributeList rowBuffer;
  rowBuffer.extend<std::string>( 
     RelationalCollectionNames::collectionNameVariableInCollectionHeadersTable() );
  rowBuffer.extend<std::string>( 
     RelationalCollectionNames::dataTableNameVariableInCollectionHeadersTable() );
  rowBuffer.extend<std::string>( 
     RelationalCollectionNames::linksTableNameVariableInCollectionHeadersTable() );
  rowBuffer.extend<unsigned>( 
     RelationalCollectionNames::writtenRecordsVariableInCollectionHeadersTable() );
  rowBuffer.extend<unsigned>( 
     RelationalCollectionNames::deletedRecordsVariableInCollectionHeadersTable() );
  rowBuffer.extend<std::string>( 
     RelationalCollectionNames::childCollectionNameVariableInCollectionHeadersTable() );
  rowBuffer.extend<std::string>( 
     RelationalCollectionNames::foreignKeyNameVariableInCollectionHeadersTable() );
  const ICollectionFragment& fragment = m_description.collectionFragment( fragmentName );
  coral::AttributeList::iterator iAttribute = rowBuffer.begin();
  iAttribute->data< std::string >() = fragmentName;
  ++iAttribute;
  iAttribute->data< std::string >() = dataTableName;
  ++iAttribute;
  iAttribute->data< std::string >() = linksTableName;
  ++iAttribute;
  iAttribute->data< unsigned >() = 0;
  ++iAttribute;
  iAttribute->data< unsigned >() = 0;
  ++iAttribute;
  iAttribute->data< std::string >() = fragment.childCollectionFragmentName();
  ++iAttribute;
  iAttribute->data< std::string >() = fragment.foreignKeyName();
  nominalSchema.tableHandle( RelationalCollectionNames::nameOfCollectionHeadersTable() ).dataEditor().insertRow( rowBuffer );

  // Lock row in collection headers table for this collection fragment for duration of transaction.
  exists( fragmentName, true );
}


void
pool::RelationalCollection::RelationalCollection::
initializeDescriptionsTable( const std::string& fragmentName ) 
{
   // Add rows for new collection fragment into collection descriptions table.
   coral::AttributeList rowBuffer;

   rowBuffer.extend< std::string >( 
      RelationalCollectionNames::collectionNameVariableInCollectionDescriptionsTable() );
   rowBuffer.extend< std::string >( 
      RelationalCollectionNames::collectionVariableNameVariableInCollectionDescriptionsTable() );
   rowBuffer.extend< std::string >( 
      RelationalCollectionNames::collectionVariableTypeVariableInCollectionDescriptionsTable() );
   rowBuffer.extend< int >( 
      RelationalCollectionNames::collectionVariableMaxSizeVariableInCollectionDescriptionsTable() );
   rowBuffer.extend< std::string >( 
      RelationalCollectionNames::collectionVariableSizeIsFixedVariableInCollectionDescriptionsTable() );
   rowBuffer.extend< int >( 
      RelationalCollectionNames::collectionVariablePositionVariableInCollectionDescriptionsTable() );
   rowBuffer.extend< std::string >( 
      RelationalCollectionNames::collectionVariableAnnotationVariableInCollectionDescriptionsTable() );

  // Get handle to collection descriptions table.
  coral::ITable& descriptionsTable = 
    m_session->nominalSchema().tableHandle( RelationalCollectionNames::nameOfCollectionDescriptionsTable() );

  // Set collection fragment name to be the same for all rows to be added here.
  rowBuffer[0].data< std::string >() = fragmentName;

  // Add rows for Token column descriptions.
  for( int i = 0; i < m_description.numberOfTokenColumns( fragmentName ); i++ )  {
     const pool::ICollectionColumn& column = m_description.tokenColumn( i , fragmentName );
     if( column.name() == CollectionBaseNames::defaultEventReferenceColumnName()
         && column.name() == m_description.eventReferenceColumnName()
         && column.annotation().empty() ) {
        // don't store event reference column if it had default name and no annotation
        continue;
     }
     rowBuffer[1].data< std::string >() = column.name();
     rowBuffer[2].data< std::string >() = CollectionBaseNames::tokenTypeName();
     rowBuffer[3].data< int >()         = 0;
     rowBuffer[4].data< std::string >() = "true";
     rowBuffer[5].data< int >()         = column.id();
     rowBuffer[6].data< std::string >() = column.annotation();
     //cout << " -->>>  Adding Token column " << column.name() << ", ID=" << column.id() <<", annotated:" << column.annotation() << endl;
     // Insert new row:
     descriptionsTable.dataEditor().insertRow( rowBuffer );
  }

  // Add rows for Attribute descriptions.
  for( int i = 0; i < m_description.numberOfAttributeColumns( fragmentName ); i++ )  {
     const pool::ICollectionColumn& column = m_description.attributeColumn( i , fragmentName );

     rowBuffer[1].data< std::string >() = column.name();
     rowBuffer[2].data< std::string >() = column.type();
     rowBuffer[3].data< int >()         = column.maxSize();
     rowBuffer[4].data< std::string >() = column.sizeIsFixed()? "true" : "false";
     rowBuffer[5].data< int >()         = column.id(); 
     rowBuffer[6].data< std::string >() = column.annotation();
     // Insert new row.
     descriptionsTable.dataEditor().insertRow( rowBuffer );
  }
}


void
pool::RelationalCollection::RelationalCollection::
retrieveCollectionFragmentDescriptions()
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  coral::ITable& headersTable = nominalSchema.tableHandle(
    RelationalCollectionNames::nameOfCollectionHeadersTable() );

  // Get all collection fragments and add to fragment list in correct order. (Note: Top level collection fragment already
  // added in collection description constructor).
  std::string fragmentName = m_name;
  while ( fragmentName.size() > 0 )  {
    coral::IQuery* query = headersTable.newQuery();

    query->addToOutputList(
       RelationalCollectionNames::childCollectionNameVariableInCollectionHeadersTable() );
    query->addToOutputList(
       RelationalCollectionNames::foreignKeyNameVariableInCollectionHeadersTable() );
    m_whereDataForCollectionNameInHeadersTable->begin()->data<std::string>() = fragmentName;
    query->setCondition( m_whereClauseForCollectionNameInHeadersTable, *m_whereDataForCollectionNameInHeadersTable );
    query->limitReturnedRows( 1, 0 );

    coral::ICursor& cursor = query->execute();

    if( cursor.next() )    {
      const coral::AttributeList& row = cursor.currentRow();
      std::string childFragmentName;
      if( !row[0].isNull() )
         childFragmentName = row[0].data<std::string>();
      std::string foreignKeyName;
      if( !row[1].isNull() )
         foreignKeyName = row[1].data<std::string>();

      if( childFragmentName.size() > 0 ) {
         if (!m_schemaEditor) {
           throw pool::Exception( "No schema editor",
                                  "RelationalCollection::retrieveCollectionFragmentDescriptions",
                                  "RelationalCollection" );
         }
         if( foreignKeyName.size() > 0 ) {
           //m_description.addCollectionFragment( childFragmentName, fragmentName, true );
           // MN: FIX guess - check functionality!
           m_schemaEditor->addCollectionFragment( childFragmentName, fragmentName, true );
        }
        else {
           // m_description.addCollectionFragment( childFragmentName, fragmentName, false );
           // MN: FIX guess - check functionality!           
           m_schemaEditor->addCollectionFragment( childFragmentName, fragmentName, false );
        }
      }
      fragmentName = childFragmentName;
    }
    else {
      delete query;
      std::string errorMsg = "Collection fragment `" + fragmentName + "' does not exist.";
      throw pool::Exception( errorMsg,
                             "RelationalCollection::retrieveCollectionFragmentDescriptions",
                             "RelationalCollection" );
    }
    delete query;
  }
}


void
pool::RelationalCollection::RelationalCollection::
retrieveColumnDescriptions( const std::string& fragmentName,
                            CollectionDescription *collDesc ) const
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  // Retrieve column names and types for this fragment and insert into collection description object.
  coral::ITable& descriptionsTable = nominalSchema.tableHandle( RelationalCollectionNames::nameOfCollectionDescriptionsTable() );
  std::unique_ptr<coral::IQuery>         descQuery( descriptionsTable.newQuery() );
  descQuery->addToOutputList( RelationalCollectionNames::collectionVariableNameVariableInCollectionDescriptionsTable() );
  descQuery->addToOutputList( RelationalCollectionNames::collectionVariableTypeVariableInCollectionDescriptionsTable() );
  descQuery->addToOutputList( RelationalCollectionNames::collectionVariableMaxSizeVariableInCollectionDescriptionsTable() );
  descQuery->defineOutputType(RelationalCollectionNames::collectionVariableMaxSizeVariableInCollectionDescriptionsTable(), "int" );
  descQuery->addToOutputList( RelationalCollectionNames::collectionVariableSizeIsFixedVariableInCollectionDescriptionsTable() );
  descQuery->addToOutputList( RelationalCollectionNames::collectionVariablePositionVariableInCollectionDescriptionsTable() );
  descQuery->defineOutputType(RelationalCollectionNames::collectionVariablePositionVariableInCollectionDescriptionsTable(), "int" );
  descQuery->addToOutputList( RelationalCollectionNames::collectionVariableAnnotationVariableInCollectionDescriptionsTable() );
  m_whereDataForCollectionNameInDescriptionsTable->begin()->data<std::string>() = fragmentName;
  descQuery->setCondition( m_whereClauseForCollectionNameInDescriptionsTable, 
                           *m_whereDataForCollectionNameInDescriptionsTable );
  descQuery->addToOrderList( RelationalCollectionNames::collectionVariablePositionVariableInCollectionDescriptionsTable() );
  descQuery->setRowCacheSize( 100 );
  coral::ICursor& descCursor = descQuery->execute();

  while( descCursor.next() )   {
     const coral::AttributeList& row = descCursor.currentRow();
     
     std::string collectionColumnName   = row[0].data<std::string>();
     std::string columnType             = row[1].data<std::string>();
     int         maxSize                = row[2].data<int>();
     bool        sizeIsFixed            = ( row[3].data<std::string>() == "true" );
     int         variablePosition       = row[4].data<int>();
     std::string annotation;
     if( !row[5].isNull() )  annotation = row[5].data<std::string>();
     //cout << " -->>> Got Description for column " << collectionColumnName << " annotated:" << annotation << ", isFixed='" << row[3].data<std::string>() << "' (" << sizeIsFixed << ")" <<endl;

     if( variablePosition == 0 ) {
        // this is the EvRef token column
        collDesc->setEventReferenceColumnName( collectionColumnName );
     }
     collDesc->insertColumn( collectionColumnName, columnType, annotation, fragmentName, maxSize, sizeIsFixed );
     collDesc->setColumnId( collectionColumnName, variablePosition, "retrieveColumnDescriptions" );
  }
}


// Initialize all maps for retrieving table column names
// and all internal row buffers for this collection fragment.
void
pool::RelationalCollection::RelationalCollection::
initializeMapsAndDataRowBuffers( const std::string& fragmentName )
{
   coral::AttributeList* dataTableRowBuffer = new coral::AttributeList;

   // Add primary key column - only temporary fix?  MN
   dataTableRowBuffer->extend( RelationalCollectionNames::recordIdVariableInCollectionDataTable(), typeid( unsigned ) );

   for( int i = 0; i < m_description.numberOfTokenColumns( fragmentName ); i++ )  {
      const pool::ICollectionColumn& column = m_description.tokenColumn( i , fragmentName );
      const std::string&        columnName = column.name();
      int                       variablePosition = column.id();
      std::string               variableName;
      if( columnName != m_description.eventReferenceColumnName() ) {
         variableName = RelationalCollectionNames::variableDataVariableInCollectionDataTable( variablePosition ) + "_";
      }
//      m_poolOut << "RelColl - adding token column " << columnName << ", ID=" << variablePosition << ", variable name=" <<  variableName << coral::MessageStream::endmsg;
      m_schemaEditor->addTokenColumnToMaps( variableName, columnName, variablePosition );
      m_schemaEditor->addTokenColumnToRowBuffer( dataTableRowBuffer, variableName );
   }

   for( int i = 0; i < m_description.numberOfAttributeColumns( fragmentName ); i++ )  {
      const pool::ICollectionColumn& column = m_description.attributeColumn( i , fragmentName );
      const std::string&        columnName = column.name();
      const std::string&        columnType = column.type();
      int                       variablePosition = column.id();
      std::string               tableColumnName = RelationalCollectionNames::variableDataVariableInCollectionDataTable( variablePosition );

//      m_poolOut << "RelColl - adding attrib column " << columnName << ", ID=" << variablePosition << ", table column name=" <<  tableColumnName << coral::MessageStream::endmsg;
     m_schemaEditor->addColumnToMaps( columnName, tableColumnName, columnType, variablePosition );
      dataTableRowBuffer->extend( tableColumnName, columnType );
   }

   // Add data table row buffer object for this collection fragment to private map.
   m_dataTableRowBufferForCollectionFragmentName[fragmentName] = dataTableRowBuffer;
}


void
pool::RelationalCollection::RelationalCollection::createLinksTableRowBuffer()
{
  m_linksTableRowBuffer->extend( 
    RelationalCollectionNames::linkIdVariableInCollectionLinksTable(), typeid(unsigned) );

  m_linksTableRowBuffer->extend( 
     RelationalCollectionNames::databaseIdVariableInCollectionLinksTable(), typeid(std::string) );

  m_linksTableRowBuffer->extend( 
     RelationalCollectionNames::containerIdVariableInCollectionLinksTable(), typeid(std::string) );

  m_linksTableRowBuffer->extend( 
     RelationalCollectionNames::classIdVariableInCollectionLinksTable(), typeid(std::string) );

  m_linksTableRowBuffer->extend( 
     RelationalCollectionNames::technologyIdVariableInCollectionLinksTable(), typeid(unsigned) );
}


void
pool::RelationalCollection::RelationalCollection::
createLinkIdToTokenKeyMaps( const std::string& fragmentName )
{
  // Query the links table.
  unique_ptr<coral::IQuery> query( m_session->nominalSchema().newQuery() );
  query->addToTableList( m_linksTableNameForCollectionFragmentName[fragmentName] );

  // Select all columns from links table and order by link ID.
  coral::AttributeList outputDataBuffer;
  std::string linkIdColumnName = RelationalCollectionNames::linkIdVariableInCollectionLinksTable();
  query->addToOutputList( linkIdColumnName );
  query->addToOrderList( linkIdColumnName );
  outputDataBuffer.extend< unsigned >( linkIdColumnName );
  std::string databaseIdColumnName = RelationalCollectionNames::databaseIdVariableInCollectionLinksTable();
  query->addToOutputList( databaseIdColumnName );
  outputDataBuffer.extend< std::string >( databaseIdColumnName );
  std::string containerIdColumnName = RelationalCollectionNames::containerIdVariableInCollectionLinksTable();
  query->addToOutputList( containerIdColumnName );
  outputDataBuffer.extend< std::string >( containerIdColumnName );
  std::string classIdColumnName = RelationalCollectionNames::classIdVariableInCollectionLinksTable();
  query->addToOutputList( classIdColumnName );
  outputDataBuffer.extend< std::string >( classIdColumnName );
  std::string technologyIdColumnName = RelationalCollectionNames::technologyIdVariableInCollectionLinksTable();
  query->addToOutputList( technologyIdColumnName );
  outputDataBuffer.extend< unsigned >( technologyIdColumnName );

  // Execute query.
  query->defineOutput( outputDataBuffer );
  query->setRowCacheSize( 5000 );
  coral::ICursor& cursor = query->execute();

  // Create map for retrieving Token key (link ID) using link ID (Token key) as key.
  std::map< unsigned, std::string > *tokenKeyForLinkId = new std::map< unsigned, std::string >();
  std::map< std::string, unsigned > *linkIdForTokenKey = new std::map< std::string, unsigned >();

  // Get all link ID's and Token keys for collection fragment and add to maps. 
  while( cursor.next() )   {
    unsigned linkId = outputDataBuffer[0].data< unsigned >();
    std::string databaseId = outputDataBuffer[1].data< std::string >();
    std::string containerId = outputDataBuffer[2].data< std::string >();
    std::string classId = outputDataBuffer[3].data< std::string >();
    unsigned technologyId = outputDataBuffer[4].data< unsigned >();
    /*
    Token token;
    token.setDb( databaseId );
    token.setCont( containerId );
    pool::Guid        guid;
    token.setClassID( guid.fromString( classId ) );
    token.setTechnology( technologyId );
    const std::string tokenKey = token.key();
    */
    // MN:this is a hack to build the string representation by hand,
    // but it preserves the technology ID unlike the Token::key() method
    char text[128];
    ::sprintf(text, "][TECH=%08X]", technologyId);
    std::string tokenKey = "[DB="+databaseId+"][CNT="+containerId+"][CLID="+classId+text;

    tokenKeyForLinkId->insert( std::make_pair( linkId, tokenKey ) );
    linkIdForTokenKey->insert( std::make_pair( tokenKey, linkId ) );
  }
 
  m_mapOfTokenKeyForLinkIdMaps[ fragmentName ] = tokenKeyForLinkId;
  m_mapOfLinkIdForTokenKeyMaps[ fragmentName ] = linkIdForTokenKey;
}


void
pool::RelationalCollection::RelationalCollection::createIndex( const std::string& indexName,
                                                               const std::vector< std::string >& columnNames,
                                                               bool isUnique )
{
   coral::ISchema& nominalSchema = m_session->nominalSchema();

   // Get name of collection fragment to contain index.
   std::string fragmentName = m_description.collectionFragmentName( *( columnNames.begin() ) );

  // Get data table name to contain index.
   std::string dataTableName = m_dataTableNameForCollectionFragmentName[ fragmentName ];

  // Check if an index already exists for these columns.
  std::map< std::string, std::vector< std::string >* > collectionColumnNamesForIndexName;
  std::map< std::string, bool > isUniqueFlagForIndexName;
  coral::IQuery* query = nominalSchema.tableHandle( RelationalCollectionNames::nameOfCollectionIndexDescriptionsTable() ).newQuery();
  query->addToOutputList( 
     RelationalCollectionNames::indexNameVariableInCollectionIndexDescriptionsTable() );
  query->addToOutputList( 
    RelationalCollectionNames::uniqueConstraintNameVariableInCollectionIndexDescriptionsTable() );
  query->addToOutputList( 
    RelationalCollectionNames::collectionVariableNameVariableInCollectionIndexDescriptionsTable() );
  m_whereDataForCollectionNameInIndexDescriptionsTable->begin()->data<std::string>() = fragmentName;
  query->setCondition( m_whereClauseForCollectionNameInIndexDescriptionsTable, 
                       *m_whereDataForCollectionNameInIndexDescriptionsTable );
  query->setRowCacheSize( 100 );
  coral::ICursor& cursor = query->execute();
  while ( cursor.next() ) 
  {
    const coral::AttributeList& row = cursor.currentRow();
     
    std::string existingIndexName = row[0].data<std::string>();
    std::string existingUniqueConstraintName = row[1].data<std::string>();
    std::string collectionColumnName = row[2].data<std::string>();

    if( existingIndexName.size() > 0 ) {
      if( existingUniqueConstraintName == existingIndexName ) {
         isUniqueFlagForIndexName.insert( std::make_pair( existingIndexName, true ) );
      }
      else {
         isUniqueFlagForIndexName.insert( std::make_pair( existingIndexName, false ) );
      }

      std::map< std::string, std::vector< std::string >* >::iterator iNames = 
        collectionColumnNamesForIndexName.find( existingIndexName );
      if ( iNames != collectionColumnNamesForIndexName.end() )      {
        iNames->second->push_back( collectionColumnName );
      }
      else {
        std::vector< std::string >* existingColumnNames = new std::vector< std::string >();
        existingColumnNames->push_back( collectionColumnName );
        collectionColumnNamesForIndexName.insert( std::make_pair( existingIndexName, existingColumnNames ) );
      }
    }
  }
  delete query;
  bool indexFound = false;
  for( std::map< std::string, std::vector< std::string >* >::const_iterator iData = 
          collectionColumnNamesForIndexName.begin(); iData != collectionColumnNamesForIndexName.end(); ++iData )
  {
    std::string existingIndexName = iData->first;
    std::vector< std::string >* existingColumnNames = iData->second;
 
    if ( existingColumnNames->size() == columnNames.size() && 
         std::equal( existingColumnNames->begin(),
                     existingColumnNames->end(),
                     columnNames.begin() ) )
    { 
      bool existingIsUnique = ( isUniqueFlagForIndexName.find( existingIndexName ) )->second;
      if ( existingIsUnique == isUnique )
      {
        indexFound = true;
      }
      else
      {
         nominalSchema.tableHandle( dataTableName ).schemaEditor().dropIndex( existingIndexName );

        (*m_whereDataForIndexNameInIndexDescriptionsTable)[0].data<std::string>() = existingIndexName;
        (*m_whereDataForIndexNameInIndexDescriptionsTable)[1].data<std::string>() = fragmentName;
        nominalSchema.tableHandle( RelationalCollectionNames::nameOfCollectionIndexDescriptionsTable() )
           .dataEditor().deleteRows( m_whereClauseForIndexNameInIndexDescriptionsTable,
                                     *m_whereDataForIndexNameInIndexDescriptionsTable );
      }
      break;
    }   
  }
  for ( std::map< std::string, std::vector< std::string >* >::iterator iNames = collectionColumnNamesForIndexName.begin(); 
        iNames != collectionColumnNamesForIndexName.end(); ++iNames )
  {
    delete iNames->second;
  }

  if ( ! indexFound )
  {
    // Get table column names for columns used by this index.
    std::vector< std::string > tableColumnNames;
    for ( std::vector< std::string >::const_iterator iName = columnNames.begin(); iName != columnNames.end(); ++iName )
    {
      std::string collectionColumnName = *iName;
      std::string tableColumnName = ( m_tableColumnNameForCollectionColumnName.find( collectionColumnName ) )->second;

      if ( m_description.column( collectionColumnName ).type() == pool::CollectionBaseNames::tokenTypeName() )
      {
        std::string oid1ColumnName = tableColumnName + 
           RelationalCollectionNames::oid_1_variableInCollectionDataTable();
        tableColumnNames.push_back( oid1ColumnName ); 
        std::string oid2ColumnName = tableColumnName + 
           RelationalCollectionNames::oid_2_variableInCollectionDataTable();
        tableColumnNames.push_back( oid2ColumnName ); 
      }
      else
      {
        tableColumnNames.push_back( tableColumnName ); 
      }
    }

    // Create index in database.
    nominalSchema.tableHandle( dataTableName ).schemaEditor().createIndex( indexName,
                                                                           tableColumnNames,
                                                                           isUnique );

    // Insert rows for new index into collection index descriptions table.
    for ( std::vector< std::string >::const_iterator iName = columnNames.begin(); iName != columnNames.end(); ++iName )
    {
      coral::AttributeList rowBuffer;
      rowBuffer.extend<std::string>( RelationalCollectionNames::indexNameVariableInCollectionIndexDescriptionsTable() );
      rowBuffer.extend<std::string>( RelationalCollectionNames::uniqueConstraintNameVariableInCollectionIndexDescriptionsTable() );
      rowBuffer.extend<std::string>( RelationalCollectionNames::collectionVariableNameVariableInCollectionIndexDescriptionsTable() );
      rowBuffer.extend<std::string>( 
         RelationalCollectionNames::collectionNameVariableInCollectionIndexDescriptionsTable() );
      coral::AttributeList::iterator iAttribute = rowBuffer.begin();
      iAttribute->data< std::string >() = indexName;
      ++iAttribute;
      iAttribute->data< std::string >() = (isUnique? indexName : "");
      ++iAttribute;
      iAttribute->data< std::string >() = *iName;
      ++iAttribute;
      iAttribute->data< std::string >() = fragmentName;
      nominalSchema.tableHandle( RelationalCollectionNames::nameOfCollectionIndexDescriptionsTable() ).dataEditor().insertRow( rowBuffer );
    }
  }
}


void
pool::RelationalCollection::RelationalCollection::
setUniqueConstraint( const std::string& constraintName, const std::vector< std::string >& columnNames )
{
   coral::ISchema& nominalSchema = m_session->nominalSchema();

  // Get name of collection fragment to contain unique constraint.
  std::string fragmentName = m_description.collectionFragmentName( *( columnNames.begin() ) );

  // Get data table name to contain unique constraint.
  std::string dataTableName = m_dataTableNameForCollectionFragmentName[fragmentName];

  // Check if a unique constraint already exists for these columns.
  std::map< std::string, std::vector< std::string >* > collectionColumnNamesForUniqueConstraintName;
  std::map< std::string, bool > isUniqueFlagForUniqueConstraintName;
  coral::IQuery* query = nominalSchema.tableHandle( pool::RelationalCollection::RelationalCollectionNames::nameOfCollectionIndexDescriptionsTable() ).newQuery();
  query->addToOutputList( 
    pool::RelationalCollection::RelationalCollectionNames::indexNameVariableInCollectionIndexDescriptionsTable() );
  query->addToOutputList( 
    pool::RelationalCollection::RelationalCollectionNames::uniqueConstraintNameVariableInCollectionIndexDescriptionsTable() );
  query->addToOutputList( 
    pool::RelationalCollection::RelationalCollectionNames::collectionVariableNameVariableInCollectionIndexDescriptionsTable() );
  m_whereDataForCollectionNameInIndexDescriptionsTable->begin()->data<std::string>() = fragmentName;
  query->setCondition( m_whereClauseForCollectionNameInIndexDescriptionsTable,
                       *m_whereDataForCollectionNameInIndexDescriptionsTable );
  query->setRowCacheSize( 100 );
  coral::ICursor& cursor = query->execute();
  while ( cursor.next() ) 
  {
    const coral::AttributeList& row = cursor.currentRow();
     
    std::string existingIndexName = row[0].data<std::string>();
    std::string existingUniqueConstraintName = row[1].data<std::string>();
    std::string collectionColumnName = row[2].data<std::string>();

    if( existingUniqueConstraintName.size() > 0 && existingUniqueConstraintName != existingIndexName )
    {
      std::map< std::string, std::vector< std::string >* >::iterator iNames = 
        collectionColumnNamesForUniqueConstraintName.find( existingUniqueConstraintName );
      if ( iNames != collectionColumnNamesForUniqueConstraintName.end() )
      {
        iNames->second->push_back( collectionColumnName );
      }
      else
      {
        std::vector< std::string >* existingColumnNames = new std::vector< std::string >();
        existingColumnNames->push_back( collectionColumnName );
        collectionColumnNamesForUniqueConstraintName.insert( std::make_pair( existingUniqueConstraintName, 
                                                                             existingColumnNames ) );
      }
    }
  }
  delete query;
  bool uniqueConstraintFound = false;
  for(         std::map< std::string, std::vector< std::string >* >::const_iterator iData = 
          collectionColumnNamesForUniqueConstraintName.begin();
       iData != collectionColumnNamesForUniqueConstraintName.end(); 
       ++iData )
  {
    std::string existingUniqueConstraintName = iData->first;
    std::vector< std::string >* existingColumnNames = iData->second;
 
    if ( existingColumnNames->size() == columnNames.size() && 
         std::equal( existingColumnNames->begin(),
                     existingColumnNames->end(),
                     columnNames.begin() ) )
    { 
      uniqueConstraintFound = true;
      break;
    }   
  }
  for ( std::map< std::string, std::vector< std::string >* >::iterator iNames = 
           collectionColumnNamesForUniqueConstraintName.begin();
        iNames != collectionColumnNamesForUniqueConstraintName.end();
        ++iNames )
  {
    delete iNames->second;
  }

  if ( ! uniqueConstraintFound )
  {
    // Get table column names for columns used by this unique constraint.
    std::vector< std::string > tableColumnNames;
    for ( std::vector< std::string >::const_iterator iName = columnNames.begin(); iName != columnNames.end(); ++iName )
    {
      std::string collectionColumnName = *iName;
      std::string tableColumnName = ( m_tableColumnNameForCollectionColumnName.find( collectionColumnName ) )->second;

      if ( m_description.column( collectionColumnName ).type() == pool::CollectionBaseNames::tokenTypeName() )
      {
        std::string oid1ColumnName = tableColumnName + 
          pool::RelationalCollection::RelationalCollectionNames::oid_1_variableInCollectionDataTable();
        tableColumnNames.push_back( oid1ColumnName ); 
        std::string oid2ColumnName = tableColumnName + 
          pool::RelationalCollection::RelationalCollectionNames::oid_2_variableInCollectionDataTable();
        tableColumnNames.push_back( oid2ColumnName ); 
      }
      else
      {
        tableColumnNames.push_back( tableColumnName ); 
      }
    }

    // set unique constraint in database.
    nominalSchema.tableHandle( dataTableName ).schemaEditor().createIndex( constraintName,
                                                                           tableColumnNames,
                                                                           true );

    // Insert rows for new unique constraint into collection index descriptions table.
    for ( std::vector< std::string >::const_iterator iName = columnNames.begin(); iName != columnNames.end(); ++iName )
    {
      coral::AttributeList rowBuffer;
      rowBuffer.extend<std::string>( 
        pool::RelationalCollection::RelationalCollectionNames::indexNameVariableInCollectionIndexDescriptionsTable() );
      rowBuffer.extend<std::string>( 
        pool::RelationalCollection::RelationalCollectionNames::uniqueConstraintNameVariableInCollectionIndexDescriptionsTable() );
      rowBuffer.extend<std::string>( 
        pool::RelationalCollection::RelationalCollectionNames::collectionVariableNameVariableInCollectionIndexDescriptionsTable() );
      rowBuffer.extend<std::string>( 
        pool::RelationalCollection::RelationalCollectionNames::collectionNameVariableInCollectionIndexDescriptionsTable() );
      coral::AttributeList::iterator iAttribute = rowBuffer.begin();
      iAttribute->data< std::string >() = "";
      ++iAttribute;
      iAttribute->data< std::string >() = constraintName;
      ++iAttribute;
      iAttribute->data< std::string >() = *iName;
      ++iAttribute;
      iAttribute->data< std::string >() = fragmentName;
      nominalSchema.tableHandle( pool::RelationalCollection::RelationalCollectionNames::nameOfCollectionIndexDescriptionsTable() ).dataEditor().insertRow( rowBuffer );
    }
  }
}


void
pool::RelationalCollection::RelationalCollection::retrieveIndexDescriptions( const std::string& fragmentName )
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  std::map< std::string, std::vector< std::string >* > collectionColumnNamesForIndexName;
  std::map< std::string, bool > isUniqueFlagForIndexName;

  // Find indices for this collection fragment.
  coral::IQuery* query = nominalSchema.tableHandle( pool::RelationalCollection::RelationalCollectionNames::nameOfCollectionIndexDescriptionsTable() ).newQuery();
  query->addToOutputList( 
    pool::RelationalCollection::RelationalCollectionNames::indexNameVariableInCollectionIndexDescriptionsTable() );
  query->addToOutputList( 
    pool::RelationalCollection::RelationalCollectionNames::uniqueConstraintNameVariableInCollectionIndexDescriptionsTable() );
  query->addToOutputList( 
    pool::RelationalCollection::RelationalCollectionNames::collectionVariableNameVariableInCollectionIndexDescriptionsTable() );
  m_whereDataForCollectionNameInIndexDescriptionsTable->begin()->data<std::string>() = fragmentName;
  query->setCondition( m_whereClauseForCollectionNameInIndexDescriptionsTable,
                       *m_whereDataForCollectionNameInIndexDescriptionsTable );
  query->setRowCacheSize( 100 );
  coral::ICursor& cursor = query->execute();
  while ( cursor.next() ) 
  {
    const coral::AttributeList& row = cursor.currentRow();
     
    std::string indexName = row[0].data<std::string>();
    std::string uniqueConstraintName = row[1].data<std::string>();
    std::string collectionColumnName = row[2].data<std::string>();

    if( indexName.size() > 0 )
    {
      if ( uniqueConstraintName == indexName )
      {
        isUniqueFlagForIndexName.insert( std::make_pair( indexName, true ) );
      }
      else
      {
        isUniqueFlagForIndexName.insert( std::make_pair( indexName, false ) );
      }

      std::map< std::string, std::vector< std::string >* >::iterator iNames = 
        collectionColumnNamesForIndexName.find( indexName );
      if ( iNames != collectionColumnNamesForIndexName.end() )
      {
        iNames->second->push_back( collectionColumnName );
      }
      else
      {
        std::vector< std::string >* columnNames = new std::vector< std::string >();
        columnNames->push_back( collectionColumnName );
        collectionColumnNamesForIndexName.insert( std::make_pair( indexName, columnNames ) );
      }
    }
  }
  delete query;

  // Add indices to collection description.
  for( std::map< std::string, std::vector< std::string >* >::const_iterator iIndex = 
          collectionColumnNamesForIndexName.begin();
       iIndex != collectionColumnNamesForIndexName.end();
       ++iIndex )
  {
    std::string indexName = iIndex->first;

    std::map< std::string, bool >::const_iterator isUnique = isUniqueFlagForIndexName.find( indexName );
    if ( isUnique != isUniqueFlagForIndexName.end() )
    {
       // MN: FIX guess - check functionality!
       //m_description.createIndex( indexName, *iIndex->second, isUnique->second );
       m_schemaEditor->createIndex( indexName, *iIndex->second, isUnique->second );
    }
    else
    {
      std::string errorMsg = "Uniqueness flag cannot be found for index with name `"
         + indexName + "'.";
      throw pool::Exception( errorMsg,
                             "RelationalCollection::retrieveIndexDescriptions",
                             "RelationalCollection" );
    }
  }

  // Delete column names from private map.
  for ( std::map< std::string, std::vector< std::string >* >::iterator iNames =
        collectionColumnNamesForIndexName.begin(); iNames != 
        collectionColumnNamesForIndexName.end(); ++iNames )
  {
    delete iNames->second;
  }
}


void
pool::RelationalCollection::RelationalCollection::retrieveUniqueConstraintDescriptions( const std::string& fragmentName )
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  std::map< std::string, std::vector< std::string >* > collectionColumnNamesForUniqueConstraintName;

  // Find unique constraints for this collection fragment.
  coral::IQuery* query = nominalSchema.tableHandle( pool::RelationalCollection::RelationalCollectionNames::nameOfCollectionIndexDescriptionsTable() ).newQuery();
  query->addToOutputList( 
    pool::RelationalCollection::RelationalCollectionNames::indexNameVariableInCollectionIndexDescriptionsTable() );
  query->addToOutputList( 
    pool::RelationalCollection::RelationalCollectionNames::uniqueConstraintNameVariableInCollectionIndexDescriptionsTable() );
  query->addToOutputList( 
    pool::RelationalCollection::RelationalCollectionNames::collectionVariableNameVariableInCollectionIndexDescriptionsTable() );
  m_whereDataForCollectionNameInIndexDescriptionsTable->begin()->data<std::string>() = fragmentName;
  query->setCondition( m_whereClauseForCollectionNameInIndexDescriptionsTable,
                       *m_whereDataForCollectionNameInIndexDescriptionsTable );
  query->setRowCacheSize( 100 );
  coral::ICursor& cursor = query->execute();
  while ( cursor.next() ) 
  {
    const coral::AttributeList& row = cursor.currentRow();
     
    std::string indexName = row[0].data<std::string>();
    std::string uniqueConstraintName = row[1].data<std::string>();
    std::string collectionColumnName = row[2].data<std::string>();

    if( uniqueConstraintName.size() > 0 && uniqueConstraintName != indexName )
    {
       std::map< std::string, std::vector< std::string >* >::iterator iNames =
          collectionColumnNamesForUniqueConstraintName.find( uniqueConstraintName );
       if ( iNames != collectionColumnNamesForUniqueConstraintName.end() )
       {
          iNames->second->push_back( collectionColumnName );
       }
       else
       {
          std::vector< std::string >* columnNames = new std::vector< std::string >();
          columnNames->push_back( collectionColumnName );
          collectionColumnNamesForUniqueConstraintName.insert( std::make_pair( uniqueConstraintName, columnNames ) );
       }
    }
  }
  delete query;

  // Add unique constraints to collection description.
  for( std::map< std::string, std::vector< std::string >* >::const_iterator iConstraint = 
       collectionColumnNamesForUniqueConstraintName.begin(); iConstraint != 
       collectionColumnNamesForUniqueConstraintName.end(); ++iConstraint )
  {
     // MN: FIX guess - check functionality!
     // m_description.setUniqueConstraint( iConstraint->first, *iConstraint->second );
     m_schemaEditor->setUniqueConstraint( iConstraint->first, *iConstraint->second );
  }

  // Delete column names from private map.
  for ( std::map< std::string, std::vector< std::string >* >::iterator iNames =
        collectionColumnNamesForUniqueConstraintName.begin(); iNames != 
        collectionColumnNamesForUniqueConstraintName.end(); ++iNames )
  {
    delete iNames->second;
  }
}


void
pool::RelationalCollection::RelationalCollection::
dropParentForeignKeyConstraints( const std::string& fragmentName, bool keepDefinedConstraint )
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  // Get collection fragment description object.
  const pool::ICollectionFragment& fragment = m_description.collectionFragment( fragmentName );

  // Get name of parent collection fragment in collection description.
  std::string parentFragmentName = fragment.parentCollectionFragmentName();

  // Get data table name of collection fragment.
  std::string dataTableName = m_dataTableNameForCollectionFragmentName[ fragmentName ];
  
  // Drop foreign key constraints used to reference parent collection fragments of this collection fragment.
  coral::IQuery* query = nominalSchema.tableHandle( pool::RelationalCollection::RelationalCollectionNames::nameOfCollectionHeadersTable() ).newQuery();
  query->addToOutputList(
    pool::RelationalCollection::RelationalCollectionNames::collectionNameVariableInCollectionHeadersTable() );
  query->addToOutputList(
    pool::RelationalCollection::RelationalCollectionNames::foreignKeyNameVariableInCollectionHeadersTable() );
  m_whereDataForChildCollectionNameInHeadersTable->begin()->data<std::string>() = fragmentName;
  query->setCondition( m_whereClauseForChildCollectionNameInHeadersTable, *m_whereDataForChildCollectionNameInHeadersTable );
  query->limitReturnedRows( 1, 0 );
  coral::ICursor& cursor = query->execute();
  while ( cursor.next() )
  {
    const coral::AttributeList& row = cursor.currentRow();

    std::string externalParentFragmentName = row[0].data<std::string>();
    std::string externalForeignKeyName = row[1].data<std::string>();

    if ( keepDefinedConstraint && ( externalParentFragmentName == parentFragmentName ) )
    {
      continue;
    }
    else
    {
       if( externalForeignKeyName.size() ) {
          // Drop the foreign key
          nominalSchema.tableHandle( dataTableName ).schemaEditor().dropForeignKey( externalForeignKeyName );

          // Update the collection descriptions table.
          m_whereDataForCollectionNameInHeadersTable->begin()->data<std::string>() = externalParentFragmentName;
          std::string setClause = pool::RelationalCollection::RelationalCollectionNames::foreignKeyNameVariableInCollectionHeadersTable() + " = " + "\'\'";
          nominalSchema.tableHandle( pool::RelationalCollection::RelationalCollectionNames::nameOfCollectionHeadersTable() ).dataEditor().updateRows(
             setClause,
             m_whereClauseForCollectionNameInHeadersTable, 
             *m_whereDataForCollectionNameInHeadersTable );
       }
    }
  }
  delete query;
}


void
pool::RelationalCollection::RelationalCollection::
dropChildForeignKeyConstraint( const std::string& fragmentName )
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  // Get collection fragment description object.
  const pool::ICollectionFragment& fragment = m_description.collectionFragment( fragmentName );

  // Get name of child collection fragment in collection description.
  std::string childFragmentName = fragment.childCollectionFragmentName();

  // Get data table name of collection fragment.
  std::string dataTableName = m_dataTableNameForCollectionFragmentName.find( fragmentName )->second;
 
  // Drop foreign key constraint on this collection fragment if such a constrait exists.
  coral::IQuery* query = nominalSchema.tableHandle( pool::RelationalCollection::RelationalCollectionNames::nameOfCollectionHeadersTable() ).newQuery();
  query->addToOutputList(
     RelationalCollectionNames::childCollectionNameVariableInCollectionHeadersTable() );
  query->addToOutputList(
     RelationalCollectionNames::foreignKeyNameVariableInCollectionHeadersTable() );
  m_whereDataForCollectionNameInHeadersTable->begin()->data<std::string>() = fragmentName;
  query->setCondition( m_whereClauseForCollectionNameInHeadersTable,
                       *m_whereDataForCollectionNameInHeadersTable );
  query->limitReturnedRows( 1, 0 );
  coral::ICursor& cursor = query->execute();
  while( cursor.next() )  {
    const coral::AttributeList& row = cursor.currentRow();

    std::string externalChildFragmentName;
    if( !row[0].isNull() )
       externalChildFragmentName = row[0].data<std::string>();
    std::string externalForeignKeyName;
    if( !row[1].isNull() )
       externalForeignKeyName = row[1].data<std::string>();

    if( externalChildFragmentName == childFragmentName )  {
      if( externalForeignKeyName.size() > 0 ) {
         coral::IQuery* childQuery = nominalSchema.tableHandle( pool::RelationalCollection::RelationalCollectionNames::nameOfCollectionHeadersTable() ).newQuery();
        childQuery->addToOutputList( pool::RelationalCollection::RelationalCollectionNames::dataTableNameVariableInCollectionHeadersTable() );
        m_whereDataForCollectionNameInHeadersTable->begin()->data<std::string>() = externalChildFragmentName;
        childQuery->setCondition( m_whereClauseForCollectionNameInHeadersTable,
                                  *m_whereDataForCollectionNameInHeadersTable );
        childQuery->limitReturnedRows( 1, 0 );
        coral::ICursor& childCursor = childQuery->execute();
        while ( childCursor.next() ) {
          // Drop the foreign key
          const coral::AttributeList& childRow = childCursor.currentRow();
          nominalSchema.tableHandle( childRow[0].data<std::string>() ).schemaEditor().dropForeignKey( externalForeignKeyName );

          // Update the collection headers table.
          m_whereDataForCollectionNameInHeadersTable->begin()->data<std::string>() = fragmentName;
          std::string setClause = RelationalCollectionNames::foreignKeyNameVariableInCollectionHeadersTable() + " = " + "\'\'";
          nominalSchema.tableHandle(
             RelationalCollectionNames::nameOfCollectionHeadersTable() ).dataEditor().updateRows(
              setClause,
              m_whereClauseForCollectionNameInHeadersTable, 
              *m_whereDataForCollectionNameInHeadersTable );
        }
      }
    }
    else
    {
      delete query;

      std::string errorMsg = "Found more than one entry for collection fragment `" + fragmentName + 
         "' in collection headers table.";
      throw pool::Exception( errorMsg,
                             "RelationalCollection::dropChildForeignKeyConstraint",
                             "RelationalCollection" );
    }
  }
  delete query;
}


void
pool::RelationalCollection::RelationalCollection::setForeignKeyConstraint( const std::string& fragmentName )
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  // Get names of child collection fragment and foreign key constraints.
  const pool::ICollectionFragment& fragment = m_description.collectionFragment( fragmentName );
  std::string childFragmentName = fragment.childCollectionFragmentName();
  bool usesForeignKey = fragment.usesForeignKey();
  std::string foreignKeyName = fragment.foreignKeyName();  

  // Check if foreign key constraints already exist for this collection fragment and drop those for collection
  // fragments not defined in the collection description.
  bool foreignKeyFound = false;
  coral::IQuery* query = nominalSchema.tableHandle( RelationalCollectionNames::nameOfCollectionHeadersTable() ).newQuery();
  query->addToOutputList( 
     RelationalCollectionNames::childCollectionNameVariableInCollectionHeadersTable() );
  query->addToOutputList( 
     RelationalCollectionNames::foreignKeyNameVariableInCollectionHeadersTable() );
  m_whereDataForCollectionNameInHeadersTable->begin()->data<std::string>() = fragmentName;
  query->setCondition( m_whereClauseForCollectionNameInHeadersTable,
                       *m_whereDataForCollectionNameInHeadersTable );
  query->limitReturnedRows( 1, 0 );
  coral::ICursor& cursor = query->execute();
  if ( cursor.next() )
  {
    const coral::AttributeList& row = cursor.currentRow();

    std::string existingChildFragmentName;
    if( !row[0].isNull() )
       existingChildFragmentName = row[0].data<std::string>();
    std::string existingForeignKeyName;
    if( !row[1].isNull() )
       existingForeignKeyName = row[1].data<std::string>();

    if( existingForeignKeyName.size() > 0 ) {
      if( existingChildFragmentName == childFragmentName ) {
         foreignKeyFound = true;
        if( ! usesForeignKey ) {
          std::string childDataTableName = ( m_dataTableNameForCollectionFragmentName.find( childFragmentName ) )->second;
          nominalSchema.tableHandle( childDataTableName ).schemaEditor().dropForeignKey( foreignKeyName );
        }
      }
      else {
        coral::IQuery* childQuery = nominalSchema.tableHandle( pool::RelationalCollection::RelationalCollectionNames::nameOfCollectionHeadersTable() ).newQuery();
        childQuery->addToOutputList( RelationalCollectionNames::dataTableNameVariableInCollectionHeadersTable() );
        m_whereDataForCollectionNameInHeadersTable->begin()->data<std::string>() = existingChildFragmentName;
        childQuery->setCondition( m_whereClauseForCollectionNameInHeadersTable,
                                  *m_whereDataForCollectionNameInHeadersTable );
        childQuery->limitReturnedRows( 1, 0 );
        coral::ICursor& childCursor = childQuery->execute();
        if( childCursor.next() )        {
          // Drop foreign keys not defined by collection description.
          std::string existingChildDataTableName = ( childCursor.currentRow() )[0].data<std::string>();
          nominalSchema.tableHandle( existingChildDataTableName ).schemaEditor().dropForeignKey( existingForeignKeyName );
        }
        else {
          delete childQuery;

          std::string errorMsg = "Cannot find child collection fragment of fragment `" + fragmentName + 
            "' with name `" + childFragmentName + "'.";
          throw pool::Exception( errorMsg,
                                 "RelationalCollection::setForeignKeyConstraint",
                                 "RelationalCollection" );
        }
        delete childQuery;
      }
    }
  }
  else {
    std::string errorMsg = "Collection headers table row for collection fragment `" + fragmentName + "' does not exist.";
    delete query;
    throw pool::Exception( errorMsg,
                           "RelationalCollection::setForeignKeyConstraint",
                           "RelationalCollection" );
  }
  delete query;

  // Create foreign key constraint if specified in collection fragment description object and it does not exist yet.
  if ( fragment.usesForeignKey() && ( ! foreignKeyFound ) )
  {
    std::string childDataTableName = ( m_dataTableNameForCollectionFragmentName.find( childFragmentName ) )->second;
    nominalSchema.tableHandle( childDataTableName ).schemaEditor().createForeignKey(
      foreignKeyName,
      RelationalCollectionNames::recordIdVariableInCollectionDataTable(),
      ( m_dataTableNameForCollectionFragmentName.find( fragmentName ) )->second,
      RelationalCollectionNames::recordIdVariableInCollectionDataTable() );

    // Update collection headers table.
    m_whereDataForCollectionNameInHeadersTable->begin()->data<std::string>() = fragmentName;
    std::string setClause = RelationalCollectionNames::childCollectionNameVariableInCollectionHeadersTable() + " = " + childFragmentName + " , " + RelationalCollectionNames::foreignKeyNameVariableInCollectionHeadersTable() + " = " + foreignKeyName;
    nominalSchema.tableHandle( RelationalCollectionNames::nameOfCollectionHeadersTable() ).dataEditor().updateRows(
        setClause,
        m_whereClauseForCollectionNameInHeadersTable, 
        *m_whereDataForCollectionNameInHeadersTable );
  }
}





void
pool::RelationalCollection::RelationalCollection::
retrieveFragmentTableNames( const std::string& fragmentName )
{
   // Retrieve data and links table names for this collection fragment and insert in private maps.
   std::unique_ptr<coral::IQuery> query(
      m_session->nominalSchema().tableHandle( RelationalCollectionNames::nameOfCollectionHeadersTable() ).newQuery() );

   query->addToOutputList( RelationalCollectionNames::dataTableNameVariableInCollectionHeadersTable() );
   query->addToOutputList( RelationalCollectionNames::linksTableNameVariableInCollectionHeadersTable() );
   query->addToOutputList( RelationalCollectionNames::writtenRecordsVariableInCollectionHeadersTable() );
   query->defineOutputType( RelationalCollectionNames::writtenRecordsVariableInCollectionHeadersTable(), "int" );
   m_whereDataForCollectionNameInHeadersTable->begin()->data<std::string>() = fragmentName;
   query->setCondition( m_whereClauseForCollectionNameInHeadersTable, *m_whereDataForCollectionNameInHeadersTable );
   query->limitReturnedRows( 1, 0 );

   coral::ICursor& cursor = query->execute();
   if( cursor.next() ) {
      const coral::AttributeList& row = cursor.currentRow();
      m_dataTableNameForCollectionFragmentName[fragmentName] = row[0].data<std::string>();
      m_linksTableNameForCollectionFragmentName[fragmentName] = row[1].data<std::string>();
      m_fragmentDataMap[fragmentName].setCurrentRowId( row[2].data<int>() );
   }
   else {
      std::string errorMsg = "No data and links tables found for collection fragment `"
         + fragmentName + "'.";
      throw pool::Exception( errorMsg,
                             "RelationalCollection::initialize",
                             "RelationalCollection" );
   }
}




pool::ICollectionGUIDQuery*
pool::RelationalCollection::RelationalCollection::newGUIDQuery() const
{
   return new GUIDQuery(
      *m_session,
      m_description,
      m_dataTableNameForCollectionFragmentName,
      m_linksTableNameForCollectionFragmentName,
      m_tableTokenColumnPrefixForCollectionTokenColumnName,
      m_tableAttributeColumnNameForCollectionAttributeColumnName,
      *m_tableAttributeList,
      m_mapOfLinkIdForTokenKeyMaps,
      m_mapOfTokenKeyForLinkIdMaps );
}

