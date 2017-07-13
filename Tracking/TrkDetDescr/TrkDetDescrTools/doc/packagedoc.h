/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkDetDescrTools_page TrkDetDescrTools Package

This Package contains the Trk::GeometryBuilder for the ATLAS Tracking Geometry
It also contains many Tools that are given throuth interfaces in the TrkDetDescrInterfaces
package.

@author Andreas.Salzburger@cern.ch

@section TrkDetDescrTools_TrkDetDescrToolsIntro Introduction

   - Geometry Building:
The actual implementation of the Trk::LayerBuilders and Trk::TrackingVolumeBuilders must be found in the 
corresponding Sub Detector repositories. The Trk::GeometryBuilder acts as a central Tool retrieving
Trk::Volume and Trk::Layer instances from the SubDetectors, gluing them together to a Trk::TrackingGeometry.
Therefor it owns friend-rights of Trk::Layer, Trk::Volume and Trk::BoundarySurface classes.
Various helper Tools exist for the geometry building process.


   - Material mapping
The Trk::MaterialMapper is a convenient plugin that can be used within the Trk::Extrapolator to monitor the
crossed material amount. The Trk::LayerMaterialInspector is on the other hand a AlgTool that allows to fill
the material information per layer into an nTuple.

   - Displaying
The Trk::TrackingVolumeDisplayer writes ROOT scripts for a dedicated TrackingGeometry visualization.



*/
