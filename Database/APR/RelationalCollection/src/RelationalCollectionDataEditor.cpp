/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RelationalCollectionDataEditor.h"
#include "RelationalCollectionNames.h"
#include "RelationalCollectionBindVariables.h"
#include "RelationalCollectionQuery.h"
#include "RelationalCollectionCursor.h"

#include "PersistentDataModel/Token.h"

#include "CollectionBase/ICollectionDescription.h"
#include "CollectionBase/CollectionRowBuffer.h"
#include "CollectionBase/TokenList.h"
#include "CollectionBase/ICollectionCursor.h"
#include "CollectionBase/ICollectionFragment.h"
#include "CollectionBase/ICollectionColumn.h"

#include "CoralBase/MessageStream.h"
#include "POOLCore/Exception.h"

#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ISchema.h"

#include "RelationalAccess/ConnectionService.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/ITableDataEditor.h"
#include "RelationalAccess/IBulkOperation.h"
#include "RelationalAccess/IConnectionServiceConfiguration.h"

#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"

#include <sstream>
#include <cstdio> // For sprintf (bug #69271)
//#include <iostream>
#include <memory>
using namespace std;



template< typename T >
inline void 
extendAttributeList( coral::AttributeList& list, const std::string& attribute, const T& value )
{
   list.extend<T>( attribute );
   list[ attribute ].data<T>() = value;
}


pool::RelationalCollection::RelationalCollectionDataEditor::RelationalCollectionDataEditor(
   coral::ISessionProxy& session,
   const pool::ICollectionDescription& description,
   const std::map< std::string, std::string >& dataTableNameForCollectionFragmentName,
   const std::map< std::string, std::string >& linksTableNameForCollectionFragmentName,
   FragmentDataMap& fragmentDataMap,
   const std::map< std::string, std::string >& tableTokenColumnPrefixForCollectionTokenColumnName,
   const std::map< std::string, std::string >& tableAttributeColumnNameForCollectionAttributeColumnName,
   const coral::AttributeList& tableAttributeList,
   pool::CollectionRowBuffer& collectionRowBuffer,
   std::map< std::string, coral::AttributeList* >& dataTableRowBufferForCollectionFragmentName,
   coral::AttributeList& linksTableRowBuffer,
   std::map< std::string, std::map< std::string, unsigned >* >& mapOfLinkIdForTokenKeyMaps,
   std::map< std::string, std::map< unsigned, std::string >* >& mapOfTokenKeyForLinkIdMaps,
   const std::map< std::string, std::string >& mapOfWhereClausesForOID1InDataTable,
   const std::map< std::string, coral::AttributeList* >& mapOfWhereDataForOID1InDataTable )
      : m_rowCacheSize( 0 ),
        m_rowIDAllocationStep( 100 ),
        m_session( session ),
        m_schema( &m_session.nominalSchema() ),
        m_description( description ),
        m_dataTableNameForCollectionFragmentName( dataTableNameForCollectionFragmentName ),
        m_linksTableNameForCollectionFragmentName( linksTableNameForCollectionFragmentName ),
        m_fragmentDataMap( fragmentDataMap ),
        m_tableTokenColumnPrefixForCollectionTokenColumnName( tableTokenColumnPrefixForCollectionTokenColumnName ),
        m_tableAttributeColumnNameForCollectionAttributeColumnName( tableAttributeColumnNameForCollectionAttributeColumnName ),
        m_tableAttributeList( tableAttributeList ),
        m_collectionRowBuffer( collectionRowBuffer ),
        m_dataTableRowBufferForCollectionFragmentName( dataTableRowBufferForCollectionFragmentName ),
        m_linksTableRowBuffer( linksTableRowBuffer ),
        m_mapOfLinkIdForTokenKeyMaps( mapOfLinkIdForTokenKeyMaps ),
        m_mapOfTokenKeyForLinkIdMaps( mapOfTokenKeyForLinkIdMaps ),
        m_mapOfWhereClausesForOID1InDataTable( mapOfWhereClausesForOID1InDataTable ),
        m_mapOfWhereDataForOID1InDataTable( mapOfWhereDataForOID1InDataTable ),
        m_whereClauseForCollectionNameInHeadersTable( RelationalCollectionBindVariables::whereClauseForCollectionNameInHeadersTable() ),
        m_whereDataForCollectionNameInHeadersTable( new coral::AttributeList ),
        m_whereClauseForPrimaryKeyInDataTable( RelationalCollectionBindVariables::whereClauseForPrimaryKeyInDataTable() ),
        m_whereDataForPrimaryKeyInDataTable( new coral::AttributeList ),
        m_whereClauseForLinkIdInLinksTable( RelationalCollectionBindVariables::whereClauseForLinkIdInLinksTable() ),
        m_whereDataForLinkIdInLinksTable( new coral::AttributeList ),
        m_bulkRowOperationForCollectionFragmentName(),
        m_rowIDSession( 0 )
{
   // Define bind variable type used in where clause to find collection fragment name in collection headers table.
   m_whereDataForCollectionNameInHeadersTable->extend<std::string>(
      RelationalCollectionNames::collectionNameVariableInCollectionHeadersTable() );

   // Define bind variable type used in where clause to find primary key in data table of collection.
   m_whereDataForPrimaryKeyInDataTable->extend<unsigned>(
      pool::RelationalCollection::RelationalCollectionNames::recordIdVariableInCollectionDataTable() );

   // Define bind variable type used in where clause to find link ID in links table of collection.
   m_whereDataForLinkIdInLinksTable->extend<unsigned>(
      pool::RelationalCollection::RelationalCollectionNames::linkIdVariableInCollectionLinksTable() );
}



pool::RelationalCollection::RelationalCollectionDataEditor::~RelationalCollectionDataEditor()
{
   delete m_whereDataForCollectionNameInHeadersTable;
   delete m_whereDataForPrimaryKeyInDataTable;
   delete m_whereDataForLinkIdInLinksTable;
   delete m_rowIDSession;
}


void
pool::RelationalCollection::RelationalCollectionDataEditor::setRowCacheSize( int rowCacheSize )
{
   if( rowCacheSize <= 0 )
      return;

   flushCachedRows();
   deleteRowCache();

   m_rowCacheSize = rowCacheSize;
   createRowCache();
 
   setRowIDAllocationStep( rowCacheSize );
}


void
pool::RelationalCollection::RelationalCollectionDataEditor::createRowCache()
{
   if( m_rowCacheSize <= 0 )
      return;

   for( std::map< std::string, coral::AttributeList* >::const_iterator iData =
            m_dataTableRowBufferForCollectionFragmentName.begin(); iData != 
            m_dataTableRowBufferForCollectionFragmentName.end(); ++iData )
   {
      // Get collection fragment name.
      std::string fragmentName = iData->first;

      // Get data table name.
      std::string dataTableName = m_dataTableNameForCollectionFragmentName.find( fragmentName )->second;

      // Get reference to data table row buffer.
      coral::AttributeList* dataTableRowBuffer = iData->second;
    
      // Create new bulk operation object for this collection fragment and cache size.
      coral::ITable& dataTable = m_schema->tableHandle( dataTableName );
      coral::IBulkOperation* bulkOperation = dataTable.dataEditor().bulkInsert( *dataTableRowBuffer, m_rowCacheSize );
  
      // Store bulk operation object for this collection fragment.
      m_bulkRowOperationForCollectionFragmentName[ fragmentName ] = bulkOperation;
   }
}

   
void
pool::RelationalCollection::RelationalCollectionDataEditor::flushCachedRows()
{
   if( m_rowCacheSize <= 0 )
      return;

   for( std::map< std::string, coral::IBulkOperation* >::const_iterator iOper =
           m_bulkRowOperationForCollectionFragmentName.begin();
        iOper != m_bulkRowOperationForCollectionFragmentName.end();
        ++iOper ) {
      iOper->second->flush();
   }
}

  

void
pool::RelationalCollection::RelationalCollectionDataEditor::deleteRowCache()
{
   if( m_rowCacheSize <= 0 )
      return;

   for( std::map< std::string, coral::IBulkOperation* >::const_iterator iOper =
           m_bulkRowOperationForCollectionFragmentName.begin();
        iOper != m_bulkRowOperationForCollectionFragmentName.end();
        ++iOper ) {
      delete iOper->second;
   }
   m_bulkRowOperationForCollectionFragmentName.clear();
}

    

void
pool::RelationalCollection::RelationalCollectionDataEditor::
setRowIDAllocationStep( int step )
{
   
   if( step < 100 )  m_rowIDAllocationStep = 100;
   else if( step > 1000 ) m_rowIDAllocationStep = 1000;
   else m_rowIDAllocationStep = step;
}


   
pool::CollectionRowBuffer&
pool::RelationalCollection::RelationalCollectionDataEditor::rowBuffer()
{
   return m_collectionRowBuffer;
}


pool::CollectionRowBuffer&
pool::RelationalCollection::RelationalCollectionDataEditor::emptyRowBuffer()
{
   clearRowBuffers();
   return m_collectionRowBuffer;
}


void
pool::RelationalCollection::RelationalCollectionDataEditor::clearRowBuffers()
{
   // NOTE - similar code is in main collection.cpp and schema editor - keep in synch
   
   // Create empty collection and data table row buffers and reset private map.
   pool::TokenList                 tokenList;
   coral::AttributeList         attributeList;
   for( int i = 0; i < m_description.numberOfCollectionFragments(); i++ )
   {
      coral::AttributeList        *dataTableRowBuffer = new coral::AttributeList;
      std::string                 fragmentName = m_description.collectionFragment( i ).name();
      // Add primary key column - only temporary fix?  MN
      dataTableRowBuffer->extend( RelationalCollectionNames::recordIdVariableInCollectionDataTable(),
				  coral::AttributeSpecification::typeNameForType<unsigned>() );
      for( int j = 0; j < m_description.numberOfTokenColumns( fragmentName ); j++ )
      {
         const std::string& collectionColumnName = m_description.tokenColumn( j, fragmentName ).name();
         tokenList.extend( collectionColumnName );
         std::string tableColumnPrefix = 
            ( m_tableTokenColumnPrefixForCollectionTokenColumnName.find( collectionColumnName ) )->second;
         std::string oid1ColumnName = tableColumnPrefix +
            RelationalCollectionNames::oid_1_variableInCollectionDataTable();
         std::string oid2ColumnName = tableColumnPrefix +
            RelationalCollectionNames::oid_2_variableInCollectionDataTable();
         dataTableRowBuffer->extend( oid1ColumnName, typeid(unsigned) );
         dataTableRowBuffer->extend( oid2ColumnName, typeid(unsigned) );
      }
      for( int j = 0; j < m_description.numberOfAttributeColumns( fragmentName ); j++ )
      {
         const std::string& collectionColumnName = m_description.attributeColumn( j, fragmentName ).name();
         const std::string& columnType = m_description.attributeColumn( j, fragmentName ).type();
         attributeList.extend( collectionColumnName, columnType );
         std::string tableColumnName = 
            ( m_tableAttributeColumnNameForCollectionAttributeColumnName.find( collectionColumnName ) )->second;
         dataTableRowBuffer->extend( tableColumnName, columnType );
      }
      // maybe a check for existance is needed // MN
      delete m_dataTableRowBufferForCollectionFragmentName[fragmentName];
      m_dataTableRowBufferForCollectionFragmentName[fragmentName] = dataTableRowBuffer;
   }
   m_collectionRowBuffer.setTokenList( tokenList );
   m_collectionRowBuffer.setAttributeList( attributeList );

   setRowCacheSize( m_rowCacheSize );   // Needed to take into account the new row buffer?
}



int 
pool::RelationalCollection::RelationalCollectionDataEditor::
getUniqueRowID( )
{
   // Obtain unique RowID number
   // ==============================
   std::string  fragmentName = m_description.name();   // this is the main fragment
   FragmentData &fragmentData = m_fragmentDataMap[ fragmentName ];
   if( !fragmentData.hasAllocatedIds() ) {

      if( !m_rowIDSession ) {
         unique_ptr<coral::ConnectionService> connectionService( new coral::ConnectionService() );
	 //cout << ">>>>>>>   Timeout=" << connectionService->configuration().connectionTimeOut() << endl;
         m_rowIDSession = connectionService->connect( m_description.connection(), coral::Update );
      }

      // Start a transaction.
      m_rowIDSession->transaction().start();

      m_whereDataForCollectionNameInHeadersTable->begin()->data<std::string>() = fragmentName;
      coral::ITable& headersTable = m_rowIDSession->nominalSchema().tableHandle( RelationalCollectionNames::nameOfCollectionHeadersTable() );

      std::unique_ptr<coral::IQuery> query( headersTable.newQuery() );
      query->addToOutputList( RelationalCollectionNames::writtenRecordsVariableInCollectionHeadersTable() );
      query->defineOutputType( RelationalCollectionNames::writtenRecordsVariableInCollectionHeadersTable(), "unsigned int" );
      query->setCondition( m_whereClauseForCollectionNameInHeadersTable, *m_whereDataForCollectionNameInHeadersTable );
      query->limitReturnedRows( 1, 0 );
      query->setForUpdate();

      coral::ICursor& cursor = query->execute();
      if( !cursor.next() ) {
         std::string errorMsg = "Failed to query number of records written for collection `"
            + m_description.name() + "'.";
         throw pool::Exception( errorMsg, "RelationalCollectionDataEditor::insertRow",
                                "RelationalCollection" );
      }
      int currentID = cursor.currentRow()[0].data<unsigned>();
      delete query.release();

      fragmentData.allocateIDs( currentID, m_rowIDAllocationStep );

      coral::MessageStream log( "pool::RelationalCollection::insertRow()" );
      log << coral::Debug
          << " Allocated new range of rowIDs: " << currentID << " - "
          << currentID + m_rowIDAllocationStep  << coral::MessageStream::endmsg;

      std::ostringstream setClause;
      const string      recwrittenBVName = "RECORDS_WRITTEN";
      setClause << RelationalCollectionNames::writtenRecordsVariableInCollectionHeadersTable() << " = :" <<  recwrittenBVName;
      coral::AttributeList bindVariables;
      extendAttributeList<int>( bindVariables, recwrittenBVName, currentID + m_rowIDAllocationStep );

      extendAttributeList<string>( bindVariables, RelationalCollectionNames::collectionNameVariableInCollectionHeadersTable(), fragmentName );
      headersTable.dataEditor().updateRows( setClause.str(), m_whereClauseForCollectionNameInHeadersTable, bindVariables );

      m_rowIDSession->transaction().commit();
   }

   return fragmentData.getNextRowId();
}



void 
pool::RelationalCollection::RelationalCollectionDataEditor::
insertRow( const pool::CollectionRowBuffer& inputRowBuffer, bool /* updateRecordsCounter */ )
{
   int  rowID = -1;   // no rowID generated yet
   
   // Add input Token data to data and links table row buffers.
   for( pool::TokenList::const_iterator iToken = inputRowBuffer.tokenList().begin();
         iToken != inputRowBuffer.tokenList().end(); ++iToken )
   {
      // Get collection column name and table column name prefix.
      std::string collectionColumnName = iToken.tokenName();
      // cout << " ----------   ColumnName = " << collectionColumnName << endl;
      std::string tableColumnNamePrefix = m_tableTokenColumnPrefixForCollectionTokenColumnName.find(collectionColumnName)->second;

      // Get data table column names for Token.
      std::string oid1ColumnName = tableColumnNamePrefix + RelationalCollectionNames::oid_1_variableInCollectionDataTable();
      std::string oid2ColumnName = tableColumnNamePrefix + RelationalCollectionNames::oid_2_variableInCollectionDataTable();

      // Get name of collection fragment that contains Token.
      std::string fragmentName = m_description.collectionFragmentName( collectionColumnName );
    
      // Get data table row buffer for collection fragment.
      coral::AttributeList& dataTableRowBuffer = *m_dataTableRowBufferForCollectionFragmentName[fragmentName];

      // Get mappings of link ID's to Token keys for this collection fragment.
      std::map< std::string, unsigned >
         *linkIdForTokenKey = m_mapOfLinkIdForTokenKeyMaps[ fragmentName ];
      std::map< unsigned, std::string >
         *tokenKeyForLinkId = m_mapOfTokenKeyForLinkIdMaps[ fragmentName ];

      // Look for this Token key in map.
      //const std::string tokenKey = iToken->key();
      // MN:this is a hack to build the string representation by hand,
      // but it preserves the technology ID unlike the Token::key() method
      char text[128];
      ::sprintf(text, "][TECH=%08X]", iToken->technology());
      const std::string tokenKey = "[DB=" + iToken->dbID().toString() + "][CNT=" + iToken->contID()
         + "][CLID=" + iToken->classID().toString() + text;
      //cout << " --- Checking Token key: " << tokenKey << endl;
      map< string, unsigned >::const_iterator linkIdIter = linkIdForTokenKey->find( tokenKey );
      if( linkIdIter == linkIdForTokenKey->end() ) {
         // Create new link ID. 
         //cout << " --- Adding new Token key " << tokenKey << endl;
         unsigned linkId = getUniqueRowID();
         if( rowID < 0 ) {
            rowID = linkId;   // reuse the unique rowID for data row
         }

         // Store new link ID in maps.
         linkIdForTokenKey->insert( std::make_pair( tokenKey, linkId ) );
         tokenKeyForLinkId->insert( std::make_pair( linkId, tokenKey ) );

         // Get name of links table name for this collection fragment.
         std::string linksTableName = m_linksTableNameForCollectionFragmentName.find( fragmentName )->second;
         
         // Store new token key information in links table row buffer.
         m_linksTableRowBuffer[RelationalCollectionNames::linkIdVariableInCollectionLinksTable()].data<unsigned>() = linkId;
         m_linksTableRowBuffer[RelationalCollectionNames::databaseIdVariableInCollectionLinksTable()].data<std::string>() = iToken->dbID().toString();
         m_linksTableRowBuffer[RelationalCollectionNames::containerIdVariableInCollectionLinksTable()].data<std::string>() = iToken->contID();
         m_linksTableRowBuffer[RelationalCollectionNames::classIdVariableInCollectionLinksTable()].data<std::string>() = iToken->classID().toString();
         m_linksTableRowBuffer[RelationalCollectionNames::technologyIdVariableInCollectionLinksTable()].data<unsigned>() = iToken->technology();

         // Insert new row into links table.
         //cout << " Insert new row into links table " << tokenKey << ", ROW ID = " << linkId;
         m_schema->tableHandle( linksTableName ).dataEditor().insertRow( m_linksTableRowBuffer );
         // Set OID_1 of Token to new link ID.
         dataTableRowBuffer[oid1ColumnName].data<unsigned>() = linkId;
         //cout << " Inserting token,  ROW ID = " <<  linkId<< endl;
      }
      else {
         //cout << " Inserting token,  ROW ID = " <<  linkIdIter->second << endl;
         // Set OID_1 of Token to existing link ID.
         dataTableRowBuffer[oid1ColumnName].data<unsigned>() = linkIdIter->second;
      }
      //cout << " Token, OID2= " << iToken->oid().second << endl;
      // Set OID_2 of Token.
      dataTableRowBuffer[oid2ColumnName].data<unsigned>() = iToken->oid().second;
   }

   // Bind input Attribute data to data table row buffers.
   // ================== Attribute part ================
   for( coral::AttributeList::const_iterator iAttribute = inputRowBuffer.attributeList().begin();
        iAttribute != inputRowBuffer.attributeList().end(); ++iAttribute )
   {
      // Get collection Attribute column name.
      std::string collectionColumnName = iAttribute->specification().name();

      // Get name of collection fragment that contains this Attribute.
      std::string fragmentName = m_description.collectionFragmentName( collectionColumnName );
    
      // Get reference to data table row buffer for collection fragment.
      coral::AttributeList& dataTableRowBuffer = *m_dataTableRowBufferForCollectionFragmentName[fragmentName];

      // Bind collection Attribute column values to corresponding Attribute column values in data table row buffer.
      const std::string& tableColumnName = m_tableAttributeColumnNameForCollectionAttributeColumnName.find(collectionColumnName)->second;

      dataTableRowBuffer[tableColumnName].bindUnsafely( iAttribute->addressOfData() );
   }

   // generate rowID if it was not done earlier
   if( rowID < 0 ) rowID = getUniqueRowID();
         
   // Insert new row into data table of each corresponding collection fragment.
   // ========================= Inserting Rows for each Fragment =============
   for( std::map< std::string, coral::AttributeList* >::const_iterator iData = 
           m_dataTableRowBufferForCollectionFragmentName.begin();
        iData != m_dataTableRowBufferForCollectionFragmentName.end(); ++iData )
   {
      // Get collection fragment name and the corresponding row buffer
      const std::string          &fragmentName = iData->first;
      coral::AttributeList       &dataTableRowBuffer = *(iData->second);

      dataTableRowBuffer[RelationalCollectionNames::recordIdVariableInCollectionDataTable()].data<unsigned>() = rowID;
      // Insert row or cache it for bulk row insertion.
      if( m_rowCacheSize == 0 )  {
         const std::string& dataTableName = m_dataTableNameForCollectionFragmentName.find(fragmentName)->second;
         m_schema->tableHandle( dataTableName ).dataEditor().insertRow( dataTableRowBuffer );
      } else {
         m_bulkRowOperationForCollectionFragmentName[ fragmentName ]->processNextIteration();
      }
   }
   
}


int
pool::RelationalCollection::RelationalCollectionDataEditor::
updateRows( coral::AttributeList* attributeSetList,
            pool::TokenList* tokenSetList,
            const std::string& whereClause,
            coral::AttributeList* attributeBindData,
            pool::TokenList* tokenBindData )
{
   // Check that a list of Attribute and/or Token names to be updated was provided as input.
   if ( ( ! attributeSetList ) && ( ! tokenSetList ) )
   {
      std::string errorMsg = "No list of Attribute and/or Token names and associated new values were provided as input. Nothing to update.";
      throw pool::Exception( errorMsg,
                             "RelationalCollectionDataEditor::updateRows",
                             "RelationalCollection" );
   }

   // Create a new query object.
   RelationalCollectionQuery *query = new RelationalCollectionQuery( 
      m_session,
      m_description,
      m_dataTableNameForCollectionFragmentName,
      m_linksTableNameForCollectionFragmentName,
      m_tableTokenColumnPrefixForCollectionTokenColumnName,
      m_tableAttributeColumnNameForCollectionAttributeColumnName,
      m_tableAttributeList,
      m_mapOfLinkIdForTokenKeyMaps,
      m_mapOfTokenKeyForLinkIdMaps );

   // Add Tokens to update to query output list.
   if (tokenSetList) {
     for( pool::TokenList::const_iterator iToken = tokenSetList->begin(); iToken != tokenSetList->end(); ++iToken )
     {
       query->addToOutputList( iToken.tokenName() );
     } 
   }

   // Set query condition.
   query->setCondition( whereClause, attributeBindData, tokenBindData );

   // Execute query and get cursor over rows that pass query. Primary key added to output list by default.
   RelationalCollectionCursor& cursor = *dynamic_cast<RelationalCollectionCursor*>(&query->execute());

   long rowCounter = 0;
   while( ! cursor.next() )   {
      // Get primary key for this row and use it as where clause data.
      m_whereDataForPrimaryKeyInDataTable->begin()->data<unsigned>() = cursor.primaryKey();

      // Make requested updates to Attribute columns for this row.
      if (attributeSetList) {
        for ( coral::AttributeList::const_iterator iAttribute = attributeSetList->begin(); iAttribute !=
                attributeSetList->end(); ++iAttribute )
        {
          // Get names of Attribute column and associated data table.
          std::string collectionColumnName = iAttribute->specification().name();
          std::string tableColumnName = 
            ( m_tableAttributeColumnNameForCollectionAttributeColumnName.find( collectionColumnName ) )->second;
          std::string fragmentName = m_description.collectionFragmentName( collectionColumnName );
          std::string dataTableName = ( m_dataTableNameForCollectionFragmentName.find( fragmentName ) )->second;
          
          // Define set clause.
          std::ostringstream setClauseStream;
          //setClauseStream << tableColumnName << " = " << **iAttribute;
          // This is my interpretation of what should happen, check??   FIXME
          setClauseStream << tableColumnName << " = ";
          bool valueOnly(true);
          iAttribute->toOutputStream( setClauseStream, valueOnly );

          // Update row.
          m_schema->tableHandle( dataTableName ).dataEditor().updateRows(
            setClauseStream.str(),
            m_whereClauseForPrimaryKeyInDataTable,
            *m_whereDataForPrimaryKeyInDataTable );
        }
      }

      // Make requested updates to Token columns for this row.
      if (tokenSetList) {
        for( pool::TokenList::const_iterator iToken = tokenSetList->begin(); iToken != tokenSetList->end(); ++iToken )
        {
          // Get names of Token OID columns and associated data and links tables.
          std::string collectionColumnName = iToken.tokenName();
          std::string tableColumnPrefix = m_tableTokenColumnPrefixForCollectionTokenColumnName.find( collectionColumnName )->second;
          std::string oid1ColumnName = tableColumnPrefix + RelationalCollectionNames::oid_1_variableInCollectionDataTable();
          std::string oid2ColumnName = tableColumnPrefix + RelationalCollectionNames::oid_2_variableInCollectionDataTable();
          std::string fragmentName = m_description.collectionFragmentName( collectionColumnName );
          std::string dataTableName = m_dataTableNameForCollectionFragmentName.find( fragmentName )->second;
          std::string linksTableName = m_linksTableNameForCollectionFragmentName.find( fragmentName )->second;
          
          // Get mappings of link ID's to Token keys for this collection fragment.
          std::map< std::string, unsigned >* linkIdForTokenKey =
            ( m_mapOfLinkIdForTokenKeyMaps.find( fragmentName ) )->second;
          std::map< unsigned, std::string >* tokenKeyForLinkId =
            ( m_mapOfTokenKeyForLinkIdMaps.find( fragmentName ) )->second;

          // Look for Token key in map and add new entry to links table if it does not exist.
          unsigned newOID1ColumnValue = 0;
          unsigned newOID2ColumnValue = 0;
          std::string tokenKey = iToken->key();
          if ( linkIdForTokenKey->find( tokenKey ) == linkIdForTokenKey->end() )
          {
            // Create new link ID.
            unsigned linkId = 0;
            while( tokenKeyForLinkId->find(linkId) != tokenKeyForLinkId->end() ) {
               linkId++;
            }

            // Store link ID in maps.
            linkIdForTokenKey->insert( std::make_pair( tokenKey, linkId ) );
            tokenKeyForLinkId->insert( std::make_pair( linkId, tokenKey ) );

            // Get links table column names.
            std::string linkIdColumnName = linksTableName + "." + 
               RelationalCollectionNames::linkIdVariableInCollectionLinksTable();
            std::string dbIdColumnName = linksTableName + "." + 
               RelationalCollectionNames::databaseIdVariableInCollectionLinksTable();
            std::string contIdColumnName = linksTableName + "." + 
               RelationalCollectionNames::containerIdVariableInCollectionLinksTable();
            std::string classIdColumnName = linksTableName + "." + 
               RelationalCollectionNames::classIdVariableInCollectionLinksTable();
            std::string technologyIdColumnName = linksTableName + "." + 
               RelationalCollectionNames::technologyIdVariableInCollectionLinksTable();

            // Store new token key information in links table row buffer.
            Token token;
            token.fromString( tokenKey );
            m_linksTableRowBuffer[linkIdColumnName].data<unsigned>() = linkId;
            m_linksTableRowBuffer[dbIdColumnName].data<std::string>() = token.dbID().toString();
            m_linksTableRowBuffer[contIdColumnName].data<std::string>() = token.contID();
            m_linksTableRowBuffer[classIdColumnName].data<std::string>() = token.classID().toString();
            m_linksTableRowBuffer[technologyIdColumnName].data<unsigned>() = token.technology();

            // Insert new row into links table.
            m_schema->tableHandle( linksTableName ).dataEditor().insertRow( m_linksTableRowBuffer );

            // Set OID_1 of Token to new link ID.
            newOID1ColumnValue = linkId;
          }
          else {
            // Set OID_1 of Token to existing link ID.
            newOID1ColumnValue = ( linkIdForTokenKey->find( tokenKey ) )->second;
         }

          // Get OID_2 of Token.
          newOID2ColumnValue = iToken->oid().second;

          // Define set clause.
          std::ostringstream setClauseStream;
          setClauseStream << oid1ColumnName << " = " << newOID1ColumnValue << " , "
                          << oid2ColumnName << " = " << newOID2ColumnValue;

          // Update row.
          m_schema->tableHandle( dataTableName ).dataEditor().updateRows( setClauseStream.str(),
                                                                          m_whereClauseForPrimaryKeyInDataTable,
                                                                          *m_whereDataForPrimaryKeyInDataTable );

          // Get old OID_1 column value.
          // int oldOID1ColumnValue = cursor.currentRow().getToken( collectionColumnName ).oid()->first;
          // another guess,  FIXME??
          unsigned oldOID1ColumnValue = cursor.currentRow().tokenList()[collectionColumnName].oid().first;

          // Look for other Tokens with old OID_1 value in data table.
          if ( newOID1ColumnValue != oldOID1ColumnValue )
          {
            bool oldOID1ColumnValueFound = false;
            //for ( int i = 0; i < m_description.numberOfTokens( i, fragmentName ); i++ )
            // another guess,  FIXME??
            for( int i = 0; i < m_description.numberOfTokenColumns(fragmentName); i++ )
            {
               std::string tokenName = m_description.tokenColumn( i, fragmentName ).name();
               coral::IQuery* oid1Query = m_schema->tableHandle( dataTableName ).newQuery();
               coral::AttributeList* whereDataForOID1InDataTable = 
                  ( m_mapOfWhereDataForOID1InDataTable.find( tokenName ) )->second;
               // FIXME?? assume that the attribute list has only 1 attribute
               whereDataForOID1InDataTable->begin()->data<unsigned>() = oldOID1ColumnValue;
               oid1Query->setCondition( ( m_mapOfWhereClausesForOID1InDataTable.find( tokenName ) )->second, 
                                        *whereDataForOID1InDataTable );
               oid1Query->setRowCacheSize( 1000 );
               coral::ICursor& oid1Cursor = oid1Query->execute();
               if ( oid1Cursor.next() )
               {
                  oldOID1ColumnValueFound = true; 
                  delete oid1Query;
                  break;
               }

               delete oid1Query;
            }

            // Delete row for this link ID if it is no longer needed in the links table and update private maps.
            if( ! oldOID1ColumnValueFound ) {
               m_whereDataForLinkIdInLinksTable->begin()->data<unsigned>() = oldOID1ColumnValue;
               m_schema->tableHandle( linksTableName ).dataEditor().deleteRows( m_whereClauseForLinkIdInLinksTable, 
                                                                               *m_whereDataForLinkIdInLinksTable );
               linkIdForTokenKey->erase( tokenKey );
               tokenKeyForLinkId->erase( oldOID1ColumnValue );
            }
          } 
        }
      }

      // Increment counter of number of rows updated.
      rowCounter++;
   } 

   // Cleanup.
   delete query;

   return rowCounter;
}


int
pool::RelationalCollection::RelationalCollectionDataEditor::
deleteRows( const std::string& whereClause, coral::AttributeList* attributeBindData, pool::TokenList* tokenBindData, bool updateRecordsCounter )
{
   // Check that a condition for deletion has been provided.
   if( whereClause.empty() )   {
      std::string errorMsg = "Must provide a condition for row deletion. If all rows are to be deleted use method `ICollectionService::drop' to drop collection `" + m_description.name() + "'.";
      throw pool::Exception( errorMsg,
                             "RelationalCollectionDataEditor::deleteRows",
                             "RelationalCollection" );
   }

   // Create new collection query object, select all Tokens columns and set where clause.
   pool::ICollectionQuery* query = new pool::RelationalCollection::RelationalCollectionQuery( 
      m_session,
      m_description,
      m_dataTableNameForCollectionFragmentName,
      m_linksTableNameForCollectionFragmentName,
      m_tableTokenColumnPrefixForCollectionTokenColumnName,
      m_tableAttributeColumnNameForCollectionAttributeColumnName,
      m_tableAttributeList,
      m_mapOfLinkIdForTokenKeyMaps,
      m_mapOfTokenKeyForLinkIdMaps );
   query->selectAllTokens();
   query->setRowCacheSize( 1000 );
   query->setCondition( whereClause, attributeBindData, tokenBindData );

   // Execute query and get cursor over rows that pass query. Primary key added to output list by default.
   RelationalCollectionCursor& cursor = *dynamic_cast<RelationalCollectionCursor*>(&query->execute());

   long rowCounter = 0;
   while( ! cursor.next() )
   {
      // Get primary key for this row and add it to where clause data.
      m_whereDataForPrimaryKeyInDataTable->begin()->data<unsigned>() = cursor.primaryKey();

      // Loop over all collection fragments.
      for ( std::map< std::string, std::string >::const_iterator iName = m_dataTableNameForCollectionFragmentName.begin();
            iName != m_dataTableNameForCollectionFragmentName.end(); ++iName )
      {
         // Get names of collection fragment, data table and links table.
         std::string fragmentName = iName->first;
         std::string dataTableName = iName->second;
         std::string linksTableName = ( m_linksTableNameForCollectionFragmentName.find( fragmentName ) )->second;

         // Delete row in data table of collection fragment that contains this primary key.
         m_schema->tableHandle( dataTableName ).dataEditor().deleteRows( m_whereClauseForPrimaryKeyInDataTable, 
                                                                        *m_whereDataForPrimaryKeyInDataTable );

         if( updateRecordsCounter ) {
            // Set where data for this collection fragment in collection headers table.
            m_whereDataForCollectionNameInHeadersTable->begin()->data<std::string>() = fragmentName;

            /*
            // Decrement number of records written into collection fragment in collection headers table.
            std::string setClause = pool::RelationalCollection::RelationalCollectionNames::writtenRecordsVariableInCollectionHeadersTable() + " = " + pool::RelationalCollection::RelationalCollectionNames::writtenRecordsVariableInCollectionHeadersTable() + " - 1";
            nominalSchema.tableHandle( pool::RelationalCollection::RelationalCollectionNames::nameOfCollectionHeadersTable() ).dataEditor().updateRows(
               setClause,
               m_whereClauseForCollectionNameInHeadersTable,
               *m_whereDataForCollectionNameInHeadersTable );
               */
            // Increment number of records deleted from collection fragment in collection headers table.
            std::string setClause2 = RelationalCollectionNames::deletedRecordsVariableInCollectionHeadersTable() + " = " + RelationalCollectionNames::deletedRecordsVariableInCollectionHeadersTable() + " + 1";
            m_schema->tableHandle( RelationalCollectionNames::nameOfCollectionHeadersTable() ).dataEditor().updateRows(
               setClause2,
               m_whereClauseForCollectionNameInHeadersTable,
               *m_whereDataForCollectionNameInHeadersTable );
         }

         // Get link Id to Token key maps for possible updates.
         std::map< std::string, unsigned >* linkIdForTokenKey =
            ( m_mapOfLinkIdForTokenKeyMaps.find( fragmentName ) )->second;
         std::map< unsigned, std::string >* tokenKeyForLinkId =
            ( m_mapOfTokenKeyForLinkIdMaps.find( fragmentName ) )->second;

         // Get values of all Tokens in deleted row and check if corresponding rows in links table are still needed.
         for ( pool::TokenList::const_iterator iToken = cursor.currentRow().tokenList().begin(); iToken !=
                  cursor.currentRow().tokenList().end(); ++iToken )
         {
            // Get OID_1 column name.
            std::string collectionColumnName = iToken.tokenName();
            std::string tableColumnPrefix = 
               ( m_tableTokenColumnPrefixForCollectionTokenColumnName.find( collectionColumnName ) )->second;
            std::string oid1ColumnName = tableColumnPrefix +
               RelationalCollectionNames::oid_1_variableInCollectionDataTable();

            // Get OID_1 column value.
            unsigned oid1ColumnValue = iToken->oid().first;

            // Look for OID_1 value in data table.
            bool oid1ColumnFound = false;
            // FIXME - changed here
            for( int i = 0; i < m_description.numberOfTokenColumns( fragmentName ); i++ )
            {
               // Get collection column name.
               std::string collectionColumnName = m_description.tokenColumn( i, fragmentName ).name();

               // Get OID_1 column name.
               std::string prefix = ( m_tableTokenColumnPrefixForCollectionTokenColumnName.find( collectionColumnName ) )->second;
               std::string oid1Name = prefix +  RelationalCollectionNames::oid_1_variableInCollectionDataTable();

               // Look for other Tokens in data table with this OID_1 column value.
               coral::IQuery* oid1Query = m_schema->tableHandle( dataTableName ).newQuery();
               coral::AttributeList* whereDataForOID1InDataTable = ( 
                  m_mapOfWhereDataForOID1InDataTable.find( collectionColumnName ) )->second;
               whereDataForOID1InDataTable->begin()->data<unsigned>() = oid1ColumnValue;
               oid1Query->setCondition( ( m_mapOfWhereClausesForOID1InDataTable.find( collectionColumnName ) )->second, 
                                        *whereDataForOID1InDataTable );
               oid1Query->setRowCacheSize( 1000 );
               coral::ICursor& oid1Cursor = oid1Query->execute();
               if ( oid1Cursor.next() ) {
                  oid1ColumnFound = true;
                  delete oid1Query;
                  break;
               }

               // Cleanup.
               delete oid1Query;
            }

            // If this OID_1 (i.e. link ID) is no longer used delete row from links table and update private maps.
            if( ! oid1ColumnFound )      {
               m_whereDataForLinkIdInLinksTable->begin()->data<unsigned>() = oid1ColumnValue;
               m_schema->tableHandle( linksTableName ).dataEditor().deleteRows( m_whereClauseForLinkIdInLinksTable, *m_whereDataForLinkIdInLinksTable );
               std::string tokenKey = iToken->key();
               linkIdForTokenKey->erase( tokenKey );
               tokenKeyForLinkId->erase( oid1ColumnValue );
            }
         }
      }

// Increment counter of number of rows deleted.
      rowCounter++;
   } 

// Cleanup.
   delete query;

   return rowCounter;
}


