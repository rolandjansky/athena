/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "InDetReadoutGeometry/SiDetectorElementMap.h"


SiDetectorElementMap::SiDetectorElementMap()
{
  m_detElementMap.clear();
}

SiDetectorElementMap::~SiDetectorElementMap()
{
  m_detElementMap.clear();
}

void SiDetectorElementMap::addEntry(const IdentifierHash hashId, const InDetDD::SiDetectorElement* elt)
{
  m_detElementMap.insert( std::pair<const IdentifierHash , const InDetDD::SiDetectorElement*>(hashId, elt) );  
}

const InDetDD::SiDetectorElement* SiDetectorElementMap::readEntry(const IdentifierHash hashId) const
{
  for (auto it = m_detElementMap.begin(); it != m_detElementMap.end(); it++) {
    if(it->first==hashId) return it->second;
  }
  return 0;
}

