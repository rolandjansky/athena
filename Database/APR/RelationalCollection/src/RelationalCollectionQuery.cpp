/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RelationalCollectionQuery.h"
#include "RelationalCollectionNames.h"
#include "RelationalCollectionExpressionParser.h"
#include "RelationalCollectionCursor.h"

#include "CollectionBase/ICollectionDescription.h"
#include "CollectionBase/TokenList.h"
#include "CollectionBase/CollectionRowBuffer.h"
#include "CollectionBase/ICollectionCursor.h"
#include "CollectionBase/ICollectionColumn.h"
#include "CollectionBase/ICollectionFragment.h"
#include "CollectionBase/CollectionBaseNames.h"
#include "CollectionBase/boost_tokenizer_headers.h"

#include "POOLCore/Exception.h"

#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"

#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/ISchema.h"

#include <iostream>
using namespace std;

pool::RelationalCollection::RelationalCollectionQuery::RelationalCollectionQuery(
   coral::ISessionProxy& session,
   const pool::ICollectionDescription& description,
   const std::map< std::string, std::string >& dataTableNameForCollectionFragmentName,
   const std::map< std::string, std::string >& linksTableNameForCollectionFragmentName,
   const std::map< std::string, std::string >& tableTokenColumnPrefixForCollectionTokenColumnName,
   const std::map< std::string, std::string >& tableAttributeColumnNameForCollectionAttributeColumnName,
   const coral::AttributeList& tableAttributeList,
   const std::map< std::string, std::map< std::string, unsigned >* >& mapOfLinkIdForTokenKeyMaps,
   const std::map< std::string, std::map< unsigned, std::string >* >& mapOfTokenKeyForLinkIdMaps,
   bool readPrimaryKey )
      : m_description( description ),
	m_dataTableNameForCollectionFragmentName( dataTableNameForCollectionFragmentName ),
	m_linksTableNameForCollectionFragmentName( linksTableNameForCollectionFragmentName ),
	m_tableTokenColumnPrefixForCollectionTokenColumnName( tableTokenColumnPrefixForCollectionTokenColumnName ),
	m_tableAttributeColumnNameForCollectionAttributeColumnName( tableAttributeColumnNameForCollectionAttributeColumnName ),
	m_tableAttributeList( tableAttributeList ),
	m_mapOfLinkIdForTokenKeyMaps( mapOfLinkIdForTokenKeyMaps ),
	m_mapOfTokenKeyForLinkIdMaps( mapOfTokenKeyForLinkIdMaps ),
	m_query( 0 ),
	m_outputDataBuffer( 0 ),
	m_whereClauseBindData( new coral::AttributeList ),
	m_outputTokenList( new pool::TokenList ),
	m_outputAttributeList( new coral::AttributeList ),
	m_cursor( 0 ),
	m_session( session ),
	m_rowCacheSize( 0 ),
	m_readPrimaryKey( readPrimaryKey )
{
   // by default always include primary Event Ref in the query
   m_skipEventRef = false;
   // Get name of data table for top level collection fragment.
   m_dataTableName = m_dataTableNameForCollectionFragmentName.find( m_description.name() )->second;
}


pool::RelationalCollection::RelationalCollectionQuery::~RelationalCollectionQuery()
{
   delete m_query;
   delete m_outputDataBuffer;
   delete m_whereClauseBindData;
   delete m_outputTokenList;
   delete m_outputAttributeList;
   delete m_cursor;
}


   
void
pool::RelationalCollection::RelationalCollectionQuery::addToOutputList( const std::string& columnNames )
{
   typedef boost::tokenizer<boost::char_separator<char> > Tizer;
   boost::char_separator<char> sep(" ,");
   Tizer tizer( columnNames, sep );

   for( Tizer::iterator token = tizer.begin(); token != tizer.end(); ++token ) { 
      if( *token == "*" ) {
         selectAll();
         return;
      }
      // Check if is a Token column.
      if( m_description.column( *token ).type() == pool::CollectionBaseNames::tokenTypeName() ) {
         addToTokenOutputList( *token );
      } else {
         addToAttributeOutputList( *token );
      }
   }
}


void
pool::RelationalCollection::RelationalCollectionQuery::addToOutputList( const std::vector<std::string>& columnNames )
{
  for ( std::vector< std::string >::const_iterator
           iName = columnNames.begin(); iName != columnNames.end(); ++iName )
  {
     addToOutputList( *iName );
  }
}


void
pool::RelationalCollection::RelationalCollectionQuery::selectAllAttributes()
{
   for( int i = 0; i < m_description.numberOfCollectionFragments(); i++ )  {
      for( int j = 0; j < m_description.numberOfAttributeColumns( i ); j++ )    {
         addToAttributeOutputList( m_description.attributeColumn( j, i ).name() );
      }
   }
}


void
pool::RelationalCollection::RelationalCollectionQuery::selectAllTokens()
{
   for( int i = 0; i < m_description.numberOfCollectionFragments(); i++ )  {
      for( int j = 0; j < m_description.numberOfTokenColumns( i ); j++ )    {
         addToTokenOutputList( m_description.tokenColumn( j, i ).name() );
      }
   }
}


void
pool::RelationalCollection::RelationalCollectionQuery::selectAll()
{
  this->selectAllAttributes();
  this->selectAllTokens();
}


void
pool::RelationalCollection::RelationalCollectionQuery::addToCollectionFragmentList( const std::string& fragmentName )
{
   m_collectionFragmentNames.insert( fragmentName );
}


void
pool::RelationalCollection::RelationalCollectionQuery::addToCollectionFragmentList( 
      const std::vector< std::string >& fragmentNames )
{
  for( vector< string >::const_iterator iName = fragmentNames.begin(); iName != fragmentNames.end(); ++iName )  {
    addToCollectionFragmentList( *iName );
  }
}


void
pool::RelationalCollection::RelationalCollectionQuery::addAllCollectionFragments()
{
  for ( int i = 0; i < m_description.numberOfCollectionFragments(); i++ ) {
     addToCollectionFragmentList( m_description.collectionFragment( i ).name() );
  }
}


void
pool::RelationalCollection::RelationalCollectionQuery::
setCondition( const std::string& whereClause,
              coral::AttributeList* attributeBindData,
              pool::TokenList* tokenBindData )
{
  // Names of collection fragments containing metadata used in where clause. Value will be returned by reference.
  std::vector< std::string > collectionFragmentNames;

  // Parse where clause.
  m_whereClause += RelationalCollectionExpressionParser::parse( whereClause,
            m_description,
            m_dataTableNameForCollectionFragmentName,
            m_linksTableNameForCollectionFragmentName,
            m_tableTokenColumnPrefixForCollectionTokenColumnName,
            m_tableAttributeColumnNameForCollectionAttributeColumnName,
            m_mapOfLinkIdForTokenKeyMaps,
            collectionFragmentNames,
            m_linksTableNames,
            m_linkIDToOID1MatchingConditions,
            attributeBindData,
            tokenBindData,
            m_whereClauseBindData );
                                              
  // Add names of collection fragments containing metadata used in where clause to query table list.
  for ( std::vector< std::string >::const_iterator iName = collectionFragmentNames.begin();
        iName != collectionFragmentNames.end(); ++iName )
  {
     addToCollectionFragmentList( *iName );
  }
}


const std::string&
pool::RelationalCollection::RelationalCollectionQuery::whereClause() const
{
  return m_whereClause;
}


void
pool::RelationalCollection::RelationalCollectionQuery::setRowCacheSize( int rowCacheSize )
{
   m_rowCacheSize = rowCacheSize;
}




void
pool::RelationalCollection::RelationalCollectionQuery::prepareQueryCondition()
{
   // has to query at least 1 table
   if( m_collectionFragmentNames.empty() ) {
      addToCollectionFragmentList( m_description.collectionFragmentName( m_description.eventReferenceColumnName() ) );
   }

   // Add primary key matching conditions to where clause.
   for( set<string>::const_iterator
	   fragmentI = m_collectionFragmentNames.begin(), end = m_collectionFragmentNames.end();
	fragmentI != end; ++fragmentI )
   {
      string dataTableName = m_dataTableNameForCollectionFragmentName.find( *fragmentI )->second;
      if( dataTableName != m_dataTableName ) {
	 // Add condition to match primary key of data table with that of top level collection fragment data table.
         string condition = " " + dataTableName + "." + RelationalCollectionNames::recordIdVariableInCollectionDataTable() + " = " + m_dataTableName + "." + RelationalCollectionNames::recordIdVariableInCollectionDataTable() + " ";
	 m_whereClause += ( (m_whereClause.size() > 0)? " AND " : "" )  + condition;
      }
   }
  
   // Add links table ID to data table OID_1 matching conditions to where clause.
   for( std::vector< std::string >::const_iterator iCondition = 
	   m_linkIDToOID1MatchingConditions.begin(); iCondition != 
	   m_linkIDToOID1MatchingConditions.end(); ++iCondition )
   {
      m_whereClause += ( (m_whereClause.size() > 0)? " AND " : "" )  + *iCondition;
   }

}
   

void
pool::RelationalCollection::RelationalCollectionQuery::prepareQuery( )
{
   delete m_query; 
   delete m_outputDataBuffer; 
   m_query = m_session.nominalSchema().newQuery();
   m_outputDataBuffer = new coral::AttributeList ;
   
   // set the default to avoid row by row reading if the user forgets to set this
   m_query->setRowCacheSize( m_rowCacheSize ?  m_rowCacheSize : 1000 );

   if( m_readPrimaryKey )   addPrimaryKeyToSelect();
   
   // Add names of links tables used for Token column conditions to query table list.
   for (const std::string& name : m_linksTableNames) {
      m_query->addToTableList( name );
   }

   // Add data table associated with collection fragment to query table list.
   for( set<string>::const_iterator
	   fragmentI = m_collectionFragmentNames.begin(), end = m_collectionFragmentNames.end();
	fragmentI != end; ++fragmentI )
   {
      m_query->addToTableList( m_dataTableNameForCollectionFragmentName.find( *fragmentI )->second );
   }
   
   m_query->setCondition( m_whereClause, *m_whereClauseBindData );
}


   
// Add primary key of the top level collection fragment to the query select list
// Should be first on the list (assumed so in cursor::next()
void
pool::RelationalCollection::RelationalCollectionQuery::addPrimaryKeyToSelect()
{
   std::string primaryKeyColumnName = m_dataTableName + "." + 
      RelationalCollectionNames::recordIdVariableInCollectionDataTable();
   m_query->addToOutputList( primaryKeyColumnName );
   m_outputDataBuffer->extend<unsigned>( primaryKeyColumnName ); 
}




pool::ICollectionCursor& 
pool::RelationalCollection::RelationalCollectionQuery::execute()
{
   // Add event reference Token column to query select list
   // if one is defined in the collection description
   // and exclusion was not requested
  if( !m_skipEventRef && m_description.hasEventReferenceColumn() )  {
     addToTokenOutputList( m_description.eventReferenceColumnName() );
  }

  prepareQueryCondition();
  prepareQuery();
  
  // Bind the selected Tokens.
  for( TokenList::iterator iToken = m_outputTokenList->begin(); 
       iToken != m_outputTokenList->end(); ++iToken )
  {
    // Form the OID_1 and OID_2 column names of Token.
     std::string tableColumnPrefix = m_tableTokenColumnPrefixForCollectionTokenColumnName.find( iToken.tokenName() )->second;
    std::string oid1ColumnName = tableColumnPrefix + RelationalCollectionNames::oid_1_variableInCollectionDataTable();
    std::string oid2ColumnName = tableColumnPrefix + RelationalCollectionNames::oid_2_variableInCollectionDataTable();

    // cout << "-->>>***  Adding Token column to query: " << iToken.tokenName()
    //<< ", odi2 DBcolumn name is " << oid2ColumnName << endl;

    // Add column names for this Token to query selection list.
    m_query->addToOutputList( oid1ColumnName );
    m_query->addToOutputList( oid2ColumnName );

    // Add columns for this Token to query output data.
    m_outputDataBuffer->extend( oid1ColumnName, typeid(unsigned) );
    m_outputDataBuffer->extend( oid2ColumnName, typeid(unsigned) );
  }  

  // Bind the selected Attributes.
  for( std::vector< std::string >::const_iterator iName = m_selectedAttributeColumnNames.begin();
       iName != m_selectedAttributeColumnNames.end(); ++iName ) 
  {
     // select column name of Attribute and add Attribute to query bind data.
     const coral::Attribute& tableAttribute = m_tableAttributeList[ m_tableAttributeColumnNameForCollectionAttributeColumnName.find( *iName )->second ];
     std::string tableColumnName = tableAttribute.specification().name();
     m_query->addToOutputList( tableColumnName );
     m_outputDataBuffer->extend( tableColumnName, tableAttribute.specification().type() );

     // Bind Attributes selected by query to Attribute list accessed by user.
     // MN: do we want unsafe bind here?  or something different
//    (*m_outputDataBuffer)[tableColumnName].bindUnsafely( m_tableAttributeList[*iName].addressOfData() );
//     (*m_outputDataBuffer)[tableColumnName].bindUnsafely( (*m_outputAttributeList)[*iName].addressOfData() );
     (*m_outputAttributeList)[*iName].shareData( (*m_outputDataBuffer)[tableColumnName] );
  }

  // Execute query and retrieve cursor for navigation over result.
  m_query->defineOutput( *m_outputDataBuffer );
  coral::ICursor& cursor = m_query->execute();

  // Create collection row buffer to contain query output.
  pool::CollectionRowBuffer collectionRowBuffer( *m_outputTokenList, *m_outputAttributeList );
  m_cursor = new RelationalCollectionCursor( cursor,
                                             m_description,
                                             m_mapOfTokenKeyForLinkIdMaps,
                                             collectionRowBuffer );
  return *m_cursor;
}




void
pool::RelationalCollection::RelationalCollectionQuery::addToTokenOutputList( const std::string& columnName )
{
  if ( m_tableTokenColumnPrefixForCollectionTokenColumnName.find( columnName ) == 
       m_tableTokenColumnPrefixForCollectionTokenColumnName.end() )
  {
    std::string errorMsg( "Token column with name `" + columnName + "' does not exist." );
    throw pool::Exception( errorMsg,
                           "RelationalCollectionQuery::addToTokenOutputList",
                           "RelationalCollection" );
  }

  if (m_selectedTokenColumnNames.insert(columnName).second) {
     // cout << "Adding token column to query output list:" << columnName << endl;
     m_outputTokenList->extend( columnName );
     addToCollectionFragmentList( m_description.collectionFragmentName( columnName ) );
  }
}




void
pool::RelationalCollection::RelationalCollectionQuery::addToAttributeOutputList( const std::string& columnName )
{  
   // Check for column existence.
   if ( m_tableAttributeColumnNameForCollectionAttributeColumnName.find( columnName ) == 
        m_tableAttributeColumnNameForCollectionAttributeColumnName.end() )
   {
      std::string errorMsg( "Attribute column with name `" + columnName + "' does not exist." );
      throw pool::Exception( errorMsg,
                             "RelationalCollectionQuery::addToAttributeOutputList",
                             "RelationalCollection" );
   }

   // Add to select list.
   if( find( m_selectedAttributeColumnNames.begin(), m_selectedAttributeColumnNames.end(), columnName ) == m_selectedAttributeColumnNames.end() )
   {
      m_selectedAttributeColumnNames.push_back( columnName );        
      const coral::Attribute& tableAttribute = m_tableAttributeList[ m_tableAttributeColumnNameForCollectionAttributeColumnName.find( columnName )->second ];
      m_outputAttributeList->extend( columnName, tableAttribute.specification().type() );
      addToCollectionFragmentList( m_description.collectionFragmentName( columnName ) );
      // cout << "Adding attribute column to query output list:" << columnName << endl;
   }
}



void
pool::RelationalCollection::RelationalCollectionQuery::
skipEventReference( bool skip )
{
   m_skipEventRef = skip;
}
