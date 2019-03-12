/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_Pigtail.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelModuleParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/Units.h"

SCT_Pigtail::SCT_Pigtail(const std::string & name,
                         InDetDD::SCT_DetectorManager* detectorManager,
                         const SCT_GeometryManager* geometryManager,
                         SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_physVolume = build();
}

void
SCT_Pigtail::getParameters() 
{
  const SCT_BarrelModuleParameters * parameters = m_geometryManager->barrelModuleParameters();
  m_material  = m_materials->getMaterial(parameters->pigtailMaterial());
  m_thickness = parameters->pigtailThickness();
  m_length    = parameters->pigtailLength();
  m_width     = parameters->pigtailWidth();
}

GeoVPhysVol * 
SCT_Pigtail::build()
{
  
  // Build the Pigtail. Just a simple box.
  const GeoBox * pigtailShape = new GeoBox(0.5*m_thickness,
                                           0.5*m_width,
                                           0.5*m_length);
  const GeoLogVol * pigtailLog = 
    new GeoLogVol(getName(), pigtailShape, m_material);
  GeoPhysVol * pigtail = new GeoPhysVol(pigtailLog);

  return pigtail;
}
