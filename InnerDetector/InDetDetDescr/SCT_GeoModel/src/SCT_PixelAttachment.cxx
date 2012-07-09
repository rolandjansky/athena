/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_PixelAttachment.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "CLHEP/Units/SystemOfUnits.h"

// This is the SCT to Pixel attachment. This was originally defined in the 
// general indet services but it is in the SCT mother volume.

SCT_PixelAttachment::SCT_PixelAttachment(const std::string & name)
  : SCT_SharedComponentFactory(name)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_PixelAttachment::getParameters()
{
  const SCT_BarrelParameters * parameters = geometryManager()->barrelParameters();
  SCT_MaterialManager materials;
    
  m_material    = materials.getMaterial(parameters->pixelAttachmentMaterial());
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
