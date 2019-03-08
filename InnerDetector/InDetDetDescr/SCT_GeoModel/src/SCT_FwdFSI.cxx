/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdFSI.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/Units.h"


SCT_FwdFSI::SCT_FwdFSI(const std::string & name, int type,
                       InDetDD::SCT_DetectorManager* detectorManager,
                       const SCT_GeometryManager* geometryManager,
                       SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials),
    m_type(type)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_FwdFSI::getParameters()
{
  const SCT_ForwardParameters * parameters = m_geometryManager->forwardParameters();
        
  m_materialName= parameters->fwdFSIGeomMaterial(m_type);
  m_thickness   = parameters->fwdFSIGeomThickness(m_type);
  m_deltaR      = parameters->fwdFSIGeomDeltaR(m_type);
  m_rphi        = parameters->fwdFSIGeomRPhi(m_type);
  m_zOffset     = parameters->fwdFSIGeomZOffset(m_type);
}

GeoVPhysVol * 
SCT_FwdFSI::build() 
{
  const GeoBox * fsiShape = new GeoBox(0.5 * m_deltaR, 0.5 * m_rphi, 0.5 * m_thickness);
  m_material = m_materials->getMaterialForVolume(m_materialName, fsiShape->volume());
  const GeoLogVol * fsiLog = new GeoLogVol(getName(), fsiShape, m_material);

  GeoPhysVol * fsi = new GeoPhysVol(fsiLog);

  return fsi;
}


