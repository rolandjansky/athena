/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetTrackingGeometry_page InDetTrackingGeometry Package

This Package contains the Layer and VolumeBuilder AlgTools that 
inherit from the Trk::IVolumeBuilder and Trk::ILayerBuilder interface classes.
The GeoModel detector description is parsed and a simplified Trk::TrackingGeometry
is created for navigation and material update in the Extrapolation realm.

@author Andreas.Salzburger@cern.ch

@section InDetTrackingGeometry_InDetTrackingGeometryDescr Description

Following classes are currently implemented:
  - InDet::BeamPipeBuilder: build a cylindrical Layer representing the BeamPipe.
  - InDet::PixelLayerBuilder: builds cylindrical and disc-like Layers for the Pixel detector.
  - InDet::SCT_LayerBuilder: builds cylindrical and disc-like Layers for the SCT detector.
  - InDet::TRT_VolumeBuilder: builds 3 cylindrical Trk::TrackingVolume for the TRT detector.
  - InDet::RobustTrackingGeometryBuilder: builds a connective Trl::TrackingGeometry for the Inner Detector
  
The dedicated Trk::TrackingVolume and Trk::Layer classes can be found in the TrkGeometry package in the Tracking/TrkDetDescr realm.

@section InDetTrackingGeometry_InDetTrackinGeometryDocu Material Comparison for DC2

The following plots show the comparison between the material distribution gathered from Geant4 simulation and the Trk::TrackingGeometry 
material description for DC2 Layout and default values. Fine-tuning of the values can be done by jobOptions acess.

<table>
<tr>
 <td> @image html BeamPipe.gif </td>
 <td> @image html Pixel.gif </td>
</tr>
<tr>
 <td> @image html SCT.gif </td>
 <td> @image html TRT.gif </td>
</tr>
<tr>
 <td> @image html IDS.gif </td>
 <td> @image html ID.gif </td>
</tr>
</table>
  


*/