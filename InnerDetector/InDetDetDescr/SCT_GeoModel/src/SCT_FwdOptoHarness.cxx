/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdOptoHarness.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>
#include <iostream>

inline double sqr(double x) {return x*x;}

SCT_FwdOptoHarness::SCT_FwdOptoHarness(const std::string & name, int itype,
                                       InDetDD::SCT_DetectorManager* detectorManager,
                                       const SCT_GeometryManager* geometryManager,
                                       SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials),
    m_type(itype)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_FwdOptoHarness::getParameters()
{
  const SCT_ForwardParameters * parameters = m_geometryManager->forwardParameters();

  m_index = -1;
  for(int i=0; i<3; ++i) {
    if(parameters->fwdOptoHarnessDiscType(i) == m_type) {m_index = i;}
  }

  if(m_index < 0) {
    std::cout << "SCT_FwdOptoHarness: ERROR. Disc type is missing" << m_type << std::endl; 
    std::cout << "Assuming type 1" << std::endl; 
    m_index = 0;
  }
    
  m_materialName= parameters->fwdOptoHarnessMaterial(m_index);
  m_innerRadius = parameters->fwdOptoHarnessInnerRadius(m_index);
  m_outerRadius = parameters->fwdOptoHarnessOuterRadius(m_index);
  m_thickness   = parameters->fwdOptoHarnessThickness(m_index);
}

GeoVPhysVol * 
SCT_FwdOptoHarness::build() 
{
  const GeoTube * optoHarnessShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_thickness);
  m_material = m_materials->getMaterialForVolume(m_materialName, optoHarnessShape->volume());
  const GeoLogVol * optoHarnessLog = new GeoLogVol(getName(), optoHarnessShape, m_material);

  GeoPhysVol * optoHarness = new GeoPhysVol(optoHarnessLog);

  return optoHarness;
}
