/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/PixelGeoModelAthenaComps.h"
#include "InDetCondServices/ISiLorentzAngleSvc.h"

PixelGeoModelAthenaComps::PixelGeoModelAthenaComps()
  : InDetDD::AthenaComps("PixelGeoModel"),
    m_lorentzAngleSvc("",""),
    m_bcmTool(0),
    m_blmTool(0),
    m_serviceBuilderTool(0),
    m_idHelper(0)
{}

 
void 
PixelGeoModelAthenaComps::setLorentzAngleSvc(const ServiceHandle<ISiLorentzAngleSvc> & lorentzAngleSvc)
{
  m_lorentzAngleSvc = lorentzAngleSvc;
}

const ServiceHandle<ISiLorentzAngleSvc> & 
PixelGeoModelAthenaComps::lorentzAngleSvc() const
{
  return m_lorentzAngleSvc;
}

void 
PixelGeoModelAthenaComps::setBCM(IGeoSubDetTool * bcmTool) 
{
  m_bcmTool = bcmTool;
}

IGeoSubDetTool * 
PixelGeoModelAthenaComps::bcm() const 
{
  return m_bcmTool;
}

void 
PixelGeoModelAthenaComps::setBLM(IGeoSubDetTool * blmTool) 
{
  m_blmTool = blmTool;
}

IGeoSubDetTool * 
PixelGeoModelAthenaComps::blm() const 
{
  return m_blmTool;
}

void 
PixelGeoModelAthenaComps::setServiceBuilderTool(IInDetServMatBuilderTool * serviceBuilderTool) 
{
  m_serviceBuilderTool = serviceBuilderTool;
}

IInDetServMatBuilderTool * 
PixelGeoModelAthenaComps::serviceBuilderTool() const 
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
