/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TagEvent/TagFragmentCollection.h"

/** Copy Constructor */
TagFragmentCollection::TagFragmentCollection( const TagFragmentCollection& rhs )
  : m_attrData (rhs.m_attrData)
{
}

/** Assignement operator */
TagFragmentCollection& TagFragmentCollection::operator=( const TagFragmentCollection& rhs ) {
  if ( this != &rhs ) {
    m_attrData = rhs.m_attrData;
  }
  return *this;
}


