/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage PixelConditionsTools 
@author Wei-Ming.Yao@cern.ch, ruwiedel@physik.uni-bonn.de

@section PixelConditionsToolsIntro Introduction

 The package PixelConditionsTools provides tools to create and manage the data objects holding the calibration 
 and conditions data from the offline COOL database for the pixel detector. The PixelCalibDbTool is used to 
 read the calibration data from the offline COOL database via a callback and convert to PixelCalibData objects 
 stored in the detector store in Athena. It also provides a tool to read/write a text file from/to the 
 COOL database. The SpecialPixelMapTool is obsolete that was used to create a special pixel map. 

 The classes for the data objects are defined in the package PixelConditionsData.

@section PixelConditionsToolsPixelCalibDbTool PixelCalibDbTool 

    The PixelCalibDbTool is used to create and manage the data objects holding the calibration data for the
    Pixel detector. It reads the calibration data from the offline COOL database via a callback and converts
    to PixelCalibData objects stored in the detector store in Athena. It also provides a tool to read/write
    a text file from/to the COOL database.

@section PixelConditionsToolsPixelCalibDbTestWriteRead PixelCalibDbTestWriteRead
   The PixelCalibDbTestWriteRead provides an algorithm how to read/write the calibration data in a text file
   from/to the COOL database.

@section PixelConditionsToolsExtras Extra Pages

 - @ref PixelConditionsToolsUsedPackages
 - @ref PixelConditionsToolsRequirements
*/

/**
@page PixelConditionsToolsUsedPackages Used Packages
@htmlinclude used_packages.html
*/

/**
@page PixelConditionsToolsRequirements Requirements
@include requirements
*/

