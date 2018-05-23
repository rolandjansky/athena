/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_GeoModelAthenaComps.h"
#include "InDetCondServices/ISiLorentzAngleTool.h"

namespace InDetDDSLHC {

SCT_GeoModelAthenaComps::SCT_GeoModelAthenaComps()
  : InDetDD::AthenaComps("SCT_SLHC_GeoModel"),
    m_lorentzAngleTool(nullptr),
    m_serviceBuilderTool(0),
    m_idHelper(0)
{}
 
void 
SCT_GeoModelAthenaComps::setLorentzAngleTool(const ISiLorentzAngleTool* lorentzAngleTool)
{
  m_lorentzAngleTool = lorentzAngleTool;
}

void 
SCT_GeoModelAthenaComps::setServiceBuilderTool(IInDetServMatBuilderTool * serviceBuilderTool) 
{
  m_serviceBuilderTool = serviceBuilderTool;
}

IInDetServMatBuilderTool * 
SCT_GeoModelAthenaComps::serviceBuilderTool() const 
{
  return m_serviceBuilderTool;
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

}
