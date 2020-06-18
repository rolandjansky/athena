/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/InDetServMatAthenaComps.h"
#include "InDetGeoModelUtils/IInDetServMatBuilderTool.h"

InDetServMatAthenaComps::InDetServMatAthenaComps()
  : InDetDD::AthenaComps("InDetServMatGeoModel"),
    m_builderTool(0)
{}

 
void 
InDetServMatAthenaComps::setBuilderTool(IInDetServMatBuilderTool * builderTool) 
{
  m_builderTool = builderTool;
}

const IInDetServMatBuilderTool * 
InDetServMatAthenaComps::builderTool() const
{
  return m_builderTool;
}
