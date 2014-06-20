/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage PixelConditionsServices

@author Wei-Ming.Yao@cern.ch, ruwiedel@physik.uni-bonn.de

@section PixelConditionsServicesIntroduction Introduction

The package PixelConditionsServices provides services to create and manage the data objects holding the
calibration and conditions data from the offline COOL database for the pixel detector. The service
PixelCalibSvc is used to access the calibration data for the pixel detector. The service SpecialPixelMapSvc
creates and manages a set of maps of special pixels in the detector store. In addition to the services, two
example algorithms that illustrate the usage of the services are provided: PixelCalibServiceTest and
PixelMapTestAlg.

The classes for the data objects are defined in the package PixelConditionsData.

@section PixelConditionsServicesPixelCalibSvc PixelCalibSvc 
The PixelCalibSvc provides a set of services to access pixel calibration data objects in the detector store.
The creation of pixel calibration data is handled by PixelCalibDbTool via a callback function from the
cool database.

@section PixelConditionsServicesSpecialPixelMapSvc SpecialPixelMapSvc

This service can be used to create and manage several special pixel map objects in the detector store. The
service provides a callback function that, unless switched off in the job options, is registered in the
initialize() method. The callback function automatically updates the special pixel map objects in the detector
store for the current run/lumi block. At the default setting, all maps are merged in the callback function and
the merged map is stored in place of the first map in the list of StoreGate keys.

In addition to the callback function the SpecialPixelMapSvc provides some utility functions: The method
create() can be used to construct a special pixel map on demand, using data either from the database or from
text files. The method print() is used to print pixel maps for a list of modules specified in the job options
to the screen.

@section PixelConditionsServicesExtras Extra Pages

 - @ref PixelConditionsServicesUsedPackages
 - @ref PixelConditionsServicesRequirements
*/

/**
@page PixelConditionsServicesUsedPackages Used Packages
@htmlinclude used_packages.html
*/

/**
@page PixelConditionsServicesRequirements Requirements
@include requirements
*/

