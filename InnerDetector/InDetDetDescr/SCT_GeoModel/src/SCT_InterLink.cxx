/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_InterLink.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GaudiKernel/SystemOfUnits.h"

SCT_InterLink::SCT_InterLink(const std::string & name,
                             InDetDD::SCT_DetectorManager* detectorManager,
                             const SCT_GeometryManager* geometryManager,
                             SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials),
    m_interLinkShape(nullptr),
    m_interLinkLog(nullptr),
    m_interLink(nullptr),
    m_interLinkSegShape(nullptr),
    m_interLinkSegLog(nullptr),
    m_interLinkSeg(nullptr),
    m_bearingShape(nullptr),
    m_bearingLog(nullptr),
    m_bearing(nullptr),
    m_FSIFlangeShape(nullptr),
    m_FSIFlangeLog(nullptr),
    m_FSIFlange(nullptr)
{
  getParameters();
  m_physVolume = build();
}

SCT_InterLink::~SCT_InterLink() {
  if (m_interLink) m_interLink->unref();
  if (m_interLinkSeg) m_interLinkSeg->unref();
  if (m_bearing) m_bearing->unref();
  if (m_FSIFlange) m_FSIFlange->unref();
}

void 
SCT_InterLink::getParameters()
{
  const SCT_BarrelParameters * parameters = m_geometryManager->barrelParameters();

  // Interlink parameters   
  m_materialName  = parameters->interLinkMaterial();
  m_length      = parameters->interLinkDeltaZ();
  m_innerRadius = parameters->interLinkInnerRadius();
  m_outerRadius = parameters->interLinkOuterRadius();
  m_dPhi        = parameters->interLinkDeltaPhi();
  m_phiPos      = parameters->interLinkPhiPos();
  m_nRepeat     = parameters->interLinkNRepeat();

  // FSIFlange parameters: only for non-uniform interlink
  if(m_nRepeat > 1) {
    m_includeFSIFlange = parameters->includeFSI();
  }
  else {
    m_includeFSIFlange = false;
  }
  if(m_includeFSIFlange) {
    m_innerRadiusFSIFlange = parameters->fsiFlangeInnerRadius();
    m_outerRadiusFSIFlange = parameters->fsiFlangeOuterRadius();
    m_materialNameFSIFlange  = parameters->fsiFlangeMaterial();
  }

  // B6 bearing parameters: only for non-uniform interlink
  if(m_nRepeat > 1) {
    m_materialNameBearing  = parameters->bearingMaterial();
    m_lengthBearing      = m_length;
    m_innerRadiusBearing = m_outerRadius;
    m_outerRadiusBearing = parameters->thermalShieldBulkheadInnerRadius();
    m_dPhiBearing        = parameters->bearingDeltaPhi();
    m_phiPosBearing      = parameters->bearingPhiPos();
    m_nRepeatBearing     = parameters->bearingNRepeat();
  }

}

GeoVPhysVol * 
SCT_InterLink::build() 
{
  // Make the interlink.
  if(m_nRepeat == 1) {
    // For old geometry interlink is a simple tube of material
    m_interLinkShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5*m_length);
    const GeoMaterial* material = m_materials->getMaterialForVolume(m_materialName, m_interLinkShape->volume());
    if(!material) {material = m_materials->getMaterial(m_materialName);}
    m_interLinkLog = new GeoLogVol(getName(), m_interLinkShape, material);
    m_interLink = new GeoPhysVol(m_interLinkLog);
    m_interLink->ref();
  }

  else {
    // In updated version we make air tube containing interlink segments and B6 bearings
    // And also include FSI flange segments if defined
    // Air tube:
    m_interLinkShape = new GeoTube(m_innerRadius, m_outerRadiusBearing, 0.5*m_length);
    m_interLinkLog = new GeoLogVol(getName(), m_interLinkShape, m_materials->gasMaterial());
    m_interLink = new GeoPhysVol(m_interLinkLog);
    m_interLink->ref();

    // Interlink segments:
    m_interLinkSegShape = new GeoTubs(m_innerRadius, m_outerRadius, 0.5*m_length, - 0.5*m_dPhi*Gaudi::Units::radian, m_dPhi*Gaudi::Units::radian);
    m_interLinkSegLog = new GeoLogVol("InterlinkSegment", m_interLinkSegShape, m_materials->getMaterialForVolume(m_materialName, m_interLinkSegShape->volume()));
    m_interLinkSeg = new GeoPhysVol(m_interLinkSegLog);
    m_interLinkSeg->ref();

    for(int i=0; i<m_nRepeat; i++) {
      double interlinkAngle = m_phiPos + (i * 360./m_nRepeat)*Gaudi::Units::deg;
      GeoTransform* geoTransform = new GeoTransform(GeoTrf::RotateZ3D(interlinkAngle));
      m_geoTransforms.push_back(geoTransform);
      m_interLink->add(geoTransform);
      m_interLink->add(m_interLinkSeg);
    }

    // B6 bearings
    m_bearingShape = new GeoTubs(m_innerRadiusBearing, m_outerRadiusBearing, 0.5*m_lengthBearing, - 0.5*m_dPhiBearing*Gaudi::Units::radian, m_dPhiBearing*Gaudi::Units::radian);
    m_bearingLog = new GeoLogVol("Bearing", m_bearingShape, m_materials->getMaterialForVolume(m_materialNameBearing, m_bearingShape->volume()));
    m_bearing = new GeoPhysVol(m_bearingLog);
    m_bearing->ref();

    for(int i=0; i<m_nRepeatBearing; i++) {
      double bearingAngle = m_phiPosBearing + (i * 360./m_nRepeatBearing)*Gaudi::Units::deg;
      GeoTransform* geoTransform = new GeoTransform(GeoTrf::RotateZ3D(bearingAngle));
      m_geoTransforms.push_back(geoTransform);
      m_interLink->add(geoTransform);
      m_interLink->add(m_bearing);
    }

    // FSI Flange segments:
    // These exactly fill gaps between interlink segments, with smaller radial extent
    if(m_includeFSIFlange) {
      double dPhiFSI = (360./m_nRepeat)*Gaudi::Units::deg - m_dPhi; 
      m_FSIFlangeShape = new GeoTubs(m_innerRadiusFSIFlange, m_outerRadiusFSIFlange, 0.5*m_length, - 0.5*dPhiFSI*Gaudi::Units::radian, dPhiFSI*Gaudi::Units::radian);
      m_FSIFlangeLog = new GeoLogVol("FSIFlangeSegment", m_FSIFlangeShape, m_materials->getMaterialForVolume(m_materialNameFSIFlange, m_FSIFlangeShape->volume()));
      m_FSIFlange = new GeoPhysVol(m_FSIFlangeLog);
      m_FSIFlange->ref();

      for(int i=0; i<m_nRepeat; i++) {
        double phiPosFSI = m_phiPos + (180./m_nRepeat)*Gaudi::Units::deg;
        double FSIFlangeAngle = phiPosFSI + (i * 360./m_nRepeat)*Gaudi::Units::deg;
        GeoTransform* geoTransform = new GeoTransform(GeoTrf::RotateZ3D(FSIFlangeAngle));
        m_geoTransforms.push_back(geoTransform);
        m_interLink->add(geoTransform);
        m_interLink->add(m_FSIFlange);
      }
    }
  }

  return m_interLink;
}

