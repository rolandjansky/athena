/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModelAthenaComps.h"
#include "CxxUtils/checker_macros.h"

PixelGeoModelAthenaComps::PixelGeoModelAthenaComps()
  : InDetDD::AthenaComps("PixelGeoModel"),
    m_bcmTool(nullptr),
    m_blmTool(nullptr),
    m_serviceBuilderTool(nullptr),
    m_idHelper(nullptr)
{}

void 
PixelGeoModelAthenaComps::setBCM(IGeoSubDetTool * bcmTool) 
{
  m_bcmTool = bcmTool;
}

IGeoSubDetTool * 
PixelGeoModelAthenaComps::bcm()
{
  return m_bcmTool;
}

void 
PixelGeoModelAthenaComps::setBLM(IGeoSubDetTool * blmTool) 
{
  m_blmTool = blmTool;
}

IGeoSubDetTool * 
PixelGeoModelAthenaComps::blm()
{
  return m_blmTool;
}

void 
PixelGeoModelAthenaComps::setServiceBuilderTool(IInDetServMatBuilderTool * serviceBuilderTool) 
{
  m_serviceBuilderTool = serviceBuilderTool;
}

IInDetServMatBuilderTool * 
PixelGeoModelAthenaComps::serviceBuilderTool()
{
  return m_serviceBuilderTool;
}

void
PixelGeoModelAthenaComps::setIdHelper(const PixelID* idHelper)
{
  m_idHelper = idHelper;
}

const PixelID* 
PixelGeoModelAthenaComps::getIdHelper() const
{
  return m_idHelper;
}
