/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page InDetServMatGeoModel_page InDetServMatGeoModel Package

@authors Davide Costanzo, Grant Gorfine

@section InDetServMatGeoModel_InDetServMatGeoModelIntro Introduction


This package contains the factories for building the GeoModel Detector
Description for the services of the inner detector which are not included in the sub-system descriptions.

The creation of the geometry is initiated by the DetectorTool
(InDetServMatTool) which is called from GeoModelSvc. The DetectorTool
calls a top level factory (eg InDetServMatFactory) which starts the
building process.

@section InDetServMatGeoModel_InDetServMatGeoModelClassOverview Class Overview

 - InDetServMatTool: Tool called by GeoModelSvc to start the building process.
 - InDetServMatManager: The manager allows access to the geometry (needed by Geo2G4 for example).
 - InDetServMatFactory: Top level factory.
 - PixelServMatFactory: Build Pixel services.
 - SCT_ServMatFactory: Build SCT services.
 - TRT_ServMatFactory: Build TRT services.

@section InDetServMatGeoModel_InDetServMatGeoModelLayout Service Material layout.
 
The following diagram illustrates the location of the service materials.

@image html inafpp.gif



*/
