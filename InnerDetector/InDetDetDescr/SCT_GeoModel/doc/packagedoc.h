/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page SCT_GeoModel_page SCT_GeoModel Package

@authors Grant Gorfine

@section SCT_GeoModel_SCT_GeoModelIntro Introduction

This package contains the factories for building the GeoModel Detector
Description of the SCT. It builds both the material geometry  using the
GeoModelKernel and  the readout geometry using classes in
InDetReadoutGeometry.

The creation of the geometry is initiated by the DetectorTool
(SCT_DetectorTool) which is called from GeoModelSvc. The DetectorTool
calls a top level factory (eg SCT_DetectorFactory) which starts the
building process.

The version of geometry is controlled by the Detector Description tag which 
can be specified at various levels (ATLAS, InnerDetector or SCT node).  

See https://uimon.cern.ch/twiki/bin/view/Atlas/InDetGeometryVersions for current versions supported.

See the InDetDetDescrExample package for examples of how to load the geometry 
and access detector elements.

@section SCT_GeoModel_SCT_GeoModelClassOverview Class Overview
  
The package contains several helper factories which build the different components of the SCT.
Some of the other classes are described here:

 - SCT_DetectorTool: Tool called by GeoModelSvc to start the building process
 - SCT_DetectorFactory: Top level factory for SCT. It in turn calls the component factories.
 - SCT_DetectorFactoryCosmic: Top level factory for building SCT barrel for Cosmic run in SR1 building. 
 - SCT_ComponentFactory: %Base class for all component factories.
 - SCT_GeometryManager: General manager. 
 - SCT_Identifier: Helper class for building up the ATLAS Identifier
 - SCT_DataBase: Access the Detector Description database
 - SCT_xxxParameters: Interfaces for primary numbers. 
 - SCT_MaterialManager: Interface to the material manager.
 - SCT_Options: Class to contain various building options.
 - SCT_DefaultConditions: Class to store default conditions data.



*/
