# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for BarcodeServices
Elmar Ritsch, 23/10/2014
"""

from AthenaCommon.CfgGetter import addService

# Common tools, services and algorithms used by jobs:
addService("BarcodeServices.BarcodeServicesConfigLegacy.getMC12BarcodeSvc"          ,  "Barcode_MC12BarcodeSvc"          )
addService("BarcodeServices.BarcodeServicesConfigLegacy.getMC12LLPBarcodeSvc"       ,  "Barcode_MC12LLPBarcodeSvc"       )
addService("BarcodeServices.BarcodeServicesConfigLegacy.getMC12PlusBarcodeSvc"      ,  "Barcode_MC12PlusBarcodeSvc"      )
addService("BarcodeServices.BarcodeServicesConfigLegacy.getMC15aBarcodeSvc"         ,  "Barcode_MC15aBarcodeSvc"         )
addService("BarcodeServices.BarcodeServicesConfigLegacy.getMC15aPlusBarcodeSvc"     ,  "Barcode_MC15aPlusBarcodeSvc"     )
addService("BarcodeServices.BarcodeServicesConfigLegacy.getMC15aPlusLLPBarcodeSvc"  ,  "Barcode_MC15aPlusLLPBarcodeSvc"  )
addService("BarcodeServices.BarcodeServicesConfigLegacy.getMC15BarcodeSvc"          ,  "Barcode_MC15BarcodeSvc"          )
addService("BarcodeServices.BarcodeServicesConfigLegacy.getMC16BarcodeSvc"          ,  "Barcode_MC16BarcodeSvc"          )
addService("BarcodeServices.BarcodeServicesConfigLegacy.getMC16LLPBarcodeSvc"       ,  "Barcode_MC16LLPBarcodeSvc"       )
addService("BarcodeServices.BarcodeServicesConfigLegacy.getMC18BarcodeSvc"          ,  "Barcode_MC18BarcodeSvc"          )
addService("BarcodeServices.BarcodeServicesConfigLegacy.getMC18LLPBarcodeSvc"       ,  "Barcode_MC18LLPBarcodeSvc"       )
addService("BarcodeServices.BarcodeServicesConfigLegacy.getPhysicsProcessBarcodeSvc",  "Barcode_PhysicsProcessBarcodeSvc")
addService("BarcodeServices.BarcodeServicesConfigLegacy.getGlobalBarcodeService"    ,  "Barcode_GlobalBarcodeService"    )
addService("BarcodeServices.BarcodeServicesConfigLegacy.getLegacyBarcodeService"    ,  "Barcode_LegacyBarcodeService"    )
addService("BarcodeServices.BarcodeServicesConfigLegacy.getValidationBarcodeSvc"    ,  "Barcode_ValidationBarcodeSvc"    )

