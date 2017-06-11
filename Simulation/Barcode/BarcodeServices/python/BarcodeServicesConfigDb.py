# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for BarcodeServices
Elmar Ritsch, 23/10/2014
"""

from AthenaCommon.CfgGetter import addService

# Common tools, services and algorithms used by jobs:
addService("BarcodeServices.BarcodeServicesConfig.getMC12BarcodeSvc"          ,  "Barcode_MC12BarcodeSvc"          )
addService("BarcodeServices.BarcodeServicesConfig.getMC12LLPBarcodeSvc"       ,  "Barcode_MC12LLPBarcodeSvc"       )
addService("BarcodeServices.BarcodeServicesConfig.getMC12PlusBarcodeSvc"      ,  "Barcode_MC12PlusBarcodeSvc"      )
addService("BarcodeServices.BarcodeServicesConfig.getMC15aBarcodeSvc"         ,  "Barcode_MC15aBarcodeSvc"         )
addService("BarcodeServices.BarcodeServicesConfig.getMC15aPlusBarcodeSvc"     ,  "Barcode_MC15aPlusBarcodeSvc"     )
addService("BarcodeServices.BarcodeServicesConfig.getMC15aPlusLLPBarcodeSvc"  ,  "Barcode_MC15aPlusLLPBarcodeSvc"  )
addService("BarcodeServices.BarcodeServicesConfig.getMC15BarcodeSvc"          ,  "Barcode_MC15BarcodeSvc"          )
addService("BarcodeServices.BarcodeServicesConfig.getMC16BarcodeSvc"          ,  "Barcode_MC16BarcodeSvc"          )
addService("BarcodeServices.BarcodeServicesConfig.getMC16LLPBarcodeSvc"       ,  "Barcode_MC16LLPBarcodeSvc"       )
addService("BarcodeServices.BarcodeServicesConfig.getPhysicsProcessBarcodeSvc",  "Barcode_PhysicsProcessBarcodeSvc")
addService("BarcodeServices.BarcodeServicesConfig.getGlobalBarcodeService"    ,  "Barcode_GlobalBarcodeService"    )
addService("BarcodeServices.BarcodeServicesConfig.getLegacyBarcodeService"    ,  "Barcode_LegacyBarcodeService"    )
addService("BarcodeServices.BarcodeServicesConfig.getValidationBarcodeSvc"    ,  "Barcode_ValidationBarcodeSvc"    )

