# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for ISF_FastCaloSimServices
Elmar Ritsch, 01/06/2016
"""

from AthenaCommon.CfgGetter import addTool, addService, addAlgorithm

addTool("ISF_FastCaloSimServices.AdditionalConfig.getNIMatEffUpdator",                 "ISF_NIMatEffUpdator")
addTool("ISF_FastCaloSimServices.AdditionalConfig.getNIPropagator",                    "ISF_NIPropagator")
addTool("ISF_FastCaloSimServices.AdditionalConfig.getNINavigator",                      "ISF_NINavigator")
addTool("ISF_FastCaloSimServices.AdditionalConfig.getNITimedExtrapolator",             "ISF_NITimedExtrapolator")
addTool("ISF_FastCaloSimServices.AdditionalConfig.getTimedExtrapolator",               "TimedExtrapolator")
addTool("ISF_FastCaloSimServices.AdditionalConfig.getPunchThroughTool",                "ISF_PunchThroughTool")
addTool("ISF_FastCaloSimServices.AdditionalConfig.getEmptyCellBuilderTool",            "ISF_EmptyCellBuilderTool")
addTool("ISF_FastCaloSimServices.AdditionalConfig.getFastShowerCellBuilderTool",       "ISF_FastShowerCellBuilderTool")
addTool("ISF_FastCaloSimServices.AdditionalConfig.getLegacyFastShowerCellBuilderTool", "ISF_LegacyFastShowerCellBuilderTool")
addTool("ISF_FastCaloSimServices.AdditionalConfig.getPileupFastShowerCellBuilderTool", "ISF_PileupFastShowerCellBuilderTool")
addTool("ISF_FastCaloSimServices.AdditionalConfig.getAddNoiseCellBuilderTool",         "ISF_AddNoiseCellBuilderTool")
addTool("ISF_FastCaloSimServices.AdditionalConfig.getCaloCellContainerFinalizerTool",  "ISF_CaloCellContainerFinalizerTool")
addTool("ISF_FastCaloSimServices.AdditionalConfig.getCaloCellContainerFCSFinalizerTool",  "ISF_CaloCellContainerFCSFinalizerTool")
addTool("ISF_FastCaloSimServices.AdditionalConfig.getFastHitConvertTool",              "ISF_FastHitConvertTool")

addTool("ISF_FastCaloSimServices.AdditionalConfig.getFastCaloTool",                    "ISF_FastCaloTool")
addTool("ISF_FastCaloSimServices.AdditionalConfig.getFastCaloPileupTool",              "ISF_FastCaloPileupTool")
addTool("ISF_FastCaloSimServices.AdditionalConfig.getLegacyAFIIFastCaloTool",          "ISF_LegacyAFIIFastCaloTool")

addTool("ISF_FastCaloSimServices.AdditionalConfig.getFastCaloSimV2Tool",               "ISF_FastCaloSimV2Tool")
addAlgorithm("ISF_FastCaloSimServices.AdditionalConfig.getFastHitConvAlg",             "ISF_FastHitConvAlg")

addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getFastCaloSimSvc",                             "ISF_FastCaloSimSvc")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getFastCaloSimPileupSvc",                       "ISF_FastCaloSimPileupSvc")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getFastCaloSimPileupOTSvc",                     "ISF_FastCaloSimPileupOTSvc")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getFastHitConvAlgFastCaloSimSvc",               "ISF_FastHitConvFastCaloSimSvc")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getLegacyAFIIFastCaloSimSvc",                   "ISF_LegacyAFIIFastCaloSimSvc")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getFastHitConvAlgLegacyAFIIFastCaloSimSvc",     "ISF_FastHitConvAlgLegacyAFIIFastCaloSimSvc")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getFastCaloSimV2ParamSvc",                      "ISF_FastCaloSimV2ParamSvc")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getFastCaloSimSvcV2",                           "ISF_FastCaloSimSvcV2")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getDNNCaloSimSvc",                           "ISF_DNNCaloSimSvc")
