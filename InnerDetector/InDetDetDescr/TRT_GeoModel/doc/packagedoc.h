/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TRT_GeoModel_page TRT_GeoModel Package

@authors Thomas Kittlemann, Joe Boudreau.


@section TRT_GeoModel_TRT_GeoModelIntro Introduction

This package contains the factories for building the GeoModel Detector
Description of the TRT detector. It builds both the material
geometry using the GeoModelKernel and the readout geometry using
classes in InDetReadoutGeometry.

The creation of the geometry is initiated by the DetectorTool
(TRT_DetectorTool) which is called from GeoModelSvc. The DetectorTool
calls the top level factory (eg TRTDetectorFactory_Full) which starts the
building process.

The version of geometry is controlled by the Detector Description tag which 
can be specified at various levels (ATLAS, InnerDetector or TRT node).  

See https://uimon.cern.ch/twiki/bin/view/Atlas/InDetGeometryVersions for current versions supported.

See the InDetDetDescrExample package for examples of how to load the geometry 
and access detector elements.

@section TRT_GeoModel_TRT_GeoModelClassOverview Class Overview
  
The package contains the following main classes:

 - TRT_DetectorTool: Tool called by GeoModelSvc to start the building process
 - TRTDetectorFactory_Full: Top level factory. 
 - TRTDetectorFactory_G3Compatible: Top level factory for G3 compatible version.
 - TRTParameterInterface: %Base class for Interface to primary numbers.
 - TRT_DetDescrDB_ParameterInterface: Interface to primary numbers from the Detector Description Database.




*/
