/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/******************************************************************************
Name:        TAccept

Author:      Karsten Koeneke (CERN)
Created:     April 2011

Description: Object to encode the result of several cuts
******************************************************************************/

// This class' header
#include "PATCore/TAccept.h"


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
Root::TAccept::TAccept(const char* name) :
  m_name(name),
  m_accept(), // all bits initialized to zero
  m_cutMap(),
  m_emptyString("")
{
}




//=============================================================================
// Copy Constructor: DO NOT COPY IT! It is expensive to copy!
//=============================================================================
Root::TAccept::TAccept(const TAccept& parent) :
  m_name( parent.m_name ),
  m_accept( parent.m_accept ),
  m_cutMap( parent.m_cutMap ),
  m_emptyString( parent.m_emptyString )
{
}




//=============================================================================
// Assignemnt operator: DO NOT COPY IT! It is expensive to copy!
//=============================================================================
Root::TAccept& Root::TAccept::operator= ( const TAccept& rhs )
{
  // check for self-assignment by comparing addresses
  if (this == &rhs) return *this;

  // do the copy
  m_name        = rhs.m_name;
  m_accept      = rhs.m_accept;
  m_cutMap      = rhs.m_cutMap;
  m_emptyString = rhs.m_emptyString;
  
  // return the existing object
  return *this;
}




//=============================================================================
// Destructor
//=============================================================================
Root::TAccept::~TAccept()
{
}




//=============================================================================
// Adding a cut
//=============================================================================
int Root::TAccept::addCut( const TString& cutName, const TString& cutDescription )
{
  // Make sure that this new cuts doesn't exceed the number of bits available
  if ( m_cutMap.size() >= m_nBits )
    {
      return -1;
    }

  // Add the cut to the map
  std::pair< TString, unsigned int > cutPair = std::make_pair( cutDescription, m_cutMap.size() );
  m_cutMap.insert( std::make_pair( cutName, cutPair ) );

  // Return the position of the newly added cut in the bitmask
  return (int)(m_cutMap.size() - 1);
}




//=============================================================================
// Get the description of a cut based on the cut position
//=============================================================================
const TString& Root::TAccept::getCutName( unsigned int cutPosition ) const
{
  // Make sure that this cut doesn't exceed the number of defined cuts
  if ( cutPosition >= m_cutMap.size() )
    {
      return m_emptyString;
    }

  // iterate over the map and find the right position
  std::map< TString, std::pair< TString, unsigned int > >::const_iterator it    = m_cutMap.begin();
  std::map< TString, std::pair< TString, unsigned int > >::const_iterator itEnd = m_cutMap.end();
  for ( ; it != itEnd; ++it )
    {
      if ( (it->second).second == cutPosition )
        {
          return (it->first);
        }
    }

  return m_emptyString;
}




//=============================================================================
// Get the description of a cut based on the cut position
//=============================================================================
const TString& Root::TAccept::getCutDescription( unsigned int cutPosition ) const
{
  // Make sure that this cut doesn't exceed the number of defined cuts
  if ( cutPosition >= m_cutMap.size() )
    {
      return m_emptyString;
    }

  // iterate over the map and find the right position
  std::map< TString, std::pair< TString, unsigned int > >::const_iterator it    = m_cutMap.begin();
  std::map< TString, std::pair< TString, unsigned int > >::const_iterator itEnd = m_cutMap.end();
  for ( ; it != itEnd; ++it )
    {
      if ( (it->second).second == cutPosition )
        {
          return (it->second).first;
        }
    }

  return m_emptyString;
}





//=============================================================================
// Set the description of a cut based on the cut position
//=============================================================================
void Root::TAccept::setCutDescription( const unsigned int cutPosition, const TString& cutDescription )
{
  // iterate over the map and find the right position
  std::map< TString, std::pair< TString, unsigned int > >::iterator it    = m_cutMap.begin();
  std::map< TString, std::pair< TString, unsigned int > >::iterator itEnd = m_cutMap.end();
  for ( ; it != itEnd; ++it )
    {
      if ( (it->second).second == cutPosition )
        {
          ((it->second).first) = cutDescription;
          return;
        }
    }

  return;
}



