# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for ISF
KG Tan, 17/06/2012
"""

from AthenaCommon.CfgGetter import addTool, addToolClone, addService, addAlgorithm, \
     addTypesToExcludeIfDefaultValue, addNamesToExcludeIfDefaultValue, addFullNamesToExcludeIfDefaultValue, \
     addPropertiesToExcludeIfDefault, \
     addTypesToSkipIfNotAvailable, addNamesToSkipIfNotAvailable, addFullNamesToSkipIfNotAvailable, \
     addTypesOnlyToSkip

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
import AthenaCommon.SystemOfUnits as Units

# Common tools, services and algorithms used by jobs
addService("ISF_Geant4CommonServices.ISF_Geant4CommonServicesConfig.getG4PolyconeGeoIDSvc",    "ISF_G4PolyconeGeoIDSvc"   )
addService("ISF_Geant4CommonServices.ISF_Geant4CommonServicesConfig.getAFIIG4PolyconeGeoIDSvc","ISF_AFIIG4PolyconeGeoIDSvc")
addService("ISF_Geant4CommonServices.ISF_Geant4CommonServicesConfig.getG4PolyconeGeoIDSvc_G4", "ISF_G4PolyconeGeoIDSvc_G4")

addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getGenEventValidityChecker",    "ISF_GenEventValidityChecker")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getVertexPositionFromFile",     "ISF_VertexPositionFromFile")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getVertexBeamCondPositioner",   "ISF_VertexBeamCondPositioner")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getLongBeamspotVertexPositioner", "ISF_LongBeamspotVertexPositioner")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getGenEventVertexPositioner",   "ISF_GenEventVertexPositioner")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getParticleFinalStateFilter",   "ISF_ParticleFinalStateFilter")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getParticleSimWhiteList",       "ISF_ParticleSimWhiteList")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getParticlePositionFilter",     "ISF_ParticlePositionFilter")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getEtaPhiFilter",               "ISF_EtaPhiFilter")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getStackFiller",                "ISF_StackFiller")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getLongLivedStackFiller",       "ISF_LongLivedStackFiller")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getTruthStrategyGroupID_MC15",         "ISF_MCTruthStrategyGroupID_MC15")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getTruthStrategyGroupIDHadInt_MC15",   "ISF_MCTruthStrategyGroupIDHadInt_MC15")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getTruthStrategyGroupCaloMuBrem_MC15", "ISF_MCTruthStrategyGroupCaloMuBrem_MC15")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getTruthStrategyGroupCaloDecay_MC15",  "ISF_MCTruthStrategyGroupCaloDecay_MC15")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getTruthStrategyGroupID",           "ISF_MCTruthStrategyGroupID")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getTruthStrategyGroupIDHadInt",     "ISF_MCTruthStrategyGroupIDHadInt")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getTruthStrategyGroupCaloMuBrem",   "ISF_MCTruthStrategyGroupCaloMuBrem")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getTruthStrategyGroupCaloDecay",    "ISF_MCTruthStrategyGroupCaloDecay")
addTool("ISF_HepMC_Tools.ISF_HepMC_ToolsConfig.getValidationTruthStrategy",        "ISF_ValidationTruthStrategy")
addService("ISF_HepMC_Services.ISF_HepMC_ServicesConfig.getTruthService"              , "ISF_TruthService")
addService("ISF_HepMC_Services.ISF_HepMC_ServicesConfig.getMC12TruthService"          , "ISF_MC12TruthService")
addService("ISF_HepMC_Services.ISF_HepMC_ServicesConfig.getMC15TruthService"          , "ISF_MC15TruthService")
addService("ISF_HepMC_Services.ISF_HepMC_ServicesConfig.getValidationTruthService"    , "ISF_ValidationTruthService")

addTool("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getNIMatEffUpdator",                 "ISF_NIMatEffUpdator")
addTool("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getNIPropagator",                    "ISF_NIPropagator")
addTool("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getNITimedExtrapolator",             "ISF_NITimedExtrapolator")
addTool("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getPunchThroughTool",                "ISF_PunchThroughTool")
addTool("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getEmptyCellBuilderTool",            "ISF_EmptyCellBuilderTool")
addTool("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getFastShowerCellBuilderTool",       "ISF_FastShowerCellBuilderTool")
addTool("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getPileupFastShowerCellBuilderTool", "ISF_PileupFastShowerCellBuilderTool")
addTool("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getCaloNoiseTool",                   "ISF_FCS_CaloNoiseTool")
addTool("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getAddNoiseCellBuilderTool",         "ISF_AddNoiseCellBuilderTool")
addTool("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getCaloCellContainerFinalizerTool",  "ISF_CaloCellContainerFinalizerTool")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getNativeFastCaloSimSvc",         "ISF_NativeFastCaloSimSvc")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getFastCaloSimSvc",               "ISF_FastCaloSimSvc")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getFastCaloSimPileupSvc",         "ISF_FastCaloSimPileupSvc")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getFastHitConvAlgFastCaloSimSvc", "ISF_FastHitConvFastCaloSimSvc")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getFastCaloSimSvcBase",           "ISF_FastCaloSimSvcBase")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getLegacyAFIIFastCaloSimSvc",     "ISF_LegacyAFIIFastCaloSimSvc")
addService("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getFastHitConvAlgLegacyAFIIFastCaloSimSvc",     "ISF_FastHitConvAlgLegacyAFIIFastCaloSimSvc")
addTool("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getFastHitConvertTool",              "ISF_FastHitConvertTool")
addAlgorithm("ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfig.getFastHitConvAlg",             "ISF_FastHitConvAlg")

addService("ISF_Geant4Config.ISF_iGeant4Config.getGeant4SimSvc",                        "ISF_Geant4SimSvc")
addService("ISF_Geant4Config.ISF_iGeant4Config.getFullGeant4SimSvc",                    "ISF_FullGeant4SimSvc")
addService("ISF_Geant4Config.ISF_iGeant4Config.getPassBackGeant4SimSvc",                "ISF_PassBackGeant4SimSvc")
addService("ISF_Config.ISF_Geant4CommonConfig.getAFIIGeant4SimSvc",                     "ISF_AFIIGeant4SimSvc")
addService("ISF_Config.ISF_Geant4CommonConfig.getLongLivedGeant4SimSvc",                "ISF_LongLivedGeant4SimSvc")

addService("ISF_Example.ISF_iParticleKillerConfig.getParticleKillerSvc", "ISF_ParticleKillerSvc")

addAlgorithm("ISF_Config.ISF_MainConfig.getInput_GenericGenerator",     "ISF_Input_GenericGenerator")
addAlgorithm("ISF_Config.ISF_MainConfig.getInput_GenericFiles",         "ISF_Input_GenericFiles")
addAlgorithm("ISF_Config.ISF_MainConfig.getInput_geantinos",            "ISF_Input_geantinos")
addAlgorithm("ISF_Config.ISF_MainConfig.getInput_electrons",            "ISF_Input_electrons")
addAlgorithm("ISF_Config.ISF_MainConfig.getInput_muons",                "ISF_Input_muons")
addAlgorithm("ISF_Config.ISF_MainConfig.getInput_pions",                "ISF_Input_pions")
addAlgorithm("ISF_Config.ISF_MainConfig.getInput_ttbar",                "ISF_Input_ttbar")
addAlgorithm("ISF_Config.ISF_MainConfig.getInput_photons",              "ISF_Input_photons")
addAlgorithm("ISF_Config.ISF_MainConfig.getInput_minbias",              "ISF_Input_minbias")
addAlgorithm("ISF_Config.ISF_MainConfig.getInput_MultiParticleTest",    "ISF_Input_MultiParticleTest")

addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_GenericSimulator",    "ISF_Kernel_GenericSimulator")
#addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_FromJobProperties",   "ISF_Kernel_FromJobProperties")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_MC12G4",              "ISF_Kernel_MC12G4")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_MC12G4_longLived",    "ISF_Kernel_MC12G4_longLived")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_PassBackG4",          "ISF_Kernel_PassBackG4")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_FullG4",              "ISF_Kernel_FullG4")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_MC12G4_IDOnly",       "ISF_Kernel_MC12G4_IDOnly")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_MC12G4_IDCalo",       "ISF_Kernel_MC12G4_IDCalo")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_CosmicsG4",           "ISF_Kernel_CosmicsG4")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_ATLFASTII",           "ISF_Kernel_ATLFASTII")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_ATLFASTIIF",          "ISF_Kernel_ATLFASTIIF")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_ATLFASTIIF_PileUp",   "ISF_Kernel_ATLFASTIIF_PileUp")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_ATLFASTIIF_IDOnly",   "ISF_Kernel_ATLFASTIIF_IDOnly")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_ATLFASTIIF_IDCalo",   "ISF_Kernel_ATLFASTIIF_IDCalo")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_Fatras_newExtrapolation","ISF_Kernel_Fatras_newExtrapolation")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_Fatras_newExtrapolation_IDOnly","ISF_Kernel_Fatras_newExtrapolation_IDOnly")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_FastOnly",            "ISF_Kernel_FastOnly")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_MultiSimTest",        "ISF_Kernel_MultiSimTest")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_G4GammaCones"                , "ISF_Kernel_G4GammaCones"                 )
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_G4GammaCones_FastCalo"       , "ISF_Kernel_G4GammaCones_FastCalo"        )
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_FatrasGammaCones_FastCalo"   , "ISF_Kernel_FatrasGammaCones_FastCalo"    )
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_G4BHadronsOnly_FastCalo"     , "ISF_Kernel_G4BHadronsOnly_FastCalo"      )
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_G4BHadrons_FatrasID_FastCalo", "ISF_Kernel_G4BHadrons_FatrasID_FastCalo" )
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_FatrasBHadronsOnly_FastCalo" , "ISF_Kernel_FatrasBHadronsOnly_FastCalo"  )
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_G4TauProductsOnly"           , "ISF_Kernel_G4TauProductsOnly"            )
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_G4HiggsLeptonsConeOnly"      , "ISF_Kernel_G4HiggsLeptonsConeOnly"       )
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_G4HiggsLeptons"              , "ISF_Kernel_G4HiggsLeptons" )
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_G4WLeptonsCone_FatrasID_FastCalo"    , "ISF_Kernel_G4WLeptonsCone_FatrasID_FastCalo"    )
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_G4ZLeptonsCone_FatrasID_FastCalo"    , "ISF_Kernel_G4ZLeptonsCone_FatrasID_FastCalo"    )
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_G4ZLeptonsConeSticky_FatrasID_FastCalo", "ISF_Kernel_G4ZLeptonsConeSticky_FatrasID_FastCalo"    )
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_G4ZDecayProducts_FatrasID_FastCalo"  , "ISF_Kernel_G4ZDecayProducts_FatrasID_FastCalo"  )
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_G4JPsiLeptonsCone_FatrasID_FastCalo" , "ISF_Kernel_G4JPsiLeptonsCone_FatrasID_FastCalo" )
