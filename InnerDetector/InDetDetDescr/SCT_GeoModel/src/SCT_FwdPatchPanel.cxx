/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdPatchPanel.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/Units.h"


SCT_FwdPatchPanel::SCT_FwdPatchPanel(const std::string & name, int type,
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
SCT_FwdPatchPanel::getParameters()
{
  const SCT_ForwardParameters * parameters = m_geometryManager->forwardParameters();
        
  m_materialName= parameters->fwdPatchPanelMaterial(m_type);
  m_thickness   = parameters->fwdPatchPanelThickness(m_type);
  m_midRadius   = parameters->fwdPatchPanelMidRadius(m_type);
  m_deltaR      = parameters->fwdPatchPanelDeltaR(m_type);
  m_rphi        = parameters->fwdPatchPanelRPhi(m_type);
}

GeoVPhysVol * 
SCT_FwdPatchPanel::build() 
{
  const GeoBox * patchPanelShape = new GeoBox(0.5 * m_deltaR, 0.5 * m_rphi, 0.5 * m_thickness);
  m_material = m_materials->getMaterialForVolume(m_materialName, patchPanelShape->volume());
  const GeoLogVol * patchPanelLog = new GeoLogVol(getName(), patchPanelShape, m_material);


  GeoPhysVol * patchPanel = new GeoPhysVol(patchPanelLog);

  return patchPanel;
}


