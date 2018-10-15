/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_GeoModelAthenaComps.h"

namespace InDetDDSLHC {

SCT_GeoModelAthenaComps::SCT_GeoModelAthenaComps()
  : InDetDD::AthenaComps("SCT_SLHC_GeoModel"),
    m_serviceBuilderTool(0),
    m_idHelper(0)
{}

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

const SCT_ID* 
SCT_GeoModelAthenaComps::getIdHelper() const
{
  return m_idHelper;
}

}
