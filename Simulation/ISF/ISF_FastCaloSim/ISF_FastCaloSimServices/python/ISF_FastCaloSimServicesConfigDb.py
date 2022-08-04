# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for ISF_FastCaloSimServices
Elmar Ritsch, 01/06/2016
"""

from AthenaCommon.CfgGetter import addTool, addService, addAlgorithm

addTool("ISF_FastCaloSimServices.AdditionalConfigLegacy.getNIMatEffUpdator",                 "ISF_NIMatEffUpdator")
addTool("ISF_FastCaloSimServices.AdditionalConfigLegacy.getNIPropagator",                    "ISF_NIPropagator")
addTool("ISF_FastCaloSimServices.AdditionalConfigLegacy.getNINavigator",                      "ISF_NINavigator")
addTool("ISF_FastCaloSimServices.AdditionalConfigLegacy.getNITimedExtrapolator",             "ISF_NITimedExtrapolator")
addTool("ISF_FastCaloSimServices.AdditionalConfigLegacy.getTimedExtrapolator",               "TimedExtrapolator")
addTool("ISF_FastCaloSimServices.AdditionalConfigLegacy.getPunchThroughTool",                "ISF_PunchThroughTool")
addTool("ISF_FastCaloSimServices.AdditionalConfigLegacy.getEmptyCellBuilderTool",            "ISF_EmptyCellBuilderTool")
addTool("ISF_FastCaloSimServices.AdditionalConfigLegacy.getFastShowerCellBuilderTool",       "ISF_FastShowerCellBuilderTool")
addTool("ISF_FastCaloSimServices.AdditionalConfigLegacy.getLegacyFastShowerCellBuilderTool", "ISF_LegacyFastShowerCellBuilderTool")
addTool("ISF_FastCaloSimServices.AdditionalConfigLegacy.getPileupFastShowerCellBuilderTool", "ISF_PileupFastShowerCellBuilderTool")
addTool("ISF_FastCaloSimServices.AdditionalConfigLegacy.getAddNoiseCellBuilderTool",         "ISF_AddNoiseCellBuilderTool")
addTool("ISF_FastCaloSimServices.AdditionalConfigLegacy.getCaloCellContainerFinalizerTool",  "ISF_CaloCellContainerFinalizerTool")
addTool("ISF_FastCaloSimServices.AdditionalConfigLegacy.getCaloCellContainerFCSFinalizerTool",  "ISF_CaloCellContainerFCSFinalizerTool")
addTool("ISF_FastCaloSimServices.AdditionalConfigLegacy.getFastHitConvertTool",              "ISF_FastHitConvertTool")

addTool("ISF_FastCaloSimServices.AdditionalConfigLegacy.getFastCaloTool",                    "ISF_FastCaloTool")
addTool("ISF_FastCaloSimServices.AdditionalConfigLegacy.getFastCaloPileupTool",              "ISF_FastCaloPileupTool")
addTool("ISF_FastCaloSimServices.AdditionalConfigLegacy.getLegacyAFIIFastCaloTool",          "ISF_LegacyAFIIFastCaloTool")

addTool("ISF_FastCaloSimServices.AdditionalConfigLegacy.getFastCaloSimV2Tool",               "ISF_FastCaloSimV2Tool")
addAlgorithm("ISF_FastCaloSimServices.AdditionalConfigLegacy.getFastHitConvAlg",             "ISF_FastHitConvAlg")

addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfigLegacy.getFastCaloSimSvc",                             "ISF_FastCaloSimSvc")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfigLegacy.getFastCaloSimPileupSvc",                       "ISF_FastCaloSimPileupSvc")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfigLegacy.getFastCaloSimPileupOTSvc",                     "ISF_FastCaloSimPileupOTSvc")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfigLegacy.getFastHitConvAlgFastCaloSimSvc",               "ISF_FastHitConvFastCaloSimSvc")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfigLegacy.getLegacyAFIIFastCaloSimSvc",                   "ISF_LegacyAFIIFastCaloSimSvc")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfigLegacy.getFastHitConvAlgLegacyAFIIFastCaloSimSvc",     "ISF_FastHitConvAlgLegacyAFIIFastCaloSimSvc")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfigLegacy.getFastCaloSimV2ParamSvc",                      "ISF_FastCaloSimV2ParamSvc")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfigLegacy.getFastCaloSimSvcV2",                           "ISF_FastCaloSimSvcV2")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfigLegacy.getDNNCaloSimSvc",                           "ISF_DNNCaloSimSvc")
