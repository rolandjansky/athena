/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/******************************************************************************
Name:        TAcceptInfo

Author:      Karsten Koeneke (CERN)
Author:      Nils Krumnack (Iowa State University)
Created:     April 2011
Created:     September 2017

Description: Object to encode the result of several cuts
******************************************************************************/

// This class' header
#include "PATCore/TAcceptInfo.h"


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
Root::TAcceptInfo::TAcceptInfo(const char* name) :
  m_name(name),
  m_cutMap()
{
}




//=============================================================================
// Adding a cut
//=============================================================================
int Root::TAcceptInfo::addCut( const std::string& cutName, const std::string& cutDescription )
{
  // Make sure that this new cuts doesn't exceed the number of bits available
  if ( m_cutMap.size() >= NBITS )
    {
      return -1;
    }

  // Add the cut to the map
  std::pair< std::string, unsigned int > cutPair = std::make_pair( cutDescription, m_cutMap.size() );
  m_cutMap.insert( std::make_pair( cutName, cutPair ) );

  // Return the position of the newly added cut in the bitmask
  int result = (m_cutMap.size() - 1);
  m_cutMask.set (result);
  return result;
}




//=============================================================================
// Get the description of a cut based on the cut position
//=============================================================================
const std::string& Root::TAcceptInfo::getCutName( unsigned int cutPosition ) const
{
  static const std::string emptyString;

  // Make sure that this cut doesn't exceed the number of defined cuts
  if ( cutPosition >= m_cutMap.size() )
    {
      return emptyString;
    }

  // iterate over the map and find the right position
  auto it    = m_cutMap.begin();
  auto itEnd = m_cutMap.end();
  for ( ; it != itEnd; ++it )
    {
      if ( (it->second).second == cutPosition )
        {
          return (it->first);
        }
    }

  return emptyString;
}




//=============================================================================
// Get the description of a cut based on the cut position
//=============================================================================
const std::string& Root::TAcceptInfo::getCutDescription( unsigned int cutPosition ) const
{
  static const std::string emptyString;

  // Make sure that this cut doesn't exceed the number of defined cuts
  if ( cutPosition >= m_cutMap.size() )
    {
      return emptyString;
    }

  // iterate over the map and find the right position
  auto it    = m_cutMap.begin();
  auto itEnd = m_cutMap.end();
  for ( ; it != itEnd; ++it )
    {
      if ( (it->second).second == cutPosition )
        {
          return (it->second).first;
        }
    }

  return emptyString;
}





//=============================================================================
// Set the description of a cut based on the cut position
//=============================================================================
void Root::TAcceptInfo::setCutDescription( const unsigned int cutPosition, const std::string& cutDescription )
{
  // iterate over the map and find the right position
  auto it    = m_cutMap.begin();
  auto itEnd = m_cutMap.end();
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





/** Get the description of a cut, based on the cut name */
const std::string& Root::TAcceptInfo :: getCutDescription( const std::string& cutName ) const
{
  static const std::string emptyString;
  auto it = m_cutMap.find(cutName);
  return (it != m_cutMap.end()) ? (it->second).first : emptyString;
}
