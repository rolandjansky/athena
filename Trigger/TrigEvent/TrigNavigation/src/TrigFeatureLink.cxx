/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNavigation/TrigFeatureLink.h"

TrigFeatureLink::TrigFeatureLink(CLID clid, uint16_t subtype, uint32_t index) 
  : m_clid(clid),
    m_subTypeIndex(subtype),
    m_index(index) 
{}

TrigFeatureLink::TrigFeatureLink() 
  : m_clid(0),
    m_subTypeIndex(0),
    m_index(0) 
{}


