/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdModuleConnector.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardModuleParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>

SCT_FwdModuleConnector::SCT_FwdModuleConnector(const std::string & name, int ringType,
                                               InDetDD::SCT_DetectorManager* detectorManager,
                                               const SCT_GeometryManager* geometryManager,
                                               SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials),
    m_ringType(ringType)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_FwdModuleConnector::getParameters()
{
  const SCT_ForwardModuleParameters * parameters = m_geometryManager->forwardModuleParameters();

  m_materialName= parameters->fwdModuleConnectorMaterial();
  m_deltaR = parameters->fwdModuleConnectorDeltaR();
  m_rphi = parameters->fwdModuleConnectorRPhi();
  m_thickness   = parameters->fwdModuleConnectorThickness();
}

GeoVPhysVol * 
SCT_FwdModuleConnector::build() 
{
  const SCT_ForwardModuleParameters * parameters = m_geometryManager->forwardModuleParameters();

  // Construct box
  const GeoBox * moduleConnShape = new GeoBox(0.5 * m_thickness, 0.5 * m_rphi, 0.5 * m_deltaR);
  m_material = m_materials->getMaterialForVolume(m_materialName, moduleConnShape->volume());

  // Shift to correct position within module
  double xposition = 0.5 * (parameters->fwdHybridThickness() + m_thickness);
  double zposition = parameters->fwdModuleMountPoint(m_ringType) - parameters->fwdHybridMountPointToInnerEdge() + parameters->fwdHybridLength() - 0.5*(m_deltaR);
  if (m_ringType == 0) { zposition = -1 * zposition;  };  // outer module, hybrid in inner side
  const GeoShape & connectorPos = (*moduleConnShape << GeoTrf::Translate3D(xposition,0.,zposition));


  const GeoLogVol * moduleConnLog = new GeoLogVol(getName(), &connectorPos, m_material);

  GeoPhysVol * moduleConnector = new GeoPhysVol(moduleConnLog);

  return moduleConnector;
}
