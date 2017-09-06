/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page PixelConditionsData_page PixelConditionsData

@author Wei-Ming.Yao@cern.ch, ruwiedel@physik.uni-bonn.de

@section PixelConditionsData_PixelConditionsDataIntroduction Introduction

The calibration and conditions data for the pixel detector in the offline COOL database is stored in a format
that is not directly useful or easily accessible in an athena component. Hence, after reading the data from
the database it has to be converted into a more accessible format first, which is given by the classes
provided in this package. The calibration data for the pixel detector is accessed through the class
PixelCalibData. The class DetectorSpecialPixelMap holds the information about pixels with special properties.

The data objects can be created and managed using the tools and services provided in the packages
PixelConditionsTools and PixelConditionsServices. The package PixelConditionsServices provides some example
algorithms that illustrate the usage of the services.

@section PixelConditionsData_PixelConditionsDataPixelCalibData PixelCalibData

The PixelCalibData is a class that designed to hold the calibration data for each module that contains
16 FE PixelChipSummaryData. The calibration data consist of threshold, timewalk information and Time over
threshold calibration that averages per FE chip. The details can be found at
https://twiki.cern.ch/twiki/bin/view/Atlas/PixelCOOLoffline#Calibrations
.

@section PixelConditionsData_PixelConditionsDataSpecialPixelMap Special Pixel Map

There are two classes that are used together to hold the data about pixels with special properties,
DetectorSpecialPixelMap and ModuleSpecialPixelMap. A DetectorSpecialPixelMap is a std::vector of pointers to
ModuleSpecialPixelMaps. A ModuleSpecialPixelMap* is referenced by the IdentifierHash of the corresponding
module.

A ModuleSpecialPixelMap is a map of the status codes of the special pixels on a single module. The status code
for a pixel is referenced by a pixel ID which is independent of the position of the module in the
detector. The pixel ID can be calculated with the method encodePixelID. The possible pixel status codes are
explained on a wiki page:
https://twiki.cern.ch/twiki/bin/view/Atlas/PixelCOOLoffline#Modified_proposal_18_10_2006

More technical details about the format of the various unsigned integers can be found in the README file in
the doc directory of the package PixelConditionsServices.

@section PixelConditionsData_PixelConditionsDataExtras Extra Pages

*/
