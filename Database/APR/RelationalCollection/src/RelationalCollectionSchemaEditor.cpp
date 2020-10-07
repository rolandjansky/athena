/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RelationalCollectionSchemaEditor.h"
#include "RelationalCollectionNames.h"
#include "RelationalCollectionBindVariables.h"

#include "CollectionBase/ICollectionDescription.h"
#include "CollectionBase/CollectionDescription.h"
#include "CollectionBase/ICollectionIndex.h"
#include "CollectionBase/ICollectionUniqueConstraint.h"
#include "CollectionBase/ICollectionFragment.h"
#include "CollectionBase/CollectionColumn.h"
#include "CollectionBase/TokenList.h"
#include "CollectionBase/CollectionRowBuffer.h"
#include "CollectionBase/CollectionBaseNames.h"

#include "POOLCore/Exception.h"

#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ITableDataEditor.h"
#include "RelationalAccess/ITableSchemaEditor.h"
#include "RelationalAccess/ITablePrivilegeManager.h"
#include "RelationalAccess/TableDescription.h"

#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeSpecification.h"

#include <iostream>
using namespace std;

pool::RelationalCollection::RelationalCollectionSchemaEditor::RelationalCollectionSchemaEditor(
  coral::ISessionProxy& session,
  pool::ICollectionDescription& description,
  std::map< std::string, std::string >& dataTableNameForCollectionFragmentName,
  std::map< std::string, std::string >& linksTableNameForCollectionFragmentName,
  std::map< std::string, std::string >& tableTokenColumnPrefixForCollectionTokenColumnName,
  std::map< std::string, std::string >& tableAttributeColumnNameForCollectionAttributeColumnName,
  std::map< std::string, std::string >& tableColumnNameForCollectionColumnName,
  std::map< int, std::string >& collectionColumnNameForTableColumnPosition,
  coral::AttributeList& tableAttributeList,
  pool::CollectionRowBuffer& collectionRowBuffer,
  std::map< std::string, coral::AttributeList* >& dataTableRowBufferForCollectionFragmentName,
  std::map< std::string, std::string >& mapOfWhereClausesForOID1InDataTable,
  std::map< std::string, coral::AttributeList* >& mapOfWhereDataForOID1InDataTable )
  : m_session( &session ),
    m_description( dynamic_cast<CollectionDescription*>(&description) ),
    m_dataTableNameForCollectionFragmentName( dataTableNameForCollectionFragmentName ),
    m_linksTableNameForCollectionFragmentName( linksTableNameForCollectionFragmentName ),
    m_tableTokenColumnPrefixForCollectionTokenColumnName( tableTokenColumnPrefixForCollectionTokenColumnName ),
    m_tableAttributeColumnNameForCollectionAttributeColumnName( tableAttributeColumnNameForCollectionAttributeColumnName ),
    m_tableColumnNameForCollectionColumnName( tableColumnNameForCollectionColumnName ),
    m_collectionColumnNameForTableColumnPosition( collectionColumnNameForTableColumnPosition ),
    m_tableAttributeList( tableAttributeList ),
    m_collectionRowBuffer( collectionRowBuffer ),
    m_dataTableRowBufferForCollectionFragmentName( dataTableRowBufferForCollectionFragmentName ),
    m_mapOfWhereClausesForOID1InDataTable( mapOfWhereClausesForOID1InDataTable ),
    m_mapOfWhereDataForOID1InDataTable( mapOfWhereDataForOID1InDataTable ),
    m_whereClauseForCollectionNameInHeadersTable( 
      RelationalCollectionBindVariables::whereClauseForCollectionNameInHeadersTable() ),
    m_whereClauseForChildCollectionNameInHeadersTable( 
      RelationalCollectionBindVariables::whereClauseForChildCollectionNameInHeadersTable() ),
    m_whereClauseForCollectionNameInDescriptionsTable( 
      RelationalCollectionBindVariables::whereClauseForCollectionNameInDescriptionsTable() ),
    m_whereClauseForColumnNameInDescriptionsTable( 
      RelationalCollectionBindVariables::whereClauseForColumnNameInDescriptionsTable() ),
    m_whereClauseForCollectionNameInIndexDescriptionsTable(
      RelationalCollectionBindVariables::whereClauseForCollectionNameInIndexDescriptionsTable() ),
    m_whereClauseForIndexNameInIndexDescriptionsTable(
      RelationalCollectionBindVariables::whereClauseForIndexNameInIndexDescriptionsTable() ),
    m_whereClauseForUniqueConstraintNameInIndexDescriptionsTable(
      RelationalCollectionBindVariables::whereClauseForUniqueConstraintNameInIndexDescriptionsTable() ),
    m_whereDataForCollectionNameInHeadersTable( new coral::AttributeList ),
    m_whereDataForChildCollectionNameInHeadersTable( new coral::AttributeList ),
    m_whereDataForCollectionNameInDescriptionsTable( new coral::AttributeList ),
    m_whereDataForColumnNameInDescriptionsTable( new coral::AttributeList ),
    m_whereDataForCollectionNameInIndexDescriptionsTable( new coral::AttributeList ),
    m_whereDataForIndexNameInIndexDescriptionsTable( new coral::AttributeList ),
    m_whereDataForUniqueConstraintNameInIndexDescriptionsTable( new coral::AttributeList )
{
  // Define bind variable type used in where clause to find collection fragment name in collection headers table.
  m_whereDataForCollectionNameInHeadersTable->extend<std::string>(
     RelationalCollectionNames::collectionNameVariableInCollectionHeadersTable() );

  // Define bind variable type used in where clause to find child collection fragment name in collection headers table.
  m_whereDataForChildCollectionNameInHeadersTable->extend<std::string>(
     RelationalCollectionNames::childCollectionNameVariableInCollectionHeadersTable() );

  // Define bind variable type used in where clause to find collection fragment name in collection descriptions table.
  m_whereDataForCollectionNameInDescriptionsTable->extend<std::string>(
     RelationalCollectionNames::collectionNameVariableInCollectionDescriptionsTable() );

  // Define bind variable type used in where clause to find column name in collection descriptions table.
  m_whereDataForColumnNameInDescriptionsTable->extend<std::string>(
    RelationalCollectionNames::collectionVariableNameVariableInCollectionDescriptionsTable() );
  m_whereDataForColumnNameInDescriptionsTable->extend<std::string>(
    RelationalCollectionNames::collectionNameVariableInCollectionDescriptionsTable() );

  // Define bind variable type used in where clause to find collection fragment name in collection index descriptions table.
  m_whereDataForCollectionNameInIndexDescriptionsTable->extend<std::string>(
     RelationalCollectionNames::collectionNameVariableInCollectionIndexDescriptionsTable() );

  // Define bind variable types used in where clause to find index name in collection index descriptions table.
  m_whereDataForIndexNameInIndexDescriptionsTable->extend<std::string>(
     RelationalCollectionNames::indexNameVariableInCollectionIndexDescriptionsTable() );
  m_whereDataForIndexNameInIndexDescriptionsTable->extend<std::string>(
     RelationalCollectionNames::collectionNameVariableInCollectionIndexDescriptionsTable() );

  // Define bind variable types used in where clause to find unique constraint name in collection index descriptions table.
  m_whereDataForUniqueConstraintNameInIndexDescriptionsTable->extend<std::string>(
     RelationalCollectionNames::uniqueConstraintNameVariableInCollectionIndexDescriptionsTable() );
  m_whereDataForUniqueConstraintNameInIndexDescriptionsTable->extend<std::string>(
     RelationalCollectionNames::collectionNameVariableInCollectionIndexDescriptionsTable() );
}


pool::RelationalCollection::RelationalCollectionSchemaEditor::
~RelationalCollectionSchemaEditor()
{
   delete m_whereDataForCollectionNameInHeadersTable;
   delete m_whereDataForChildCollectionNameInHeadersTable;
   delete m_whereDataForCollectionNameInDescriptionsTable;
   delete m_whereDataForColumnNameInDescriptionsTable;
   delete m_whereDataForCollectionNameInIndexDescriptionsTable;
   delete m_whereDataForIndexNameInIndexDescriptionsTable;
   delete m_whereDataForUniqueConstraintNameInIndexDescriptionsTable;
}


void 
pool::RelationalCollection::RelationalCollectionSchemaEditor::
setEventReferenceColumnName( const std::string& columnName )
{
   renameColumn( m_description->eventReferenceColumnName(), columnName );
}



const pool::ICollectionColumn&   
pool::RelationalCollection::RelationalCollectionSchemaEditor::
insertColumn( const std::string& columnName, 
              const std::type_info& columnType,
              const std::string& annotation,
              std::string fragmentName,
              int maxSize,
              bool sizeIsFixed )
{
   return insertColumn( columnName, coral::AttributeSpecification::typeNameForId( columnType ),
                        annotation, fragmentName, maxSize, sizeIsFixed );
}



const pool::ICollectionColumn&   
pool::RelationalCollection::RelationalCollectionSchemaEditor::
insertColumn( const std::string& columnName, const std::string& columnType,
              const std::string& annotation,
              std::string fragmentName, int maxSize, bool sizeIsFixed )
{
   // Check if a Token column is being inserted.
   if( columnType == pool::CollectionBaseNames::tokenTypeName() )  {
      return insertTokenColumn( columnName, annotation, fragmentName );
   }
   
  // If no collection fragment name provided, use top level collection fragment.
  if( fragmentName.size() == 0 )  {
    fragmentName = m_description->name();
  }
  // Insert column into collection description object.
  const ICollectionColumn& column = m_description->insertColumn( columnName, columnType,
                                                                 annotation, fragmentName,
                                                                 maxSize, sizeIsFixed );
  int newPos = m_description->column( columnName ).id();
  
  // Get name of data table to contain new column.
  std::string dataTableName = dataTableNameForCollectionFragmentName( fragmentName, "insertColumn" );

  // Create new table column name.
  std::string tableColumnName = RelationalCollectionNames::variableDataVariableInCollectionDataTable( newPos );
     
  // Insert column into data table.
  m_session->nominalSchema().tableHandle( dataTableName ).schemaEditor().insertColumn( tableColumnName, columnType, maxSize, sizeIsFixed );

  addRowToDescriptionsTable( fragmentName, columnName, annotation, columnType, maxSize,
                             (sizeIsFixed? "true" : "false"), newPos );

  addColumnToMaps( columnName, tableColumnName, columnType, newPos );
 
  recreateDataTableRowBuffer( fragmentName );
  return column;
}



const pool::ICollectionColumn&   
pool::RelationalCollection::RelationalCollectionSchemaEditor::
insertTokenColumn( const std::string& columnName, const std::string& annotation, std::string fragmentName )
{
   const std::string        methodName("insertTokenColumn");
   coral::ISchema&         nominalSchema = m_session->nominalSchema();

  // If no collection fragment name provided use top level collection fragment.
  if( fragmentName.size() == 0 )  {
    fragmentName = m_description->name();
  }

  int newPos = 0;
  const ICollectionColumn* column;
  // Insert column into collection description object.
  if( columnName != m_description->eventReferenceColumnName() )  {
     // event reference  column is in the description by default
     column = &m_description->insertTokenColumn( columnName, annotation, fragmentName );
     // Get new table column position number
     newPos = m_description->column( columnName ).id();
  } else {
     column = &m_description->column( m_description->eventReferenceColumnName() );
  }

  // Get name of data table to contain column.
  const std::string& dataTableName = dataTableNameForCollectionFragmentName( fragmentName, methodName );
 
  // Create new table Token column names.
  std::string variableName = RelationalCollectionNames::variableDataVariableInCollectionDataTable( newPos ) + "_";
  std::string oid1ColumnName = variableName + RelationalCollectionNames::oid_1_variableInCollectionDataTable();
  std::string oid2ColumnName = variableName + RelationalCollectionNames::oid_2_variableInCollectionDataTable();

  // Insert column into data table.
  nominalSchema.tableHandle( dataTableName ).schemaEditor().insertColumn( 
    oid1ColumnName, coral::AttributeSpecification::typeNameForType<int>() );
  nominalSchema.tableHandle( dataTableName ).schemaEditor().insertColumn( 
    oid2ColumnName, coral::AttributeSpecification::typeNameForType<int>() );

  addRowToDescriptionsTable( fragmentName, columnName, annotation, 
                             CollectionBaseNames::tokenTypeName(), 0, "true", newPos );
  addTokenColumnToMaps( variableName, columnName, newPos );
  recreateDataTableRowBuffer( fragmentName );
  return *column;
}



const pool::ICollectionColumn&
pool::RelationalCollection::RelationalCollectionSchemaEditor::
annotateColumn( const std::string& columnName, const std::string& annotation )
{
   const ICollectionColumn& column = m_description->annotateColumn( columnName, annotation );

   // Update corresponding row in collection descriptions table.
   (*m_whereDataForColumnNameInDescriptionsTable)[0].data<std::string>() = columnName;
   (*m_whereDataForColumnNameInDescriptionsTable)[1].data<std::string>() = m_description->collectionFragmentName( columnName );

   std::string setClause = RelationalCollectionNames::collectionVariableAnnotationVariableInCollectionDescriptionsTable() + " = \'" + annotation + "\'"; 
   m_session->nominalSchema().tableHandle( RelationalCollectionNames::nameOfCollectionDescriptionsTable() )
      .dataEditor().updateRows( setClause,
                                m_whereClauseForColumnNameInDescriptionsTable, 
                                *m_whereDataForColumnNameInDescriptionsTable );

   return column;    
}



const std::string&
pool::RelationalCollection::RelationalCollectionSchemaEditor::
dataTableNameForCollectionFragmentName( const std::string& fragmentName, const std::string& method ) const
{
   std::map< std::string, std::string >::const_iterator iName
      = m_dataTableNameForCollectionFragmentName.find( fragmentName );
   if( iName == m_dataTableNameForCollectionFragmentName.end() )  {
      std::string errorMsg = "Cannot not find data table name for collection fragment `" + fragmentName + "'."; 
      throw pool::Exception( errorMsg,
                             "RelationalCollectionSchemaEditor::" + method ,
                             "RelationalCollection" );
   }
   return iName->second; 
}


void 
pool::RelationalCollection::RelationalCollectionSchemaEditor::
recreateDataTableRowBuffer( const std::string& fragmentName )
{
  coral::AttributeList* dataTableRowBuffer = new coral::AttributeList;

  // Add primary key column - only temporary fix?  MN
  dataTableRowBuffer->extend( RelationalCollectionNames::recordIdVariableInCollectionDataTable(), coral::AttributeSpecification::typeNameForType<unsigned>() );
  
  for( int i = 0; i < m_description->numberOfTokenColumns( fragmentName ); i++ )  {
     const std::string& tableColumnPrefix = m_tableTokenColumnPrefixForCollectionTokenColumnName[m_description->tokenColumn( i, fragmentName ).name()];
     addTokenColumnToRowBuffer( dataTableRowBuffer, tableColumnPrefix );
  }
  
  for( int i = 0; i < m_description->numberOfAttributeColumns( fragmentName ); i++ )  {
     const std::string& tableColumnName = m_tableAttributeColumnNameForCollectionAttributeColumnName[m_description->attributeColumn( i, fragmentName ).name()];
    dataTableRowBuffer->extend( tableColumnName, m_description->attributeColumn( i, fragmentName ).type() );
  }

  delete m_dataTableRowBufferForCollectionFragmentName[ fragmentName ];
  m_dataTableRowBufferForCollectionFragmentName[ fragmentName ] = dataTableRowBuffer;
}
   


// Add row for new column into collection descriptions table.
void 
pool::RelationalCollection::RelationalCollectionSchemaEditor::
addRowToDescriptionsTable(const std::string& fragmentName, const std::string& columnName,
                          const std::string& annotation,
                          const std::string& colType, unsigned maxSize,
                          const std::string& isFixed, unsigned newPos )
{
   coral::AttributeList rowBuffer;
   rowBuffer.extend< std::string >( RelationalCollectionNames::collectionNameVariableInCollectionDescriptionsTable() );
   rowBuffer.extend< std::string >( RelationalCollectionNames::collectionVariableNameVariableInCollectionDescriptionsTable() );
   rowBuffer.extend< std::string >( RelationalCollectionNames::collectionVariableTypeVariableInCollectionDescriptionsTable() );
   rowBuffer.extend< int >( RelationalCollectionNames::collectionVariableMaxSizeVariableInCollectionDescriptionsTable() );
   rowBuffer.extend< std::string >( RelationalCollectionNames::collectionVariableSizeIsFixedVariableInCollectionDescriptionsTable() );
   rowBuffer.extend< int >( RelationalCollectionNames::collectionVariablePositionVariableInCollectionDescriptionsTable() );
   rowBuffer[0].data< std::string >() = fragmentName;
   rowBuffer[1].data< std::string >() = columnName;
   rowBuffer[2].data< std::string >() = colType;
   rowBuffer[3].data< int >() = maxSize;
   rowBuffer[4].data< std::string >() = isFixed;
   rowBuffer[5].data< int >() = newPos;

   rowBuffer.extend< std::string >( RelationalCollectionNames::collectionVariableAnnotationVariableInCollectionDescriptionsTable() );
   rowBuffer[6].data< std::string >() = annotation;

   m_session->nominalSchema().tableHandle( RelationalCollectionNames::nameOfCollectionDescriptionsTable() ).dataEditor().insertRow( rowBuffer );  
}




// Update private maps and row buffers.
void
pool::RelationalCollection::RelationalCollectionSchemaEditor::
addColumnToMaps( const std::string& columnName,  const std::string& tableColumnName, 
                 const std::string& columnType, int columnPos )
{
   m_tableAttributeColumnNameForCollectionAttributeColumnName[ columnName ] = tableColumnName;
   m_tableColumnNameForCollectionColumnName[ columnName ] = tableColumnName;
   m_collectionRowBuffer.attributeList().extend( columnName, columnType );
   m_tableAttributeList.extend( tableColumnName, columnType );
   m_collectionColumnNameForTableColumnPosition[ columnPos ] = columnName; 
}




void 
pool::RelationalCollection::RelationalCollectionSchemaEditor::
addTokenColumnToRowBuffer( coral::AttributeList* rowBuffer, const std::string& variableName)
{
   std::string oid1ColumnName = variableName + RelationalCollectionNames::oid_1_variableInCollectionDataTable();
   std::string oid2ColumnName = variableName + RelationalCollectionNames::oid_2_variableInCollectionDataTable();

   rowBuffer->extend( oid1ColumnName, typeid( unsigned ) );
   rowBuffer->extend( oid2ColumnName, typeid( unsigned ) );
}


   
void
pool::RelationalCollection::RelationalCollectionSchemaEditor::
addTokenColumnToMaps( std::string variableName, const std::string& columnName, int columnPos )
{
   // Update private maps and row buffers.
   m_tableTokenColumnPrefixForCollectionTokenColumnName[ columnName ] = variableName;
   m_tableColumnNameForCollectionColumnName[ columnName ] = variableName;
   m_collectionRowBuffer.tokenList().extend( columnName );
   m_collectionColumnNameForTableColumnPosition[ columnPos ] = columnName;
  

   std::string oid1VariableName = variableName + RelationalCollectionNames::oid_1_variableInCollectionDataTable();
   std::string whereClause = oid1VariableName + " =:" + oid1VariableName;
   m_mapOfWhereClausesForOID1InDataTable[ columnName ] = whereClause;

   coral::AttributeList* whereData = new coral::AttributeList;
   whereData->extend<unsigned>( oid1VariableName );
   m_mapOfWhereDataForOID1InDataTable[ columnName ] = whereData;  
}


void 
pool::RelationalCollection::RelationalCollectionSchemaEditor::
dropColumn( const std::string& columnName )
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  // Get column type.
  std::string columnType = m_description->column( columnName ).type();

  // Get name of collection fragment that contains column.
  std::string fragmentName = m_description->column( columnName ).collectionFragmentName();

  // Drop column from collection description object.
  m_description->dropColumn( columnName );

  // Get handle to data table that contains column.
  coral::ITable& table = nominalSchema.tableHandle( dataTableNameForCollectionFragmentName( fragmentName, "dropColumn" ) );

  // Drop column from data table and update private maps and row buffers.
  if( columnType == CollectionBaseNames::tokenTypeName() )  {
    std::string tableColumnPrefix = m_tableTokenColumnPrefixForCollectionTokenColumnName[columnName];
    std::string oid1ColumnName = tableColumnPrefix + RelationalCollectionNames::oid_1_variableInCollectionDataTable();
    std::string oid2ColumnName = tableColumnPrefix + RelationalCollectionNames::oid_2_variableInCollectionDataTable();
    
    table.schemaEditor().dropColumn( oid1ColumnName );
    table.schemaEditor().dropColumn( oid2ColumnName );

    m_tableTokenColumnPrefixForCollectionTokenColumnName.erase( columnName );
 
    m_mapOfWhereClausesForOID1InDataTable.erase( columnName );
    delete m_mapOfWhereDataForOID1InDataTable[ columnName ];
    m_mapOfWhereDataForOID1InDataTable.erase( columnName );
  }
  else {
    std::string tableColumnName = m_tableAttributeColumnNameForCollectionAttributeColumnName[columnName];
    table.schemaEditor().dropColumn( tableColumnName );

    m_tableAttributeColumnNameForCollectionAttributeColumnName.erase( columnName );
  }
  
  m_tableColumnNameForCollectionColumnName.erase( columnName );
  recreateDataTableRowBuffer( fragmentName );
  rebuildAttributeLists();
  
  
  for( std::map< int, std::string >::iterator iData = m_collectionColumnNameForTableColumnPosition.begin();
       iData != m_collectionColumnNameForTableColumnPosition.end(); ++iData )
  {
    if( iData->second == columnName ) {
      m_collectionColumnNameForTableColumnPosition.erase( iData );
      break;
    }
  }

  // Delete corresponding row in collection descriptions table.
  (*m_whereDataForColumnNameInDescriptionsTable)[0].data<std::string>() = columnName;
  (*m_whereDataForColumnNameInDescriptionsTable)[1].data<std::string>() = fragmentName;
  nominalSchema.tableHandle( RelationalCollectionNames::nameOfCollectionDescriptionsTable() ).dataEditor().deleteRows(
      m_whereClauseForColumnNameInDescriptionsTable, *m_whereDataForColumnNameInDescriptionsTable );
}


void 
pool::RelationalCollection::RelationalCollectionSchemaEditor::renameColumn( const std::string& oldName, 
                                                                            const std::string& newName )
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  // Get column type.
  std::string columnType = m_description->column( oldName ).type();

  // Rename column in collection description object.
  m_description->renameColumn( oldName, newName );

  // Update corresponding row in collection descriptions table.
  if ( newName != m_description->eventReferenceColumnName() )  {
    (*m_whereDataForColumnNameInDescriptionsTable)[0].data<std::string>() = oldName;
    // MN: made a fix here which is just a guess!!  (newName)
    (*m_whereDataForColumnNameInDescriptionsTable)[1].data<std::string>() = newName;
    std::string setClause = RelationalCollectionNames::collectionVariableNameVariableInCollectionDescriptionsTable() + " = " + newName; 
    nominalSchema.tableHandle(
      RelationalCollectionNames::nameOfCollectionDescriptionsTable() ).dataEditor().updateRows(
        setClause,
        m_whereClauseForColumnNameInDescriptionsTable, 
        *m_whereDataForColumnNameInDescriptionsTable );
  }

  // Update private maps and row buffers.
  if ( columnType == pool::CollectionBaseNames::tokenTypeName() )  {
    // Corresponding data table column name stays the same.
    std::string tableColumnPrefix = ( m_tableTokenColumnPrefixForCollectionTokenColumnName.find( oldName ) )->second;
    m_tableTokenColumnPrefixForCollectionTokenColumnName.erase( oldName );
    m_tableTokenColumnPrefixForCollectionTokenColumnName.insert( std::make_pair( newName, tableColumnPrefix  ) );
    m_tableColumnNameForCollectionColumnName.erase( oldName );
    m_tableColumnNameForCollectionColumnName.insert( std::make_pair( newName, tableColumnPrefix ) );

    pool::TokenList tokenList;
    for( int i = 0; i < m_description->numberOfCollectionFragments(); i++ )    {
      std::string fname = m_description->collectionFragment( i ).name();
      for( int j = 0; j < m_description->numberOfTokenColumns( fname ); j++ )      {
        tokenList.extend( m_description->tokenColumn( j, fname ).name() );
      }
    }
    m_collectionRowBuffer.setTokenList( tokenList );

    // OID1 column name stays the same.
    std::string whereClause = ( m_mapOfWhereClausesForOID1InDataTable.find( oldName ) )->second;
    m_mapOfWhereClausesForOID1InDataTable.erase( oldName );
    m_mapOfWhereClausesForOID1InDataTable.insert( std::make_pair( newName, whereClause ) );

    // MN: guessing how this should work...  FIX?  validate?
    //delete m_mapOfWhereDataForOID1InDataTable.find( oldName )->second;
    //m_mapOfWhereDataForOID1InDataTable.find( oldName )->second = 0;
    coral::AttributeList *whereData = m_mapOfWhereDataForOID1InDataTable.find( oldName )->second;
    m_mapOfWhereDataForOID1InDataTable.erase( oldName );
    m_mapOfWhereDataForOID1InDataTable.insert( std::make_pair( newName, whereData ) );
  }
  else
  {
    // Corresponding data table column name stays the same.
    std::string tableColumnName = ( m_tableAttributeColumnNameForCollectionAttributeColumnName.find( oldName ) )->second;

    m_tableAttributeColumnNameForCollectionAttributeColumnName.erase( oldName );
    m_tableAttributeColumnNameForCollectionAttributeColumnName.insert( std::make_pair( newName, tableColumnName ) );

    m_tableColumnNameForCollectionColumnName.erase( oldName );
    m_tableColumnNameForCollectionColumnName.insert( std::make_pair( newName, tableColumnName ) );

    coral::AttributeList attributeList;
    for( int i = 0; i < m_description->numberOfCollectionFragments(); i++ )
    {
      std::string fname = m_description->collectionFragment( i ).name();
      for( int j = 0; j < m_description->numberOfAttributeColumns( fname ); j++ )
      {
        attributeList.extend( m_description->attributeColumn( j, fname ).name(),
                              m_description->attributeColumn( j, fname ).type() );
      }
    }
    m_collectionRowBuffer.setAttributeList( attributeList );
  }
  for ( std::map< int, std::string >::iterator iData = m_collectionColumnNameForTableColumnPosition.begin();
          iData != m_collectionColumnNameForTableColumnPosition.end(); ++iData )
  {
    int pos = iData->first;
    if ( iData->second == oldName )
    {
      m_collectionColumnNameForTableColumnPosition.erase( pos );
      m_collectionColumnNameForTableColumnPosition.insert( std::make_pair( pos, newName ) );
      break;
    }
  }
}


void 
pool::RelationalCollection::RelationalCollectionSchemaEditor::changeColumnType( const std::string& columnName, 
                                                                                const std::string& newType,
                                                                                int maxSize,
                                                                                bool sizeIsFixed )
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  // Get old column type.
  std::string oldType = m_description->column( columnName ).type();
  const std::string& annotation = m_description->column( columnName ).annotation();

  // Get name of collection fragment that contains column.
  std::string fragmentName = m_description->column( columnName ).collectionFragmentName();

  // Change column type in collection description object and data table.
  if ( oldType == pool::CollectionBaseNames::tokenTypeName() )  {
     // Drop existing Token column.
     dropColumn( columnName );
     // Insert Attribute column with new type.
     insertColumn( columnName, newType, annotation, fragmentName, maxSize, sizeIsFixed );
  }
  else  {
    if ( newType == pool::CollectionBaseNames::tokenTypeName() ) {
      // Drop existing Attribute column.
      dropColumn( columnName );
      // Insert new Token column.
      insertTokenColumn( columnName, annotation, fragmentName );
    }
    else {
      // Change column type in collection description object.
      m_description->changeColumnType( columnName, newType, maxSize, sizeIsFixed );
      // Get name of data table that contains column.
      std::string dataTableName = "";
      std::map< std::string, std::string >::const_iterator iName = 
        m_dataTableNameForCollectionFragmentName.find( fragmentName );
      if ( iName != m_dataTableNameForCollectionFragmentName.end() ) {
         dataTableName = iName->second;
      }
      else {
        std::string errorMsg = "Cannot not find data table name for collection fragment `" + fragmentName + "'."; 
        throw pool::Exception( errorMsg,
                               "RelationalCollectionSchemaEditor::insertTokenColumn",
                               "RelationalCollection" );
      }

      // Change column type in data table.
      nominalSchema.tableHandle( dataTableName ).schemaEditor().
         changeColumnType( columnName, newType, maxSize,sizeIsFixed );

      // Update corresponding row in collection descriptions table.
      (*m_whereDataForColumnNameInDescriptionsTable)[0].data<std::string>() = columnName;
      (*m_whereDataForColumnNameInDescriptionsTable)[1].data<std::string>() = fragmentName;
      std::string setClause = RelationalCollectionNames::collectionVariableTypeVariableInCollectionDescriptionsTable() + " = " + newType; 
      nominalSchema.tableHandle(
        RelationalCollectionNames::nameOfCollectionDescriptionsTable() ).dataEditor().updateRows(
          setClause,
          m_whereClauseForColumnNameInDescriptionsTable, 
          *m_whereDataForColumnNameInDescriptionsTable );

      // Update private maps and row buffers.
      coral::AttributeList* dataTableRowBuffer = new coral::AttributeList;
      for( int i = 0; i < m_description->numberOfTokenColumns( fragmentName ); i++ ) {
         std::string tableColumnPrefix = m_tableTokenColumnPrefixForCollectionTokenColumnName.find( m_description->tokenColumn( i, fragmentName ).name() )->second;

         addTokenColumnToRowBuffer( dataTableRowBuffer, tableColumnPrefix );
      }
      for( int i = 0; i < m_description->numberOfAttributeColumns( fragmentName ); i++ ) {
        std::string tableColumnName = ( m_tableAttributeColumnNameForCollectionAttributeColumnName.find( m_description->attributeColumn( i, fragmentName ).name() ) )->second;
        dataTableRowBuffer->extend( tableColumnName, m_description->attributeColumn( i, fragmentName ).type() );
      }
      delete m_dataTableRowBufferForCollectionFragmentName.find( fragmentName )->second;
      m_dataTableRowBufferForCollectionFragmentName.find( fragmentName )->second = 0;
      m_dataTableRowBufferForCollectionFragmentName.insert( std::make_pair( fragmentName, dataTableRowBuffer ) );

      coral::AttributeList attributeList;
      coral::AttributeList tableAttributeList;
      for( int i = 0; i < m_description->numberOfCollectionFragments(); i++ ) {
        std::string fname = m_description->collectionFragment( i ).name();
        
        for( int j = 0; j < m_description->numberOfAttributeColumns( fname ); j++ ) {
          std::string cname = m_description->attributeColumn( j, fname ).name();
          std::string ctype = m_description->attributeColumn( j, fname ).type();

          attributeList.extend( cname, ctype );
          tableAttributeList.extend( 
             m_tableAttributeColumnNameForCollectionAttributeColumnName.find( cname )->second, ctype );
        }
      }
      m_collectionRowBuffer.setAttributeList( attributeList );
      m_tableAttributeList = tableAttributeList;
    }
  }
} 



void 
pool::RelationalCollection::RelationalCollectionSchemaEditor::
changeColumnType( const std::string& columnName, const std::type_info& newType,
                  int maxSize, bool sizeIsFixed )
{
   changeColumnType( columnName, coral::AttributeSpecification::typeNameForId( newType ),
                     maxSize, sizeIsFixed );
}



void 
pool::RelationalCollection::RelationalCollectionSchemaEditor::
createIndex( std::string indexName, const std::string& columnName, bool isUnique )
{
   this->createIndex( indexName, std::vector<std::string>( 1, columnName ), isUnique );
}



void 
pool::RelationalCollection::RelationalCollectionSchemaEditor::
createIndex( std::string indexName, const std::vector<std::string>& columnNames, bool isUnique )
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  // Create index in collection description object.
  m_description->createIndex( indexName, columnNames, isUnique );

  // Get newly generated name for index.
  indexName = m_description->index( columnNames ).name();

  // Get data table column names for columns used by this index.
  std::vector< std::string > tableColumnNames;
  for ( std::vector< std::string >::const_iterator iName = columnNames.begin(); iName != columnNames.end(); ++iName )
  {
    std::string collectionColumnName = *iName;
    std::string tableColumnName = ( m_tableColumnNameForCollectionColumnName.find( collectionColumnName ) )->second;

    if ( m_description->column( collectionColumnName ).type() == pool::CollectionBaseNames::tokenTypeName() )
    {
      std::string oid1ColumnName = tableColumnName
         + RelationalCollectionNames::oid_1_variableInCollectionDataTable();
      tableColumnNames.push_back( oid1ColumnName ); 
      std::string oid2ColumnName = tableColumnName
         + RelationalCollectionNames::oid_2_variableInCollectionDataTable();
      tableColumnNames.push_back( oid2ColumnName ); 
    }
    else {
      tableColumnNames.push_back( tableColumnName ); 
    }
  }

  // Add index to data table.
  std::string fragmentName = m_description->collectionFragmentName( *( columnNames.begin() ) );
  std::string dataTableName = ( m_dataTableNameForCollectionFragmentName.find( fragmentName ) )->second;
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
    rowBuffer.extend<std::string>( RelationalCollectionNames::collectionNameVariableInCollectionIndexDescriptionsTable() );
    coral::AttributeList::iterator iAttribute = rowBuffer.begin();
    iAttribute->data< std::string >() = indexName;
    ++iAttribute;
    if ( isUnique )
    {
      iAttribute->data< std::string >() = indexName;
    }
    else
    {
      iAttribute->data< std::string >() = "";
    }
    ++iAttribute;
    iAttribute->data< std::string >() = *iName;
    ++iAttribute;
    iAttribute->data< std::string >() = fragmentName;
    nominalSchema.tableHandle( RelationalCollectionNames::nameOfCollectionIndexDescriptionsTable() ).dataEditor().insertRow( rowBuffer );
  }
}


void 
pool::RelationalCollection::RelationalCollectionSchemaEditor::dropIndex( const std::string& columnName )
{
  this->dropIndex( std::vector<std::string>( 1, columnName ) );
}


void 
pool::RelationalCollection::RelationalCollectionSchemaEditor::dropIndex( const std::vector< std::string >& columnNames )
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  // Get name of index.
  std::string indexName = m_description->index( columnNames ).name();

  // Get name of collection fragment that contains index.
  std::string fragmentName = m_description->collectionFragmentName( *( columnNames.begin() ) );

  // Get name of data table that contains index.
  std::string dataTableName = ( m_dataTableNameForCollectionFragmentName.find( fragmentName ) )->second;

  // Drop index from collection description object.
  m_description->dropIndex( columnNames );

  // Drop index from data table.
  nominalSchema.tableHandle( dataTableName ).schemaEditor().dropIndex( indexName );

  // Delete corresponding rows in collection index descriptions table.
  (*m_whereDataForIndexNameInIndexDescriptionsTable)[0].data<std::string>() = indexName;
  (*m_whereDataForIndexNameInIndexDescriptionsTable)[1].data<std::string>() = fragmentName;
  nominalSchema.tableHandle(
    pool::RelationalCollection::RelationalCollectionNames::nameOfCollectionIndexDescriptionsTable() ).dataEditor().deleteRows( m_whereClauseForIndexNameInIndexDescriptionsTable, *m_whereDataForIndexNameInIndexDescriptionsTable );
}


void
pool::RelationalCollection::RelationalCollectionSchemaEditor::
setUniqueConstraint( const std::string& constraintName, const std::string& columnName )
{
   setUniqueConstraint( constraintName, std::vector<std::string>( 1, columnName ) );
}


void
pool::RelationalCollection::RelationalCollectionSchemaEditor::
setUniqueConstraint( const std::string& constraintNameIn, const std::vector< std::string >& columnNames )
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  // Set unique constraint in collection description object.
  m_description->setUniqueConstraint( constraintNameIn, columnNames );

  // Get newly generated name for unique constraint.
  std::string constraintName = m_description->uniqueConstraint( columnNames ).name();

  // Get data table column names for columns used by this unique constraint.
  std::vector< std::string > tableColumnNames;
  for ( std::vector< std::string >::const_iterator iName = columnNames.begin(); iName != columnNames.end(); ++iName )
  {
    std::string collectionColumnName = *iName;
    std::string tableColumnName = ( m_tableColumnNameForCollectionColumnName.find( collectionColumnName ) )->second;

    if ( m_description->column( collectionColumnName ).type() == pool::CollectionBaseNames::tokenTypeName() )
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

  // Set unique constraint in data table.
  std::string fragmentName = m_description->collectionFragmentName( *( columnNames.begin() ) );
  std::string dataTableName = ( m_dataTableNameForCollectionFragmentName.find( fragmentName ) )->second;
  nominalSchema.tableHandle( dataTableName ).schemaEditor().setUniqueConstraint( tableColumnNames,
                                                                                 constraintName,
                                                                                 true );

  // Insert rows for new index into collection index and unique constraint descriptions table.
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


void
pool::RelationalCollection::RelationalCollectionSchemaEditor::unsetUniqueConstraint( const std::string& columnName )
{
  this->unsetUniqueConstraint( std::vector<std::string>( 1, columnName ) );
}


void
pool::RelationalCollection::RelationalCollectionSchemaEditor::unsetUniqueConstraint(
      const std::vector< std::string >& columnNames )
{
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  // Get name of unique constraint.
  std::string constraintName = m_description->uniqueConstraint( columnNames ).name();

  // Get name of collection fragment that contains unique constraint.
  std::string fragmentName = m_description->collectionFragmentName( *columnNames.begin() );

  // Get name of data table that contains unique constraint.
  std::string dataTableName = ( m_dataTableNameForCollectionFragmentName.find( fragmentName ) )->second;

  // Drop unique constraint from collection description object.
  m_description->unsetUniqueConstraint( columnNames );

  // Drop unique constraint from data table.
  nominalSchema.tableHandle( dataTableName ).schemaEditor().
     setUniqueConstraint( columnNames, constraintName, false );

  // Delete corresponding rows in collection index and unique constraint descriptions table.
  (*m_whereDataForUniqueConstraintNameInIndexDescriptionsTable)[0].data<std::string>() = constraintName;
  (*m_whereDataForUniqueConstraintNameInIndexDescriptionsTable)[1].data<std::string>() = fragmentName;
  nominalSchema.tableHandle( pool::RelationalCollection::RelationalCollectionNames::nameOfCollectionIndexDescriptionsTable() ).dataEditor().deleteRows( m_whereClauseForUniqueConstraintNameInIndexDescriptionsTable, *m_whereDataForUniqueConstraintNameInIndexDescriptionsTable );
}



void 
pool::RelationalCollection::RelationalCollectionSchemaEditor::
addCollectionFragment( const std::string& fragmentName,
                       std::string parentFragmentName,
                       bool usesForeignKey )
{
   cout << " *DEBUG*  RelationalCollectionSchemaEditor::addCollectionFragment() called" << endl;
   
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  // Check if parent collection fragment is defined in the collection description.
  if ( m_dataTableNameForCollectionFragmentName.find( parentFragmentName ) ==
       m_dataTableNameForCollectionFragmentName.end() )
  {
    std::string errorMsg = "Collection fragment with name `" + parentFragmentName + 
      "' is not defined in the collection description.";
    throw pool::Exception( errorMsg,
                           "RelationalCollectionSchemaEditor::addCollectionFragment",
                           "RelationalCollection" );
  }

  // Drop foreign key constraint on parent collection fragment if constraint exists.
  std::string oldForeignKeyName = m_description->collectionFragment( parentFragmentName ).foreignKeyName();
  if ( oldForeignKeyName.size() > 0 )
  {
    std::string oldParentChildFragmentName = 
      m_description->collectionFragment( parentFragmentName ).childCollectionFragmentName();
    std::string oldParentChildDataTableName =
       //MN: FIXed here, again a guess
       m_dataTableNameForCollectionFragmentName.find( oldParentChildFragmentName )->second;
    nominalSchema.tableHandle( oldParentChildDataTableName ).schemaEditor().dropForeignKey( oldForeignKeyName );
  }

  // Add collection fragment to collection description object.
  m_description->addCollectionFragment( fragmentName, parentFragmentName, usesForeignKey );

  // Get updated collection fragment properties.
  std::string childFragmentName = m_description->collectionFragment( fragmentName ).childCollectionFragmentName();
  std::string foreignKeyName = m_description->collectionFragment( fragmentName ).foreignKeyName();
  std::string parentForeignKeyName = m_description->collectionFragment( parentFragmentName ).foreignKeyName();

  // Update parent collection fragment row in collection headers table.
  if( parentFragmentName.size() > 0 )  {
    m_whereDataForCollectionNameInHeadersTable->begin()->data<std::string>() = parentFragmentName;
    std::string setClause = RelationalCollectionNames::childCollectionNameVariableInCollectionHeadersTable() + " = " + fragmentName + " , " + RelationalCollectionNames::foreignKeyNameVariableInCollectionHeadersTable() + " = " + parentForeignKeyName;
    nominalSchema.tableHandle(
       RelationalCollectionNames::nameOfCollectionHeadersTable() ).dataEditor().updateRows(
        setClause,
        m_whereClauseForCollectionNameInHeadersTable, 
        *m_whereDataForCollectionNameInHeadersTable );
  }

  // Check if collection fragment already exists in database.
  coral::IQuery* query = nominalSchema.tableHandle( 
     RelationalCollectionNames::nameOfCollectionHeadersTable() ).newQuery();
  m_whereDataForCollectionNameInHeadersTable->begin()->data<std::string>() = fragmentName;
  query->setCondition( m_whereClauseForCollectionNameInHeadersTable, *m_whereDataForCollectionNameInHeadersTable );
  query->limitReturnedRows( 1, 0 );
  coral::ICursor& cursor = query->execute();
  bool exists = cursor.next();
  delete query;

  // If collection fragment does not exist create new data and links tables and update headers table.
  std::string dataTableName = "";
  std::string linksTableName = "";
  if ( ! exists )
  {
    // Find next available names for data and links tables of collection fragment.
    for ( int i = 0; ; ++i ) 
    {
      dataTableName = pool::RelationalCollection::RelationalCollectionNames::nameOfCollectionDataTable( i );
      linksTableName = pool::RelationalCollection::RelationalCollectionNames::nameOfCollectionLinksTable( i );
      if ( ! ( nominalSchema.existsTable( dataTableName ) ||
               nominalSchema.existsTable( linksTableName ) ) ) break;
    }

    // Insert new data and links table names in private maps.
    m_dataTableNameForCollectionFragmentName.insert( std::make_pair( fragmentName, dataTableName ) );
    m_linksTableNameForCollectionFragmentName.insert( std::make_pair( fragmentName, linksTableName ) );

    // Create table description for data table.
    coral::TableDescription dataTableDescription(
      pool::RelationalCollection::RelationalCollectionNames::collectionTypeName() );
    dataTableDescription.setName( dataTableName );

    // Add primary key column to data table.
    dataTableDescription.insertColumn( 
      pool::RelationalCollection::RelationalCollectionNames::recordIdVariableInCollectionDataTable(),
      coral::AttributeSpecification::typeNameForType<unsigned>() );

    // Set primary key of data table.
    dataTableDescription.setPrimaryKey( 
      pool::RelationalCollection::RelationalCollectionNames::recordIdVariableInCollectionDataTable() );

    // Create data table of collection fragment and define user privileges.
    nominalSchema.createTable( dataTableDescription ); //.privilegeManager().grantToPublic( coral::ITablePrivilegeManager::Select );

    // Create description object for links table of collection fragment.
    coral::TableDescription linksTableDescription( 
      pool::RelationalCollection::RelationalCollectionNames::collectionTypeName() );
    linksTableDescription.setName( linksTableName );

    // Add link table ID and Token key information to links table schema:
    linksTableDescription.insertColumn( 
      pool::RelationalCollection::RelationalCollectionNames::linkIdVariableInCollectionLinksTable(),
      coral::AttributeSpecification::typeNameForType<unsigned>() );
    linksTableDescription.insertColumn( 
      pool::RelationalCollection::RelationalCollectionNames::databaseIdVariableInCollectionLinksTable(),
      coral::AttributeSpecification::typeNameForType<std::string>() );
    linksTableDescription.insertColumn( 
      pool::RelationalCollection::RelationalCollectionNames::containerIdVariableInCollectionLinksTable(),
      coral::AttributeSpecification::typeNameForType<std::string>() );
    linksTableDescription.insertColumn( 
      pool::RelationalCollection::RelationalCollectionNames::classIdVariableInCollectionLinksTable(),
      coral::AttributeSpecification::typeNameForType<std::string>() );
    linksTableDescription.insertColumn( 
      pool::RelationalCollection::RelationalCollectionNames::technologyIdVariableInCollectionLinksTable(),
      coral::AttributeSpecification::typeNameForType<unsigned>() );

    // Set primary key for links table.
    linksTableDescription.setPrimaryKey( 
      pool::RelationalCollection::RelationalCollectionNames::linkIdVariableInCollectionLinksTable() );

    // Create links table for new collection fragment and define user privileges.
    nominalSchema.createTable( linksTableDescription ); //.privilegeManager().grantToPublic( coral::ITablePrivilegeManager::Select );

    // Add row for new collection fragment into collection headers table.
    coral::AttributeList headersRowBuffer;
    headersRowBuffer.extend<std::string>( 
       RelationalCollectionNames::collectionNameVariableInCollectionHeadersTable() );
    headersRowBuffer.extend<std::string>( 
       RelationalCollectionNames::dataTableNameVariableInCollectionHeadersTable() );
    headersRowBuffer.extend<std::string>( 
       RelationalCollectionNames::linksTableNameVariableInCollectionHeadersTable() );
    headersRowBuffer.extend<unsigned>( 
       RelationalCollectionNames::writtenRecordsVariableInCollectionHeadersTable() );
    headersRowBuffer.extend<unsigned>( 
       RelationalCollectionNames::deletedRecordsVariableInCollectionHeadersTable() );
    headersRowBuffer.extend<std::string>( 
       RelationalCollectionNames::childCollectionNameVariableInCollectionHeadersTable() );
    headersRowBuffer.extend<std::string>( 
       RelationalCollectionNames::foreignKeyNameVariableInCollectionHeadersTable() );
    coral::AttributeList::iterator iAttribute = headersRowBuffer.begin();
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
    iAttribute->data< std::string >() = childFragmentName;
    ++iAttribute;
    iAttribute->data< std::string >() = foreignKeyName;
    nominalSchema.tableHandle( RelationalCollectionNames::nameOfCollectionHeadersTable() ).dataEditor().insertRow( headersRowBuffer );

    // Lock new row of collection headers table.
    coral::IQuery* query = nominalSchema.tableHandle( 
       RelationalCollectionNames::nameOfCollectionHeadersTable() ).newQuery();
    m_whereDataForCollectionNameInHeadersTable->begin()->data<std::string>() = fragmentName;
    query->setForUpdate();
    query->setCondition( m_whereClauseForCollectionNameInHeadersTable, *m_whereDataForCollectionNameInHeadersTable );
    query->limitReturnedRows( 1, 0 );
    query->execute();
    delete query;
  }
  else  {
    // Get name of data table for existing collection fragment.
    dataTableName = m_dataTableNameForCollectionFragmentName.find( fragmentName )->second;

    // Update child collection fragment and foreign key names of existing collection fragment.
    m_whereDataForCollectionNameInHeadersTable->begin()->data<std::string>() = fragmentName;
    std::string setClause = RelationalCollectionNames::childCollectionNameVariableInCollectionHeadersTable() + " = " + childFragmentName + " , " + RelationalCollectionNames::foreignKeyNameVariableInCollectionHeadersTable() + " = " + foreignKeyName;
    nominalSchema.tableHandle( RelationalCollectionNames::nameOfCollectionHeadersTable() ).dataEditor().updateRows(
        setClause,
        m_whereClauseForCollectionNameInHeadersTable, 
        *m_whereDataForCollectionNameInHeadersTable );
  }

  // Create foreign key constraint on parent collection fragment if specified in new collection description.
  if( parentForeignKeyName.size() > 0 )  {
    std::string parentDataTableName = ( m_dataTableNameForCollectionFragmentName.find( parentFragmentName ) )->second;
    nominalSchema.tableHandle( dataTableName ).schemaEditor().createForeignKey( 
      parentForeignKeyName,
      RelationalCollectionNames::recordIdVariableInCollectionDataTable(),
      parentDataTableName,
      RelationalCollectionNames::recordIdVariableInCollectionDataTable() );
  }

  // Create foreign key constraint on new collection fragment if specified in new collection description.
  if ( foreignKeyName.size() > 0 )  {
    std::string childDataTableName = ( m_dataTableNameForCollectionFragmentName.find( childFragmentName ) )->second;
    nominalSchema.tableHandle( childDataTableName ).schemaEditor().createForeignKey( 
      foreignKeyName, RelationalCollectionNames::recordIdVariableInCollectionDataTable(),
      dataTableName, RelationalCollectionNames::recordIdVariableInCollectionDataTable() );
  }

  // Update private maps in case collection fragment is associated with a pre-existing data table.
  const pool::ICollectionFragment& fragment = m_description->collectionFragment( fragmentName );
  coral::AttributeList* dataTableRowBuffer = new coral::AttributeList;
  int        tokPos = 0;
  for (pool::CollectionColumn* column : fragment.tokenColumns())
  {
     std::string collectionColumnName = column->name();
     std::string variableName = RelationalCollectionNames::variableDataVariableInCollectionDataTable( tokPos );
     std::string tableColumnPrefix = dataTableName + "." + variableName + "_";
     //std::cout << " >>>> Adding Token Column prefix: " << collectionColumnName
     //          << ", " << tableColumnPrefix  << std::endl;
     m_tableTokenColumnPrefixForCollectionTokenColumnName.insert( std::make_pair( collectionColumnName, tableColumnPrefix ) );
     m_tableColumnNameForCollectionColumnName.insert( std::make_pair( collectionColumnName, tableColumnPrefix ) );
     m_collectionColumnNameForTableColumnPosition[ tokPos ] = collectionColumnName; 
     addTokenColumnToRowBuffer( dataTableRowBuffer, tableColumnPrefix );
     
     std::string oid1VariableName = variableName + "_" + RelationalCollectionNames::oid_1_variableInCollectionDataTable();
     std::string whereClause = oid1VariableName + " =:" + oid1VariableName;
     m_mapOfWhereClausesForOID1InDataTable.insert( std::make_pair( collectionColumnName, whereClause ) );
     coral::AttributeList* whereData = new coral::AttributeList;
     whereData->extend<unsigned>( oid1VariableName );
     m_mapOfWhereDataForOID1InDataTable.insert( std::make_pair( collectionColumnName, whereData ) );
     tokPos++;
  }
  int        attrPos = 0;
  for (pool::CollectionColumn* column : fragment.attributeColumns())
  {
    std::string collectionColumnName = column->name();
    std::string columnType = column->type();
    std::string tableColumnName = dataTableName + "." + RelationalCollectionNames::variableDataVariableInCollectionDataTable( attrPos );
    m_tableAttributeColumnNameForCollectionAttributeColumnName.insert( std::make_pair( collectionColumnName, 
                                                                                       tableColumnName ) );
    m_tableColumnNameForCollectionColumnName.insert( std::make_pair( collectionColumnName, tableColumnName ) );
    m_collectionColumnNameForTableColumnPosition[ attrPos ] = collectionColumnName;
    dataTableRowBuffer->extend( tableColumnName, columnType );
    attrPos++;
  }
  m_dataTableRowBufferForCollectionFragmentName.insert( std::make_pair( fragmentName, dataTableRowBuffer ) );

  rebuildAttributeLists();
}



void 
pool::RelationalCollection::RelationalCollectionSchemaEditor::
rebuildAttributeLists( )
{
  // Rebuild private row buffers.
  pool::TokenList tokenList;
  coral::AttributeList attributeList;
  coral::AttributeList tableAttributeList;
  for( int i = 0; i < m_description->numberOfCollectionFragments(); i++ )
  {
    std::string fname = m_description->collectionFragment( i ).name();
    for( int j = 0; j < m_description->numberOfTokenColumns( fname ); j++ ) {
      tokenList.extend( m_description->tokenColumn( j, fname ).name() );
    }
    for( int j = 0; j < m_description->numberOfAttributeColumns( fname ); j++ )
    {
      std::string collectionColumnName = m_description->attributeColumn( j, fname ).name();
      // MN: adding columnType - FIX? validate?
      std::string columnType =  m_description->attributeColumn( j, fname ).type();
      attributeList.extend( collectionColumnName, columnType );
      std::string tableColumnName = m_tableAttributeColumnNameForCollectionAttributeColumnName.find( collectionColumnName )->second;
      tableAttributeList.extend( tableColumnName, columnType);
    }
  }

  m_collectionRowBuffer.setTokenList( tokenList );
  m_collectionRowBuffer.setAttributeList( attributeList );
  m_tableAttributeList = tableAttributeList;
}


void 
pool::RelationalCollection::RelationalCollectionSchemaEditor::
dropCollectionFragment( const std::string& fragmentName )
{

   // MN:  This method was not tested and will for sure crash
   
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  // Get names of parent and child collection fragments and child fragment data table.
  std::string parentFragmentName = m_description->collectionFragment( fragmentName ).parentCollectionFragmentName();
  std::string childFragmentName = m_description->collectionFragment( fragmentName ).childCollectionFragmentName();
  std::string childDataTableName;

  // Drop foreign key constraint on parent collection fragment if constraint exists.
  if( parentFragmentName.size() )  {
     std::string oldParentForeignKeyName = m_description->collectionFragment( parentFragmentName ).foreignKeyName();
     if ( oldParentForeignKeyName.size() > 0 )    {
        std::string dataTableName = m_dataTableNameForCollectionFragmentName.find( fragmentName )->second;
        nominalSchema.tableHandle( dataTableName ).schemaEditor().dropForeignKey( oldParentForeignKeyName );
     }
  }

  // Drop foreign key constraint on collection fragment if constraint exists.
  if( childFragmentName.size() )  {
     childDataTableName = m_dataTableNameForCollectionFragmentName.find( childFragmentName )->second;
     string foreignKeyName = m_description->collectionFragment( fragmentName ).foreignKeyName();
     if( foreignKeyName.size() ) {
        nominalSchema.tableHandle( childDataTableName ).schemaEditor().dropForeignKey( foreignKeyName );
    }
  }

  // Drop collection fragment from collection description object.
  m_description->dropCollectionFragment( fragmentName );

  // Create new foreign key constraint on parent collection fragment from child collection fragment if constraint specified.
  std::string parentForeignKeyName = m_description->collectionFragment( parentFragmentName ).foreignKeyName();
  if ( parentForeignKeyName.size() > 0 )
  {
    std::string parentDataTableName = ( m_dataTableNameForCollectionFragmentName.find( parentFragmentName ) )->second;
    nominalSchema.tableHandle( childDataTableName ).schemaEditor().createForeignKey( 
      parentForeignKeyName,
      pool::RelationalCollection::RelationalCollectionNames::recordIdVariableInCollectionDataTable(),
      parentDataTableName,
      pool::RelationalCollection::RelationalCollectionNames::recordIdVariableInCollectionDataTable() );
  }

  // Update parent collection fragment row in collection headers table.
  if( parentFragmentName.size() > 0 )  {
    m_whereDataForCollectionNameInHeadersTable->begin()->data<std::string>() = parentFragmentName;
    std::string setClause = RelationalCollectionNames::childCollectionNameVariableInCollectionHeadersTable() + " = " + childFragmentName + " , " + RelationalCollectionNames::foreignKeyNameVariableInCollectionHeadersTable() + " = " + parentForeignKeyName;
    nominalSchema.tableHandle(
       RelationalCollectionNames::nameOfCollectionHeadersTable() ).dataEditor().updateRows(
        setClause,
        m_whereClauseForCollectionNameInHeadersTable,
        *m_whereDataForCollectionNameInHeadersTable );
  }

  // Update collection fragment row in collection headers table.
  if( parentFragmentName.size() > 0 )  {
    m_whereDataForCollectionNameInHeadersTable->begin()->data<std::string>() = fragmentName;
    std::string setClause = RelationalCollectionNames::childCollectionNameVariableInCollectionHeadersTable() + " = " + "\'\'" + " , " + RelationalCollectionNames::foreignKeyNameVariableInCollectionHeadersTable() + " = " + "\'\'";
    nominalSchema.tableHandle(
       RelationalCollectionNames::nameOfCollectionDescriptionsTable() ).dataEditor().updateRows(
        setClause,
        m_whereClauseForCollectionNameInHeadersTable, 
        *m_whereDataForCollectionNameInHeadersTable );
  }

  // Drop collection collection fragment from private maps.
  m_dataTableNameForCollectionFragmentName.erase( fragmentName );
  m_linksTableNameForCollectionFragmentName.erase( fragmentName );
  delete ( m_dataTableRowBufferForCollectionFragmentName.find( fragmentName ) )->second;
  ( m_dataTableRowBufferForCollectionFragmentName.find( fragmentName ) )->second = 0;
  m_dataTableRowBufferForCollectionFragmentName.erase( fragmentName );
  for( int i = 0; i < m_description->numberOfTokenColumns( fragmentName ); i++ )
  {
    std::string collectionColumnName = m_description->tokenColumn( i, fragmentName ).name();
    m_tableTokenColumnPrefixForCollectionTokenColumnName.erase( collectionColumnName );
    m_tableColumnNameForCollectionColumnName.erase( collectionColumnName );
    for ( std::map< int, std::string >::iterator iData = m_collectionColumnNameForTableColumnPosition.begin();
          iData != m_collectionColumnNameForTableColumnPosition.end(); ++iData )
    {
      if ( iData->second == collectionColumnName ) {
        m_collectionColumnNameForTableColumnPosition.erase( iData->first );
        break;
      }
    }
    m_mapOfWhereClausesForOID1InDataTable.erase( collectionColumnName );
    delete ( m_mapOfWhereDataForOID1InDataTable.find( collectionColumnName ) )->second;
    ( m_mapOfWhereDataForOID1InDataTable.find( collectionColumnName ) )->second = 0;
    m_mapOfWhereDataForOID1InDataTable.erase( collectionColumnName );
  }
  for( int i = 0; i < m_description->numberOfAttributeColumns( fragmentName ); i++ )  {
    std::string collectionColumnName = m_description->attributeColumn( i, fragmentName ).name();
    m_tableAttributeColumnNameForCollectionAttributeColumnName.erase( collectionColumnName );
    m_tableColumnNameForCollectionColumnName.erase( collectionColumnName );
    for ( std::map< int, std::string >::iterator iData = m_collectionColumnNameForTableColumnPosition.begin();
          iData != m_collectionColumnNameForTableColumnPosition.end(); ++iData )
    {
      if ( iData->second == collectionColumnName ) {
        m_collectionColumnNameForTableColumnPosition.erase( iData->first );
        break;
      }
    }
  }

  rebuildAttributeLists();
}


void 
pool::RelationalCollection::RelationalCollectionSchemaEditor::
renameCollectionFragment( const std::string& oldName, const std::string& newName )
{
  // Rename collection fragment in collection description object.
  m_description->renameCollectionFragment( oldName, newName );
  coral::ISchema& nominalSchema = m_session->nominalSchema();

  // Change collection fragment name in collection headers table.
  m_whereDataForCollectionNameInHeadersTable->begin()->data<std::string>() = oldName;
  std::string setClause = RelationalCollectionNames::collectionNameVariableInCollectionHeadersTable() + " = " + newName;
  nominalSchema.tableHandle(
    RelationalCollectionNames::nameOfCollectionHeadersTable() ).dataEditor().updateRows(
      setClause,
      m_whereClauseForCollectionNameInHeadersTable, 
      *m_whereDataForCollectionNameInHeadersTable );

  // Change child collection fragment name in collection headers table.
  m_whereDataForChildCollectionNameInHeadersTable->begin()->data<std::string>() = oldName;
  setClause = RelationalCollectionNames::childCollectionNameVariableInCollectionHeadersTable() + " = " + newName;
  nominalSchema.tableHandle(
     RelationalCollectionNames::nameOfCollectionHeadersTable() ).dataEditor().updateRows(
      setClause,
      m_whereClauseForChildCollectionNameInHeadersTable, 
      *m_whereDataForChildCollectionNameInHeadersTable );

  // Change collection fragment name in collection descriptions table.
  m_whereDataForCollectionNameInDescriptionsTable->begin()->data<std::string>() = oldName;
  setClause = RelationalCollectionNames::collectionNameVariableInCollectionDescriptionsTable() + " = " + newName;
  nominalSchema.tableHandle(
    RelationalCollectionNames::nameOfCollectionDescriptionsTable() ).dataEditor().updateRows(
      setClause,
      m_whereClauseForCollectionNameInDescriptionsTable, 
      *m_whereDataForCollectionNameInDescriptionsTable );

  // Change collection fragment name and associated index in index and unique constraint descriptions table.
  m_whereDataForCollectionNameInIndexDescriptionsTable->begin()->data<std::string>() = oldName;
  for( int i = 0; i < m_description->numberOfIndices(); i++ )
  {
     const pool::ICollectionIndex& index = m_description->index( i );
    std::vector< std::string > columnNames = index.columnNames();
    if ( m_description->collectionFragmentName( *( columnNames.begin() ) ) == newName )
    {
      // Note: Index and unique constraint names need to be reset because they were changed by collection description update.
      std::string indexName = index.name();
      std::string uniqueConstraintName = "\'\'";
      if ( index.isUnique() )
      {
        uniqueConstraintName = indexName;
      }
      std::string setClause = RelationalCollectionNames::indexNameVariableInCollectionIndexDescriptionsTable() + " = " + indexName + " , " + RelationalCollectionNames::uniqueConstraintNameVariableInCollectionIndexDescriptionsTable() + " = " + uniqueConstraintName + " , " +  RelationalCollectionNames::collectionNameVariableInCollectionIndexDescriptionsTable() + " = " + newName;
      nominalSchema.tableHandle( RelationalCollectionNames::nameOfCollectionIndexDescriptionsTable() ).dataEditor().updateRows(
        setClause,
        m_whereClauseForCollectionNameInIndexDescriptionsTable, 
        *m_whereDataForCollectionNameInIndexDescriptionsTable );
    }
  }

  // Change collection fragment name and associated unique constraint in index and unique constraint descriptions table.
  m_whereDataForCollectionNameInIndexDescriptionsTable->begin()->data<std::string>() = oldName;
  for( int i = 0; i < m_description->numberOfUniqueConstraints(); i++ )
  {
     const pool::ICollectionUniqueConstraint& constraint = m_description->uniqueConstraint( i );
    std::vector< std::string > columnNames = constraint.columnNames();
    if ( m_description->collectionFragmentName( *( columnNames.begin() ) ) == newName )
    {
      // Note: Unique constraint names need to be reset because they were changed by collection description update.
      std::string constraintName = constraint.name();
      std::string setClause = RelationalCollectionNames::uniqueConstraintNameVariableInCollectionIndexDescriptionsTable() + " = " + constraintName + " , " +  RelationalCollectionNames::collectionNameVariableInCollectionIndexDescriptionsTable() + " = " + newName;
      nominalSchema.tableHandle( RelationalCollectionNames::nameOfCollectionIndexDescriptionsTable() ).dataEditor().updateRows(
        setClause,
        m_whereClauseForCollectionNameInIndexDescriptionsTable, 
        *m_whereDataForCollectionNameInIndexDescriptionsTable );
    }
  }

  // Update private maps.
  std::string dataTableName = (  m_dataTableNameForCollectionFragmentName.find( oldName ) )->second;
  m_dataTableNameForCollectionFragmentName.erase( oldName );
  m_dataTableNameForCollectionFragmentName.insert( std::make_pair( newName, dataTableName ) );
  std::string linksTableName = (  m_linksTableNameForCollectionFragmentName.find( oldName ) )->second;
  m_linksTableNameForCollectionFragmentName.erase( oldName );
  m_linksTableNameForCollectionFragmentName.insert( std::make_pair( newName, linksTableName ) );
}
