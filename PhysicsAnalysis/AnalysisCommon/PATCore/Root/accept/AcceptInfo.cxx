/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/******************************************************************************
Name:        AcceptInfo

Author:      Karsten Koeneke (CERN)
Author:      Nils Krumnack (Iowa State University)
Created:     April 2011
Created:     September 2017

Description: Object to encode the result of several cuts
******************************************************************************/

// This class' header
#include "PATCore/AcceptInfo.h"


// include math
#include <math.h>
#include <utility>
#include <map>
#include <iostream>

// ROOT includes
#include <TString.h>




//=============================================================================
// Get the description of a cut based on the cut position
//=============================================================================
const std::string& asg::AcceptInfo::getCutName( unsigned int cutPosition ) const
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
const std::string& asg::AcceptInfo::getCutDescription( unsigned int cutPosition ) const
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
void asg::AcceptInfo::setCutDescription( const unsigned int cutPosition, const std::string& cutDescription )
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
const std::string& asg::AcceptInfo :: getCutDescription( const std::string& cutName ) const
{
  static const std::string emptyString;
  auto it = m_cutMap.find(cutName);
  return (it != m_cutMap.end()) ? (it->second).first : emptyString;
}
