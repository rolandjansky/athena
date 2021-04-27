/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_GeoModelAthenaComps.h"

SCT_GeoModelAthenaComps::SCT_GeoModelAthenaComps()
  : InDetDD::AthenaComps("SCT_GeoModel"),
    m_idHelper(nullptr)
{}
 
void 
SCT_GeoModelAthenaComps::setIdHelper(const SCT_ID* idHelper)
{
  m_idHelper = idHelper;
}

const SCT_ID* 
SCT_GeoModelAthenaComps::getIdHelper() const
{
  return m_idHelper;
}

