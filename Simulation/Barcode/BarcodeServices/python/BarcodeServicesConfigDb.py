# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for BarcodeServices
Elmar Ritsch, 23/10/2014
"""

from AthenaCommon.CfgGetter import addTool, addToolClone, addService, addAlgorithm, \
     addTypesToExcludeIfDefaultValue, addNamesToExcludeIfDefaultValue, addFullNamesToExcludeIfDefaultValue, \
     addPropertiesToExcludeIfDefault, \
     addTypesToSkipIfNotAvailable, addNamesToSkipIfNotAvailable, addFullNamesToSkipIfNotAvailable, \
     addTypesOnlyToSkip

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
import AthenaCommon.SystemOfUnits as Units

# Common tools, services and algorithms used by jobs:
addService("BarcodeServices.BarcodeServicesConfig.getMC12BarcodeSvc"          ,  "Barcode_MC12BarcodeSvc"          )
addService("BarcodeServices.BarcodeServicesConfig.getMC15BarcodeSvc"          ,  "Barcode_MC15BarcodeSvc"          )
addService("BarcodeServices.BarcodeServicesConfig.getPhysicsProcessBarcodeSvc",  "Barcode_PhysicsProcessBarcodeSvc")
addService("BarcodeServices.BarcodeServicesConfig.getGlobalBarcodeService"    ,  "Barcode_GlobalBarcodeService"    )
addService("BarcodeServices.BarcodeServicesConfig.getLegacyBarcodeService"    ,  "Barcode_LegacyBarcodeService"    )
addService("BarcodeServices.BarcodeServicesConfig.getValidationBarcodeSvc"    ,  "Barcode_ValidationBarcodeSvc"    )

