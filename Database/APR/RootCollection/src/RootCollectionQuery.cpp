/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RootCollectionQuery.h"
#include "RootCollectionCursor.h"

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
#include "CoralBase/MessageStream.h"

#include "TEventList.h"
#include "TFormula.h"
#include "APRTreeFormula.h"

//#include <iostream>
//using namespace std;


pool::RootCollection::RootCollectionQuery::
RootCollectionQuery(
   const pool::ICollectionDescription& description,
   TTree *tree
   ) :
      m_description( description ),
      m_tree( tree ),
      m_cursor( 0 ),
      m_skipEventRef( false )
{
}


pool::RootCollection::RootCollectionQuery::~RootCollectionQuery()
{
   delete m_cursor;   m_cursor = 0;
}


void
pool::RootCollection::RootCollectionQuery::addToOutputList( const std::string& columnNames )
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
pool::RootCollection::RootCollectionQuery::addToOutputList( const std::vector<std::string>& columnNames )
{
  for ( std::vector< std::string >::const_iterator
           iName = columnNames.begin(); iName != columnNames.end(); ++iName )
  {
     addToOutputList( *iName );
  }
}


void
pool::RootCollection::RootCollectionQuery::selectAllAttributes()
{
   for( int i = 0; i < m_description.numberOfCollectionFragments(); i++ )  {
      for( int j = 0; j < m_description.numberOfAttributeColumns( i ); j++ )    {
         addToAttributeOutputList( m_description.attributeColumn( j, i ).name() );
      }
   }
}


void
pool::RootCollection::RootCollectionQuery::selectAllTokens()
{
   for( int i = 0; i < m_description.numberOfCollectionFragments(); i++ )  {
      for( int j = 0; j < m_description.numberOfTokenColumns( i ); j++ )    {
         addToTokenOutputList( m_description.tokenColumn( j, i ).name() );
      }
   }
}


void
pool::RootCollection::RootCollectionQuery::selectAll()
{
   selectAllAttributes();
   selectAllTokens();
}


void
pool::RootCollection::RootCollectionQuery::addToCollectionFragmentList( const std::string& fragmentName )
{
   if( m_collectionFragmentNames.find( fragmentName ) == m_collectionFragmentNames.end() )  {
      m_collectionFragmentNames.insert( fragmentName );

      /*
      // Add data table associated with collection fragment to query table list.
      std::string dataTableName = ( m_dataTableNameForCollectionFragmentName.find( fragmentName ) )->second;
      m_query->addToTableList( dataTableName );

      // Add condition to match primary key of data table with that of top level collection fragment data table.
      if ( dataTableName != m_dataTableName )    {
         std::string condition = " " + dataTableName + "." + pool::RootCollection::RootCollectionNames::recordIdVariableInCollectionDataTable() + " = " + m_dataTableName + "." + pool::RootCollection::RootCollectionNames::recordIdVariableInCollectionDataTable() + " ";
         m_primaryKeyMatchingConditions.push_back( condition );
      }
      */
    
   } 
}


void
pool::RootCollection::RootCollectionQuery::
addToCollectionFragmentList( const std::vector< std::string >& fragmentNames )
{
   for( std::vector< std::string >::const_iterator iName = fragmentNames.begin(); iName != fragmentNames.end(); ++iName )  {
     addToCollectionFragmentList( *iName );
  }
}


void
pool::RootCollection::RootCollectionQuery::addAllCollectionFragments()
{
  for( int i = 0; i < m_description.numberOfCollectionFragments(); i++ )  {
     addToCollectionFragmentList( m_description.collectionFragment( i ).name() );
  }
}


void
pool::RootCollection::RootCollectionQuery::
setCondition( const std::string& whereClause,
              coral::AttributeList* /* attributeBindData */,
              pool::TokenList* /*tokenBindData */ )
{
   m_whereClause += whereClause;
}


const std::string&
pool::RootCollection::RootCollectionQuery::whereClause() const
{
  return m_whereClause;
}


void
pool::RootCollection::RootCollectionQuery::setRowCacheSize( int )
{
}



pool::ICollectionCursor& 
pool::RootCollection::RootCollectionQuery::execute()
{
   //cout << "* Executing query: " << m_whereClause << endl;
   if( !m_skipEventRef && m_description.hasEventReferenceColumn() )  {
      addToTokenOutputList( m_description.eventReferenceColumnName() );
   }
  
  TEventList* eventList = 0;

  if( m_whereClause.size() && m_tree->GetEntries()>0 ) {
     // branch adresses have to be reset here!
     // if select is not called for the first time
     // the branches might still be bound to attribute addresses
     // which are already deleted. If TTree::Draw is called in 
     // this case, it writes to freed memory ... bad things will happen soon.
     const TObjArray* branches = m_tree->GetListOfBranches();
     Int_t nbranches = branches->GetEntriesFast();
     for (int i=0; i<nbranches-1; ++i){
        TBranch* branch = (TBranch*)branches->UncheckedAt(i);
        branch->ResetAddress();
     }
     eventList = new TEventList("preselected","");
     eventList->SetDirectory(0);

     APRTreeFormula formula(m_whereClause, m_tree);
     if( !formula.GetNdim() )  {
        std::string errorMsg = "Could not construct TTreeFormula object, probably because of incorrect ROOT predicate syntax in expression `" + m_whereClause + "'";
        throw pool::Exception( errorMsg,
                               "RootCollectionQuery::execute", 
                               "RootCollection");
     }
     int entries = (int)m_tree->GetEntries();
     for( int i = 0; i < entries; ++i ) {
        m_tree->LoadTree(i);
        if( formula.EvalInstance() )
           eventList->Enter(i);
     }
  }
  
  // Create collection row buffer to contain query output.
  pool::CollectionRowBuffer collectionRowBuffer( m_outputTokenList, m_outputAttributeList );

  // Execute query and create a cursor for iterating over the result.
 //  m_query->defineOutput( *m_outputDataBuffer );

  m_cursor = new RootCollectionCursor( m_description, collectionRowBuffer, m_tree, eventList );

  return *m_cursor;
}



void
pool::RootCollection::RootCollectionQuery::addToTokenOutputList( const std::string& columnName )
{
   // Add to select list, if not already present
   if( m_selectedColumnNames.find( columnName ) == m_selectedColumnNames.end() ) {
      try {
         m_description.tokenColumn( columnName ); 
      } catch( pool::Exception& /* e */ ) {
         std::string errorMsg( "Token column with name `" + columnName + "' does not exist." );
         throw pool::Exception( errorMsg,
                             "RootCollectionQuery::addToTokenOutputList",
                             "RootCollection" );
      }
      m_outputTokenList.extend( columnName );
      m_selectedColumnNames.insert( columnName );
      // keep track of fragments - not really used yet
      addToCollectionFragmentList( m_description.collectionFragmentName( columnName ) );
//      cout << "Adding token column to query output list:" << columnName << endl;
   }   
}



void
pool::RootCollection::RootCollectionQuery::addToAttributeOutputList( const std::string& columnName )
{  
   // Add to select list, if not already present
   if( m_selectedColumnNames.find( columnName ) == m_selectedColumnNames.end() ) {
      try {
         m_outputAttributeList.extend( columnName, m_description.attributeColumn( columnName ).type() );
      } catch( pool::Exception& /* e */) {
         std::string errorMsg( "Attribute column with name `" + columnName + "' does not exist." );
         throw pool::Exception( errorMsg,
                             "RootCollectionQuery::addToAttributeOutputList",
                             "RootCollection" );
      }         
      m_selectedColumnNames.insert( columnName );
      // keep track of fragments - not really used yet
      addToCollectionFragmentList( m_description.collectionFragmentName( columnName ) );
//      cout << "Adding attribute column to query output list:" << columnName << endl;
   }
}



void
pool::RootCollection::RootCollectionQuery::
skipEventReference( bool skip )
{
   m_skipEventRef = skip;
}





