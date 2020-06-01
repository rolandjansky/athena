/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"

AtlasFieldCacheCondObj::AtlasFieldCacheCondObj()
{}

AtlasFieldCacheCondObj::~AtlasFieldCacheCondObj() {}


bool
AtlasFieldCacheCondObj::initialize(double solFieldScale,
                                   double torFieldScale,
                                   const MagField::AtlasFieldMap* fieldMap)
{
    // temporary flag
    m_solFieldScale = solFieldScale;
    m_torFieldScale = torFieldScale;
    m_fieldMap      = fieldMap;
    return (m_fieldMap) != nullptr; // return false if cast failed
}

