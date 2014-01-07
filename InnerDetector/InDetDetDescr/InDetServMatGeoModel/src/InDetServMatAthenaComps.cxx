/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

IInDetServMatBuilderTool * 
InDetServMatAthenaComps::builderTool() const 
{
  return m_builderTool;
}
