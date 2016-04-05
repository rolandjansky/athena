/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/PixelGeometryManager.h"

using namespace std;

PixelGeometryManager::PixelGeometryManager(const PixelGeoModelAthenaComps * athenaComps) :
  m_athenaComps(athenaComps)
{}

PixelGeometryManager::~PixelGeometryManager()
{}

// Default implementation return 0.
PixelLegacyManager * PixelGeometryManager::legacyManager() const
{
  return 0;
}


