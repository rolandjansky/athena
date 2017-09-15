/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


This is the draft of TileDetDescr documentation, under construction ... us usual for documentation

/**

@page TileDetDescr_page TileDetDescr Package

 This package provides detector constants for geometry description (TileGeoModel) for the TileCalorimeter.

 @authors Alexander Solodkov, Pavel Starovoitov
 
@section TileDetDescr_TileDetDescr Introduction
 
 This package contains the factories for building the TileDetDescr Interface for TileGeoModel description.

 The creation of the geometry is initiated by the DetectorTool
 (TileDetectorTool) which is called from GeoModelSvc. The DetectorTool
 calls the top level factory (TileDetectorFactory) which starts the
 building process.
 
@section TileDetDescr_TileDetDescrClassOverview Class Overview
   
 The package contains several helper factories which build the different components of the Tile.
 Some of the other classes are described here:
 
  - TileCellDim: %Base class for calculation all cell dimensions
  - TileDddbManager: %Base class for access to all geometrical elements:
      TILE, Tile Globals, Modules, TILB, SCNT structure, Girder, TIFG, TICG and TICL
  - TileDetDescriptor: %Base class contains the detector description information for the Tile calorimeters: 
      Barrel, Ext.Barrel, Gap detector.
  - TileDetDescrManager: %Base class for acceess geometrical data for:
      Tile Regions, Tile Descriptors, Calo Descriptors, Tile Modules and Cells
  - TileDetDescrRegion: This class provides an interface to detector description
    information for the regions (sub-detectors) of the  Tile calorimeter.

*/
