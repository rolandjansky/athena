/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CollectionBase/TokenList.h"

#include "PersistentDataModel/Token.h"
#include "POOLCore/Exception.h"

#include <ostream>
#include <sstream>

using namespace std;
using namespace pool;

pool::TokenList::TokenList()
{}


pool::TokenList::TokenList( const std::vector<std::string>& specification )
{
   createFromSpec( specification );
}


void TokenList::createFromSpec( const std::vector<std::string>& specification ) 
{
   // copy token names
   m_tokenNames = specification;
   for( size_t i = 0; i < m_tokenNames.size(); ++i ) {
      // create vector of empty tokens
      Token* token = new Token;
      m_tokenVector.push_back( token );
      // create mapping from name to the token
      m_tokenMap[ m_tokenNames[i] ] = token;
   }
}

pool::TokenList::TokenList( const pool::TokenList& rhs )
{
   *this = rhs;
}


void TokenList::clear()
{
   // release Tokens  (only from the Vector - the Map links are not counted)
   for( size_t i = 0; i < m_tokenVector.size(); ++i ) {
      m_tokenVector[i]->release();
   } 
   m_tokenVector.clear();
   m_tokenMap.clear();
   m_tokenNames.clear();
}



pool::TokenList::~TokenList()
{
   clear();
}


pool::TokenList&
pool::TokenList::operator=( const pool::TokenList& rhs )
{
   clear();
   createFromSpec( rhs.m_tokenNames );
   for( size_t i = 0; i < m_tokenVector.size(); ++i ) {
      rhs.m_tokenVector[i]->setData( m_tokenVector[i] );
   }
   return *this;
}


bool
pool::TokenList::operator==( const pool::TokenList& rhs ) const
{
  if( this->size() != rhs.size() )
     return false;
  for( size_t i = 0; i < this->size(); ++i )
     if( (*this)[i] != rhs[i] )
      return false;
  return true;
}


void
pool::TokenList::extend( const std::string& name )
{
   m_tokenNames.push_back( name );
   Token* token = new Token;
   m_tokenVector.push_back( token ); 
   m_tokenMap[ name ] = token;  
}



Token&
pool::TokenList::operator[]( const std::string& name )
{
   std::map< std::string, Token* >::iterator iData = m_tokenMap.find( name );

   if( iData == m_tokenMap.end() )   {
      std::string errorMsg = "Cannot find Token with name `" + name + "' in Token list.";
      throw pool::Exception( errorMsg,
			     "TokenList::operator[](name)",
			     "CollectionBase" );
   }
   return *( iData->second );
}


Token&
pool::TokenList::operator[]( unsigned int index )
{
   if( index >= m_tokenVector.size() ) {
      std::ostringstream errorMsg;
      errorMsg << "Cannot find Token with index `" << index << "' in Token list.";
      throw pool::Exception( errorMsg.str(),
			     "TokenList::operator[](index)",
			     "CollectionBase" );
   }
   return *m_tokenVector[index];
}


const Token&
pool::TokenList::operator[]( const std::string& name ) const
{
   std::map< std::string, Token* >::const_iterator iData = m_tokenMap.find( name );

   if( iData == m_tokenMap.end() )   {
      std::string errorMsg = "Cannot find Token with name `" + name + "' in Token list.";
      throw pool::Exception( errorMsg,
			     "TokenList::operator[](name)",
			     "CollectionBase" );
   }
   return *( iData->second );
}


const Token&
pool::TokenList::operator[]( unsigned int index ) const
{
   if( index >= m_tokenVector.size() ) {
      std::ostringstream errorMsg;
      errorMsg << "Cannot find Token with index `" << index << "' in Token list.";
      throw pool::Exception( errorMsg.str(),
			     "TokenList::operator[](index)",
			     "CollectionBase" );
   }
   return *m_tokenVector[index];
}



std::ostream&
pool::TokenList::toOutputStream( std::ostream& os ) const
{
   for( size_t i = 0; i <= m_tokenVector.size(); ++i ) {
      os << "[" << m_tokenNames[i] << " (Token) : " << m_tokenVector[i]->toString()<< "]";
   }

   return os;
}

