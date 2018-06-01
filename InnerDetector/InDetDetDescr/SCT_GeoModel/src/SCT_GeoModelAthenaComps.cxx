/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_GeoModelAthenaComps.h"
#include "InDetCondServices/ISiLorentzAngleSvc.h"

SCT_GeoModelAthenaComps::SCT_GeoModelAthenaComps()
  : InDetDD::AthenaComps("SCT_GeoModel"),
    m_lorentzAngleTool(nullptr),
    m_idHelper(0)
{}
 
void 
SCT_GeoModelAthenaComps::setLorentzAngleTool(const ISiLorentzAngleTool* lorentzAngleTool)
{
  m_lorentzAngleTool = lorentzAngleTool;
}

void 
SCT_GeoModelAthenaComps::setIdHelper(const SCT_ID* idHelper)
{
  m_idHelper = idHelper;
}

const ISiLorentzAngleTool*
SCT_GeoModelAthenaComps::lorentzAngleTool() const
{
  return m_lorentzAngleTool;
}

const SCT_ID* 
SCT_GeoModelAthenaComps::getIdHelper() const
{
  return m_idHelper;
}

