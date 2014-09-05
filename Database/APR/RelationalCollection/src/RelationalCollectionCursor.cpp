/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RelationalCollectionCursor.h"

#include "PersistentDataModel/Token.h"

#include "CollectionBase/ICollectionDescription.h"
#include "CollectionBase/CollectionRowBuffer.h"

#include "RelationalAccess/ICursor.h"

#include "CoralBase/Attribute.h"

//#include <iostream>
using namespace std;

pool::RelationalCollection::RelationalCollectionCursor::RelationalCollectionCursor(
   coral::ICursor& cursor,
   const pool::ICollectionDescription& description,
   const std::map< std::string, std::map< unsigned, std::string >* >& mapOfTokenKeyForLinkIdMaps,
   const pool::CollectionRowBuffer& collectionRowBuffer )
      : m_cursor( cursor ),
        m_description( description ),
        m_mapOfTokenKeyForLinkIdMaps( mapOfTokenKeyForLinkIdMaps ),
        m_collectionRowBuffer( new pool::CollectionRowBuffer( collectionRowBuffer ) ),
        m_primaryKey( 0 )
{}


pool::RelationalCollection::RelationalCollectionCursor::~RelationalCollectionCursor()
{
  this->close();
  delete m_collectionRowBuffer;   //?  MN
}


bool
pool::RelationalCollection::RelationalCollectionCursor::next()
{
   if( !m_cursor.next() )  {
      return false;
   }

  // Get iterator over current row.
  coral::AttributeList::const_iterator iData = m_cursor.currentRow().begin();
/*
  cout << " * Cursor next(), values: " << endl;
  for( ; iData != m_cursor.currentRow().end(); ++iData ) {
      std::cout << "[";
      iData->toOutputStream( std::cout );
      std::cout << "] ";
  }
  cout << endl;  
  iData = m_cursor.currentRow().begin();
*/
  // Get primary key for current row.
  m_primaryKey = iData->data< unsigned >();
  ++iData;

  const Token empty_token;
  // Get Tokens for current row and add them to row buffer.
  for( pool::TokenList::iterator iToken = m_collectionRowBuffer->tokenList().begin(); 
       iToken != m_collectionRowBuffer->tokenList().end(); ++iToken, ++iData )
  {
     // clear target Token before filling (this does not affect tokenName!)
     empty_token.setData(&*iToken);  
     const std::string& tokenName = iToken.tokenName();
     const std::string fragmentName = m_description.collectionFragmentName( tokenName );
     std::map< unsigned, std::string >* tokenKeyForLinkId = m_mapOfTokenKeyForLinkIdMaps.find( fragmentName )->second;
     unsigned linkId = iData->data< unsigned >();
     iToken->fromString( ( tokenKeyForLinkId->find( linkId ) )->second );
     iToken->oid().first = static_cast<int>( linkId );
     ++iData;
     unsigned oid2 = iData->data< unsigned >();
     iToken->oid().second = static_cast<int>( oid2 );     
  }
  return true;
}


const pool::CollectionRowBuffer& 
pool::RelationalCollection::RelationalCollectionCursor::currentRow() const
{
  return *m_collectionRowBuffer;
}


const Token& 
pool::RelationalCollection::RelationalCollectionCursor::eventRef() const
{
   return ( m_collectionRowBuffer->tokenList()[ m_description.eventReferenceColumnName() ] );
}


unsigned 
pool::RelationalCollection::RelationalCollectionCursor::primaryKey() const
{
  return m_primaryKey;
}


void 
pool::RelationalCollection::RelationalCollectionCursor::close()
{}

