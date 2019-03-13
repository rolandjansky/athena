/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_ComponentFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <sstream>
#include <string>

using InDetDD::SCT_DetectorManager;

const double SCT_ComponentFactory::s_epsilon = 1.0e-6 * Gaudi::Units::mm;

SCT_ComponentFactory::SCT_ComponentFactory(const std::string & name,
                                           InDetDD::SCT_DetectorManager* detectorManager,
                                           const SCT_GeometryManager* geometryManager,
                                           SCT_MaterialManager* materials)
  : m_detectorManager(detectorManager), 
    m_geometryManager(geometryManager),
    m_materials(materials),
    m_name(name)
{}

SCT_ComponentFactory::~SCT_ComponentFactory() 
{}

std::string 
SCT_ComponentFactory::intToString(int i) const
{
  std::ostringstream str;
  str << i;
  return str.str();
}

double
SCT_ComponentFactory::epsilon() const
{
  return s_epsilon;
}
