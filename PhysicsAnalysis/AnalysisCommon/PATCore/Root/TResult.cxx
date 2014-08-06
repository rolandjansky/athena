/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/******************************************************************************
Name:        TResult

Author:      Karsten Koeneke (CERN)
Created:     October 2011

Description: Object to encode the result of efficiency/scale factor/resolution calculation
******************************************************************************/

// This class' header
#include "PATCore/TResult.h"


// include math
#include <math.h>
#include <utility>
#include <map>
#include <iostream>

// ROOT includes
#include <TString.h>




//=============================================================================
// Constructor
//=============================================================================
Root::TResult::TResult(const char* name) :
  m_name(name),
  m_defaultResult(0.0),
  m_result(),
  m_resultMap(),
  m_emptyString("")
{
}




//=============================================================================
// Copy Constructor: DO NOT COPY IT! It is expensive to copy!
//=============================================================================
Root::TResult::TResult(const TResult& parent) :
  m_defaultResult( parent.m_defaultResult ),
  m_result( parent.m_result ),
  m_resultMap( parent.m_resultMap ),
  m_emptyString( parent.m_emptyString )
{
}



// Standard assignment operator: DO NOT COPY IT! It is expensive to copy!
Root::TResult& Root::TResult::operator= ( const TResult& rhs )
{
  // check for self-assignment by comparing addresses
  if (this == &rhs) return *this;

  // do the copy
  m_defaultResult = rhs.m_defaultResult;
  m_result        = rhs.m_result;
  m_resultMap     = rhs.m_resultMap;
  m_emptyString   = rhs.m_emptyString;
  
  // return the existing object
  return *this;
}



//=============================================================================
// Destructor
//=============================================================================
Root::TResult::~TResult()
{
}




//=============================================================================
// Adding a result
//=============================================================================
int Root::TResult::addResult( const TString& resultName, const TString& resultDescription )
{
  // Add the result to the map
  std::pair< TString, unsigned int > resultPair = std::make_pair( resultDescription, m_resultMap.size() );
  m_resultMap.insert( std::make_pair( resultName, resultPair ) );

  // Add the default result entry to the vector of results (doubles)
  m_result.push_back( m_defaultResult );

  // Return the position of the newly added result in the map
  return (int)(m_resultMap.size() - 1);
}




//=============================================================================
// Get the description of a result based on the result position
//=============================================================================
const TString& Root::TResult::getResultName( unsigned int resultPosition ) const
{
  // Make sure that this result doesn't exceed the number of defined results
  if ( resultPosition >= m_resultMap.size() )
    {
      return m_emptyString;
    }

  // iterate over the map and find the right position
  std::map< TString, std::pair< TString, unsigned int > >::const_iterator it    = m_resultMap.begin();
  std::map< TString, std::pair< TString, unsigned int > >::const_iterator itEnd = m_resultMap.end();
  for ( ; it != itEnd; ++it )
    {
      if ( (it->second).second == resultPosition )
        {
          return (it->first);
        }
    }

  return m_emptyString;
}




//=============================================================================
// Get the description of a result based on the result position
//=============================================================================
const TString& Root::TResult::getResultDescription( unsigned int resultPosition ) const
{
  // Make sure that this result doesn't exceed the number of defined results
  if ( resultPosition >= m_resultMap.size() )
    {
      return m_emptyString;
    }

  // iterate over the map and find the right position
  std::map< TString, std::pair< TString, unsigned int > >::const_iterator it    = m_resultMap.begin();
  std::map< TString, std::pair< TString, unsigned int > >::const_iterator itEnd = m_resultMap.end();
  for ( ; it != itEnd; ++it )
    {
      if ( (it->second).second == resultPosition )
        {
          return (it->second).first;
        }
    }

  return m_emptyString;
}





//=============================================================================
// Set the description of a result based on the result position
//=============================================================================
void Root::TResult::setResultDescription( const unsigned int resultPosition, const TString& resultDescription )
{
  // iterate over the map and find the right position
  std::map< TString, std::pair< TString, unsigned int > >::iterator it    = m_resultMap.begin();
  std::map< TString, std::pair< TString, unsigned int > >::iterator itEnd = m_resultMap.end();
  for ( ; it != itEnd; ++it )
    {
      if ( (it->second).second == resultPosition )
        {
          ((it->second).first) = resultDescription;
          return;
        }
    }

  return;
}



