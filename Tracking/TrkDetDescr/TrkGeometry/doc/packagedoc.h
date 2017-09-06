/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkGeometry_page TrkGeometry Package

The TrkGeometry package combines the geometrical classes from the TrkSurfaces and TrkVolumes
expressed through the Surface and Volume classes with material and magnetic field information 
to its physical representation Trk::Layer respectively Trk::TrackingVolume.

@author Andreas.Salzburger@cern.ch

@section TrkGeometry_TrkGeometryIntro Introduction

The Trk::TrackingGeometry class is holder class of the highest Trk::TrackingVolume declared either 
through jobOptions or built through the GeometryBuilder from the TrkDetDescrTools package.
It enables a global search for an associated Trk::TrackingVolume/Trk::Layer such as the continous navigation
via BoundarySurfaces between the various TrackingVolumes.

@section TrkGeometry_TrkGeometryClass Class Overview
The TrkGeometry package contains following classes:
  - Trk::Layer: Abstract Base class for a layer description
      - CylinderLayer: cylindrical implementation, extends Layer and CylinderSurface
      - DiscLayer: disc-like implementation, extends Layer and DiscSurface
      - PlaneLayer: planar implementation, extends Layer and PlaneSurface
  - Trk::LayerMaterialProperties: material description for a Layer that can be multi-layered
  - Trk::MaterialProperties: material descriptions for TrackingVolume
  - Trk::MagneticFieldProperties: magnetic field descriptions for TrackingVolume an Layer
  - Trk::TrackingVolume: extends the Volume class from the TrkVolume package with pysical properties
  - Trk::TrackingGeometry: holder class of the highest TrackingVolume
   


*/
