/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_ComponentFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <sstream>
#include <string>

using InDetDD::SCT_DetectorManager;

namespace InDetDDSLHC {

SCT_DetectorManager * SCT_ComponentFactory::s_detectorManager = 0;
const SCT_GeometryManager * SCT_ComponentFactory::s_geometryManager = 0;

double SCT_ComponentFactory::s_epsilon = 1.0e-6 * Gaudi::Units::mm;

SCT_ComponentFactory::SCT_ComponentFactory(const std::string & name) 
  : m_name(name)
{}

SCT_ComponentFactory::~SCT_ComponentFactory() 
{}


void 
SCT_ComponentFactory::setDetectorManager(SCT_DetectorManager * manager)
{
  s_detectorManager = manager;
}

void 
SCT_ComponentFactory::setGeometryManager(SCT_GeometryManager * manager)
{
  s_geometryManager = manager;
}

std::string 
SCT_ComponentFactory::intToString(int i)
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
}
