/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdPowerTape.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GaudiKernel/PhysicalConstants.h"

#include <cmath>

SCT_FwdPowerTape::SCT_FwdPowerTape(const std::string & name, 
                                   int numModules,
                                   double innerRadius, 
                                   double startPos, 
                                   double endPos,
                                   InDetDD::SCT_DetectorManager* detectorManager,
                                   const SCT_GeometryManager* geometryManager,
                                   SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials),
    m_numModules(numModules), m_innerRadius(innerRadius)
{
  m_length = std::abs(endPos - startPos);
  m_zPosition = 0.5 * (startPos + endPos);
 
  getParameters();
  m_physVolume = build();
}


void 
SCT_FwdPowerTape::getParameters()
{
  const SCT_ForwardParameters * parameters = m_geometryManager->forwardParameters();
  m_material  = m_materials->getMaterial(parameters->fwdPowerTapeMaterial());
  m_crossSectArea = parameters->fwdPowerTapeCrossSectArea();
}

GeoVPhysVol * 
SCT_FwdPowerTape::build() 
{
  // Calculate the dimensions.
  // The area = 2*Gaudi::Units::pi*r_ave*delta_r approx= 2 * Gaudi::Units::pi * rMin * delta_r
  // where m_thickness = delta_r

  double area = m_crossSectArea * m_numModules;
  m_thickness = area/(2. * Gaudi::Units::pi * m_innerRadius);
  m_outerRadius = m_innerRadius +  m_thickness;

  // Make the support disk. A simple tube.
  const GeoTube * powerTapeShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_length);
  const GeoLogVol * powerTapeLog = new GeoLogVol(getName(), powerTapeShape, m_material);

  GeoPhysVol * powerTape = new GeoPhysVol(powerTapeLog);

  return powerTape;
}
