/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


This is the draft of TileGeoModel documentation, under construction ... us usual for documentation

/**

@page TileGeoModel_page TileGeoModel Package
 
 @authors Sergey Baranov, Vakho Tsulaia.
 
@section TileGeoModel_TileGeoModelIntro Introduction
 
 This package contains the factories for building the GeoModel Detector
 Description of the Tile detector. It builds the material
 geometry using the GeoModelKernel.
 
 The creation of the geometry is initiated by the DetectorTool
 (TileDetectorTool) which is called from GeoModelSvc. The DetectorTool
 calls the top level factory (TileDetectorFactory) which starts the
 building process.
 
 The version of geometry is controlled by the Detector Description tag which 
 can be specified at various levels (ATLAS or TileCal node).  
 
 See http://atlas.web.cern.ch/Atlas/GROUPS/SOFTWARE/OO/php/DDDB/tag_hierarchy_browser.php for 
 current versions supported.
 
 See the share/TileGeoModel_jobOptions.py as example of how to load the geometry.
 
@section TileGeoModel_TileGeoModelClassOverview Class Overview
   
 The package contains several helper factories which build the different components of the Pixel.
 Some of the other classes are described here:
 
  - TileAtlasFactory: The main tile geometry constructor factory
  - TileDetectorTool: Tool called by GeoModelSvc to start the building process
  - TileDetectorFactory: Top level factory. It in turn calls the component factories.
  - TileGeoSectionBuilder: %Base class for section construction.
  - TileTBFactory: %Test Beam factory. 

 

*/

//- TileOracle: Interface for filling primary numbers with Python tools to the Detector Description Database.
