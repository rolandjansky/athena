/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_Pigtail.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelModuleParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "CLHEP/Units/SystemOfUnits.h"

SCT_Pigtail::SCT_Pigtail(const std::string & name)
  : SCT_SharedComponentFactory(name)
{
  getParameters();
  m_physVolume = build();
}

void
SCT_Pigtail::getParameters() 
{
  const SCT_BarrelModuleParameters * parameters = geometryManager()->barrelModuleParameters();
  SCT_MaterialManager materials;
 
  m_material  = materials.getMaterial(parameters->pigtailMaterial());
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
