/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModelAthenaComps.h"
#include "CxxUtils/checker_macros.h"

PixelGeoModelAthenaComps::PixelGeoModelAthenaComps()
  : InDetDD::AthenaComps("PixelGeoModel"),
    m_bcmTool(0),
    m_blmTool(0),
    m_serviceBuilderTool(0),
    m_idHelper(0)
{}

void 
PixelGeoModelAthenaComps::setBCM(IGeoSubDetTool * bcmTool) 
{
  m_bcmTool = bcmTool;
}

IGeoSubDetTool * 
PixelGeoModelAthenaComps::bcm ATLAS_NOT_THREAD_SAFE () const // const method returns non-const pointer.
{
  return m_bcmTool;
}

void 
PixelGeoModelAthenaComps::setBLM(IGeoSubDetTool * blmTool) 
{
  m_blmTool = blmTool;
}

IGeoSubDetTool * 
PixelGeoModelAthenaComps::blm ATLAS_NOT_THREAD_SAFE () const // const method returns non-const pointer.
{
  return m_blmTool;
}

void 
PixelGeoModelAthenaComps::setServiceBuilderTool(IInDetServMatBuilderTool * serviceBuilderTool) 
{
  m_serviceBuilderTool = serviceBuilderTool;
}

IInDetServMatBuilderTool * 
PixelGeoModelAthenaComps::serviceBuilderTool ATLAS_NOT_THREAD_SAFE () const // const method returns non-const pointer.
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
