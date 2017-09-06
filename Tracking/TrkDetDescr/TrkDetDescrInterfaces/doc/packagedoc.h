/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkDetDescrInterfaces_page TrkDetDescrInterfaces Package

This package contains all interface classes for \<nop\>AlgTools to be used for the creation,
validation and displaying of the Trk::TrackingGeomtery.

@author Andreas.Salzburger@cern.ch

@section TrkDetDescrInterfaces_TrkDetDescrInterfacesIntro Introduction

The following interface classes are defined


   - IAlignableSurfaceProvider.h : Plugin for the AlignmentKalmanFilter 
   - IDetachedTrackingVolumeBuilder.h : dedicated VolumeBuilder for detached tracking volumes 
   - IDynamicLayerCreator.h : an interface for dynamic material integration (Layer-based) for Tracking 
   - IGeometryBuilder.h : the most generic geometry building interface
   - ILayerArrayCreator.h : helper tool for layer creation
   - ILayerBuilder.h : interface for sub-detector layer building
   - ILayerMaterialInspector.h : validation tool for material inspection
   - IMaterialEffectsOnTrackProvider.h : an interface for dynamic material integration (MEOT-based) for Tracking 
   - IMaterialMapper.h: an interface for material mapping 
   - ITrackingGeometrySvc.h : the  service interface for TrackingGeometry creation
   - ITrackingVolumeArrayCreator.h : helper tool for volume array creation
   - ITrackingVolumeBuilder.h : volume builder interface for sub-detector volume building
   - ITrackingVolumeDisplayer.h : interface for display actions
   - ITrackingVolumeHelper.h : helper for resizing, changing, creation, etc. of tracking volumes
     
The concrete implementations rely on very precise knowledge of the various sub-detectors and have 
to be found in the corresponding software repositories.

   


*/