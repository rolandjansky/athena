/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage TileTrackingGeometry package

@author Andreas.Salzburger@cern.ch

@section TileTrackingGeometryIntro Introduction

This packge contains a single AthAlgTool, the TileVolumeBuilder that
translates the TileGeometry description into Trk::TrackingVolume objects.
The layer position & dimensions are gathered directly through the GeoModel
description or through the CaloSurfaceProvider that is part of the 
CaloTrackingGeometry package.

The material maps are loaded through conditions data service in the 
TrackingGeometry building process, steered via a geometry callback.



*/
