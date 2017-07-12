/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page PixelGeoModel_page PixelGeoModel Package

@authors Davide Costanzo. Contributions from Grant Gorfine.

@section PixelGeoModel_PixelGeoModelIntro Introduction

This package contains the factories for building the GeoModel Detector
Description of the Pixel detector. It builds both the material
geometry using the GeoModelKernel and the readout geometry using
classes in InDetReadoutGeometry.

The creation of the geometry is initiated by the DetectorTool
(PixelDetectorTool) which is called from GeoModelSvc. The DetectorTool
calls the top level factory (PixelDetectorFactory) which starts the
building process.

The version of geometry is controlled by the Detector Description tag which 
can be specified at various levels (ATLAS, InnerDetector or Pixel node).  

See https://uimon.cern.ch/twiki/bin/view/Atlas/InDetGeometryVersions for current versions supported.

See the InDetDetDescrExample package for examples of how to load the geometry 
and access detector elements.

@section PixelGeoModel_PixelGeoModelClassOverview Class Overview
  
The package contains several helper factories which build the different components of the Pixel.
Some of the other classes are described here:

 - PixelDetectorTool: Tool called by GeoModelSvc to start the building process
 - PixelDetectorFactory: Top level factory. It in turn calls the component factories.
 - GeoVPixelFactory: %Base class for all component factories.
 - PixelGeometryManager: %Base class for interfaces to primary numbers. 
 - OraclePixGeoManager: Interface to primary numbers from the Detector Description Database.



*/
