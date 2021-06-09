/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "HGTD_GeoModel/HGTD_GeoModelAthenaComps.h"

HGTD_GeoModelAthenaComps::HGTD_GeoModelAthenaComps()
  : InDetDD::AthenaComps("HGTD_GeoModel"),
    m_idHelper(nullptr)
{}
 
void 
HGTD_GeoModelAthenaComps::setIdHelper(const HGTD_ID* idHelper)
{
  m_idHelper = idHelper;
}

const HGTD_ID* 
HGTD_GeoModelAthenaComps::getIdHelper() const
{
  return m_idHelper;
}
