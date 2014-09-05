/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ImplicitCollectionIterator.h"
#include "CollectionBase/CollectionDescription.h"

#include "PersistencySvc/IContainer.h"
#include "PersistencySvc/ITokenIterator.h"

#include "PersistentDataModel/Token.h"

pool::ImplicitCollectionIterator::
ImplicitCollectionIterator( pool::IContainer& container,
                            const pool::ICollectionDescription& description )
      :
      m_container( container ),
      m_tokenIterator( 0 ),
      m_token( 0 )
{
   TokenList        tokenList;
   tokenList.extend( description.eventReferenceColumnName() );
   m_rowBuffer.setTokenList( tokenList );
}


pool::ImplicitCollectionIterator::~ImplicitCollectionIterator()
{
  if ( m_token ) m_token->release();
  delete m_tokenIterator;
}


void
pool::ImplicitCollectionIterator::
setCondition( const std::string& whereClause,
              coral::AttributeList*,
              TokenList* )
{
   m_whereClause += whereClause;
}


pool::ICollectionCursor&
pool::ImplicitCollectionIterator::execute()
{
   m_tokenIterator = m_container.tokens( m_whereClause );
   return *this;
}



const std::string&
pool::ImplicitCollectionIterator::whereClause() const
{
  return m_whereClause;
}



bool
pool::ImplicitCollectionIterator::next()
{
   if( m_token ) {
      m_token->release();
      m_token = 0;
   }
   if( m_tokenIterator )
      m_token = m_tokenIterator->next();
   return (m_token!=0);
}

Token*
pool::ImplicitCollectionIterator::token() const
{
  return m_token;
}

const pool::CollectionRowBuffer&
pool::ImplicitCollectionIterator::currentRow() const
{
   m_token->setData( &*m_rowBuffer.tokenList().begin() );
   return m_rowBuffer;
}


bool
pool::ImplicitCollectionIterator::seek (long long int position)
{
  IPositionSeek* ies = dynamic_cast<IPositionSeek*> (m_tokenIterator);
  if (!ies)
    return false;
  // We'll have to do a next() to read the event.
  // So subtract one here to compensate for that.
  return ies->seek (position - 1);
}


int
pool::ImplicitCollectionIterator::size ()
{
  ICollectionSize* ics = dynamic_cast<ICollectionSize*> (m_tokenIterator);
  if (!ics)
    return 0;
  return ics->size();
}
