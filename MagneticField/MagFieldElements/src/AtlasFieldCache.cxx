/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// AtlasFieldCache.cxx
//
// Local cache for magnetic field (based on MagFieldServices/AtlasFieldSvcTLS.h)
//
// R.D.Schaffer -at- cern.ch
//
#include "MagFieldElements/AtlasFieldCache.h"

/// Constructor
MagField::AtlasFieldCache::AtlasFieldCache() = default;

MagField::AtlasFieldCache::AtlasFieldCache(double solFieldScale,
                                           double torFieldScale,
                                           const AtlasFieldMap* fieldMap)
  :
    m_solScale(solFieldScale),
    m_torScale(torFieldScale),
    // set field service
    m_fieldMap(fieldMap)

{
    if (m_fieldMap) {
        // save ZR bfield
        m_meshZR = m_fieldMap->getBFieldMesh() ;
        // Get solenoid zone id from field service
        m_solZoneId = fieldMap->solenoidZoneId();
    }
}
