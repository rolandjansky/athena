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
  : SCT_SharedComponentFactory(name)
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
  GeoPhysVol* interLink;

  if(m_nRepeat == 1) {
    // For old geometry interlink is a simple tube of material
    const GeoTube* interLinkShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5*m_length);
    const GeoMaterial* material = materials.getMaterialForVolume(m_materialName, interLinkShape->volume());
    if(!material) {material = materials.getMaterial(m_materialName);}
    const GeoLogVol* interLinkLog = new GeoLogVol(getName(), interLinkShape, material);
    interLink = new GeoPhysVol(interLinkLog);
  }

  else {
    // In updated version we make air tube containing interlink segments and B6 bearings
    // And also include FSI flange segments if defined
    // Air tube:
    const GeoTube* interLinkShape = new GeoTube(m_innerRadius, m_outerRadiusBearing, 0.5*m_length);
    const GeoLogVol* interLinkLog = new GeoLogVol(getName(), interLinkShape, materials.gasMaterial());
    interLink = new GeoPhysVol(interLinkLog);

    // Interlink segments:
    const GeoTubs* interLinkSegShape = new GeoTubs(m_innerRadius, m_outerRadius, 0.5*m_length, - 0.5*m_dPhi*CLHEP::radian, m_dPhi*CLHEP::radian);
    const GeoLogVol* interLinkSegLog = new GeoLogVol("InterlinkSegment", interLinkSegShape, materials.getMaterialForVolume(m_materialName, interLinkSegShape->volume()));
    GeoPhysVol* interLinkSeg = new GeoPhysVol(interLinkSegLog);

    for(int i=0; i<m_nRepeat; i++) {
      double interlinkAngle = m_phiPos + (i * 360./m_nRepeat)*CLHEP::deg;
      interLink->add(new GeoTransform(HepGeom::RotateZ3D(interlinkAngle)));
      interLink->add(interLinkSeg);
    }

    // B6 bearings
    const GeoTubs* bearingShape = new GeoTubs(m_innerRadiusBearing, m_outerRadiusBearing, 0.5*m_lengthBearing, - 0.5*m_dPhiBearing*CLHEP::radian, m_dPhiBearing*CLHEP::radian);
    const GeoLogVol* bearingLog = new GeoLogVol("Bearing", bearingShape, materials.getMaterialForVolume(m_materialNameBearing, bearingShape->volume()));
    GeoPhysVol* bearing = new GeoPhysVol(bearingLog);

    for(int i=0; i<m_nRepeatBearing; i++) {
      double bearingAngle = m_phiPosBearing + (i * 360./m_nRepeatBearing)*CLHEP::deg;
      interLink->add(new GeoTransform(HepGeom::RotateZ3D(bearingAngle)));
      interLink->add(bearing);
    }

    // FSI Flange segments:
    // These exactly fill gaps between interlink segments, with smaller radial extent
    if(m_includeFSIFlange) {
      double dPhiFSI = (360./m_nRepeat)*CLHEP::deg - m_dPhi; 
      const GeoTubs* FSIFlangeShape = new GeoTubs(m_innerRadiusFSIFlange, m_outerRadiusFSIFlange, 0.5*m_length, - 0.5*dPhiFSI*CLHEP::radian, dPhiFSI*CLHEP::radian);
      const GeoLogVol* FSIFlangeLog = new GeoLogVol("FSIFlangeSegment", FSIFlangeShape, materials.getMaterialForVolume(m_materialNameFSIFlange, FSIFlangeShape->volume()));
      GeoPhysVol* FSIFlange = new GeoPhysVol(FSIFlangeLog);

      for(int i=0; i<m_nRepeat; i++) {
	double phiPosFSI = m_phiPos + (180./m_nRepeat)*CLHEP::deg;
	double FSIFlangeAngle = phiPosFSI + (i * 360./m_nRepeat)*CLHEP::deg;
	interLink->add(new GeoTransform(HepGeom::RotateZ3D(FSIFlangeAngle)));
	interLink->add(FSIFlange);
      }
    }
  }

  return interLink;
}

