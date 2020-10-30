/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CollectionBase/CollectionRowBuffer.h"

pool::CollectionRowBuffer::CollectionRowBuffer()
 : m_tokenList(),
   m_attributeList (new coral::AttributeList)
{}


pool::CollectionRowBuffer::CollectionRowBuffer( const pool::TokenList& tokenList,
                                                coral::AttributeList& attributeList )
      : m_tokenList( tokenList ),
        m_attributeList( new coral::AttributeList )
{
   // share data
   m_attributeList->merge( attributeList );
}


pool::CollectionRowBuffer::CollectionRowBuffer( const pool::CollectionRowBuffer& rhs )
  : m_tokenList( rhs.m_tokenList ),
    m_attributeList( new coral::AttributeList )
{
   // share the data
   m_attributeList->merge( *rhs.m_attributeList );
}


bool pool::CollectionRowBuffer::deleteAL ATLAS_NOT_THREAD_SAFE()
{
  delete m_attributeList;
  m_attributeList = nullptr;
  return true;
}


pool::CollectionRowBuffer::~CollectionRowBuffer()
{
  [[maybe_unused]] bool flag ATLAS_THREAD_SAFE = deleteAL();
}


pool::CollectionRowBuffer&
pool::CollectionRowBuffer::operator=( const pool::CollectionRowBuffer& rhs )
{
  m_tokenList = rhs.m_tokenList;
  *m_attributeList = *rhs.m_attributeList;

  return *this;
}


bool
pool::CollectionRowBuffer::operator==( const pool::CollectionRowBuffer& rhs ) const
{
  if ( ( m_tokenList != rhs.m_tokenList ) || ( *m_attributeList != *rhs.m_attributeList ) )
  {
    return false;
  }

  return true;
}


bool
pool::CollectionRowBuffer::operator!=( const pool::CollectionRowBuffer& rhs ) const
{
  return ( ! ( *this == rhs ) );
}


void
pool::CollectionRowBuffer::setTokenList( const pool::TokenList& tokenList )
{
  m_tokenList = tokenList;
}


void
pool::CollectionRowBuffer::setAttributeList( const coral::AttributeList& attributeList )
{
  *m_attributeList = attributeList;
}


pool::TokenList&
pool::CollectionRowBuffer::tokenList()
{
  return m_tokenList;
}


coral::AttributeList&
pool::CollectionRowBuffer::attributeList()
{
  return *m_attributeList;
}


const pool::TokenList&
pool::CollectionRowBuffer::tokenList() const
{
  return m_tokenList;
}


const coral::AttributeList&
pool::CollectionRowBuffer::attributeList() const
{
  return *m_attributeList;
}

