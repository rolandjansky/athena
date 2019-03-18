/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_PixelAttachment.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/Units.h"

// This is the SCT to Pixel attachment. This was originally defined in the 
// general indet services but it is in the SCT mother volume.

SCT_PixelAttachment::SCT_PixelAttachment(const std::string & name,
                                         InDetDD::SCT_DetectorManager* detectorManager,
                                         const SCT_GeometryManager* geometryManager,
                                         SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_PixelAttachment::getParameters()
{
  const SCT_BarrelParameters * parameters = m_geometryManager->barrelParameters();
  m_material    = m_materials->getMaterial(parameters->pixelAttachmentMaterial());
  m_length      = parameters->pixelAttachmentDeltaZ();
  m_innerRadius = parameters->pixelAttachmentInnerRadius();
  m_outerRadius = parameters->pixelAttachmentOuterRadius();
  m_zMin        = parameters->pixelAttachmentZMin(); 
  m_zMax        = m_zMin + m_length; 
}

GeoVPhysVol * 
SCT_PixelAttachment::build() 
{
  // Make the SCT to Pixel attachment. A simple tube.
  const GeoTube * shape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_length);
  const GeoLogVol * log = new GeoLogVol(getName(), shape, m_material);

  GeoPhysVol * attachment = new GeoPhysVol(log);

  return attachment;
}
