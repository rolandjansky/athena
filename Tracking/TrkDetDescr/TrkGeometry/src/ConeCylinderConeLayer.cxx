/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ConeCylinderConeLayer.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkGeometry/ConeCylinderConeLayer.h"

#include<iostream>

Trk::ConeCylinderConeLayer::ConeCylinderConeLayer(Amg::Transform3D* htrans,
						  double radius,
						  double halfLength,
						  double zStitch,
						  double alpha,
						  SurfaceArray* surfaceArray,
						  const LayerMaterialProperties& laymatprop,
						  double thickness,
						  OverlapDescriptor* od,
						  int laytyp) :
  // set up the encompassing cylinder (and most of the layer properties)
  CylinderLayer(htrans, new CylinderBounds(radius, halfLength), surfaceArray, laymatprop, thickness, od, 0, laytyp),
  m_transform(htrans),
  m_radius   (radius),	        
  m_halfLength(halfLength),   
  m_zStitch  (zStitch),	        
  m_alpha    (alpha)
{
  // now create the real surfaces
  double rTanAlpha = radius/tan(alpha);
  // left cone
  Amg::Transform3D* leftConeTransform = new Amg::Transform3D;
  (*leftConeTransform) = (*htrans)*Amg::Translation3D(0,0,-zStitch-rTanAlpha);
  m_surfaces.push_back(new ConeSurface(leftConeTransform, alpha,rTanAlpha-(halfLength-zStitch),rTanAlpha));
  // center cylinder
  m_surfaces.push_back(new CylinderSurface(new Amg::Transform3D(*htrans), radius, zStitch));
  // right cone
  Amg::Transform3D* rightConeTransform = new Amg::Transform3D;
  (*rightConeTransform) = (*htrans)*Amg::Translation3D(0,0,zStitch+rTanAlpha);
  m_surfaces.push_back(new ConeSurface(rightConeTransform, alpha,rTanAlpha-(halfLength-zStitch),rTanAlpha));
}
