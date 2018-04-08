/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "CLHEP/Units/SystemOfUnits.h"

SCT_InterLink::SCT_InterLink(const std::string & name)
  : SCT_SharedComponentFactory(name),
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

void 
SCT_InterLink::getParameters()
{
  const SCT_BarrelParameters * parameters = geometryManager()->barrelParameters();

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
 
  SCT_MaterialManager materials;

  if(m_nRepeat == 1) {
    // For old geometry interlink is a simple tube of material
    m_interLinkShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5*m_length);
    const GeoMaterial* material = materials.getMaterialForVolume(m_materialName, m_interLinkShape->volume());
    if(!material) {material = materials.getMaterial(m_materialName);}
    m_interLinkLog = new GeoLogVol(getName(), m_interLinkShape, material);
    m_interLink = new GeoPhysVol(m_interLinkLog);
  }

  else {
    // In updated version we make air tube containing interlink segments and B6 bearings
    // And also include FSI flange segments if defined
    // Air tube:
    m_interLinkShape = new GeoTube(m_innerRadius, m_outerRadiusBearing, 0.5*m_length);
    m_interLinkLog = new GeoLogVol(getName(), m_interLinkShape, materials.gasMaterial());
    m_interLink = new GeoPhysVol(m_interLinkLog);

    // Interlink segments:
    m_interLinkSegShape = new GeoTubs(m_innerRadius, m_outerRadius, 0.5*m_length, - 0.5*m_dPhi*CLHEP::radian, m_dPhi*CLHEP::radian);
    m_interLinkSegLog = new GeoLogVol("InterlinkSegment", m_interLinkSegShape, materials.getMaterialForVolume(m_materialName, m_interLinkSegShape->volume()));
    m_interLinkSeg = new GeoPhysVol(m_interLinkSegLog);

    for(int i=0; i<m_nRepeat; i++) {
      double interlinkAngle = m_phiPos + (i * 360./m_nRepeat)*CLHEP::deg;
      GeoTransform* geoTransform = new GeoTransform(HepGeom::RotateZ3D(interlinkAngle));
      m_geoTransforms.push_back(geoTransform);
      m_interLink->add(geoTransform);
      m_interLink->add(m_interLinkSeg);
    }

    // B6 bearings
    m_bearingShape = new GeoTubs(m_innerRadiusBearing, m_outerRadiusBearing, 0.5*m_lengthBearing, - 0.5*m_dPhiBearing*CLHEP::radian, m_dPhiBearing*CLHEP::radian);
    m_bearingLog = new GeoLogVol("Bearing", m_bearingShape, materials.getMaterialForVolume(m_materialNameBearing, m_bearingShape->volume()));
    m_bearing = new GeoPhysVol(m_bearingLog);

    for(int i=0; i<m_nRepeatBearing; i++) {
      double bearingAngle = m_phiPosBearing + (i * 360./m_nRepeatBearing)*CLHEP::deg;
      GeoTransform* geoTransform = new GeoTransform(HepGeom::RotateZ3D(bearingAngle));
      m_geoTransforms.push_back(geoTransform);
      m_interLink->add(geoTransform);
      m_interLink->add(m_bearing);
    }

    // FSI Flange segments:
    // These exactly fill gaps between interlink segments, with smaller radial extent
    if(m_includeFSIFlange) {
      double dPhiFSI = (360./m_nRepeat)*CLHEP::deg - m_dPhi; 
      m_FSIFlangeShape = new GeoTubs(m_innerRadiusFSIFlange, m_outerRadiusFSIFlange, 0.5*m_length, - 0.5*dPhiFSI*CLHEP::radian, dPhiFSI*CLHEP::radian);
      m_FSIFlangeLog = new GeoLogVol("FSIFlangeSegment", m_FSIFlangeShape, materials.getMaterialForVolume(m_materialNameFSIFlange, m_FSIFlangeShape->volume()));
      m_FSIFlange = new GeoPhysVol(m_FSIFlangeLog);

      for(int i=0; i<m_nRepeat; i++) {
	double phiPosFSI = m_phiPos + (180./m_nRepeat)*CLHEP::deg;
	double FSIFlangeAngle = phiPosFSI + (i * 360./m_nRepeat)*CLHEP::deg;
        GeoTransform* geoTransform = new GeoTransform(HepGeom::RotateZ3D(FSIFlangeAngle));
        m_geoTransforms.push_back(geoTransform);
	m_interLink->add(geoTransform);
	m_interLink->add(m_FSIFlange);
      }
    }
  }

  return m_interLink;
}

