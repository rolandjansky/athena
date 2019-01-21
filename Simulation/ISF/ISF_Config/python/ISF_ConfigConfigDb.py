# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Configuration database for ISF
G Tan, 17/06/2012
"""

from AthenaCommon.CfgGetter import addAlgorithm

# Common algorithms used by jobs
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
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_FullG4",              "ISF_Kernel_FullG4")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_FullG4_LongLived",    "ISF_Kernel_FullG4_LongLived")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_FullG4_IDOnly",       "ISF_Kernel_FullG4_IDOnly")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_FullG4_IDCalo",       "ISF_Kernel_FullG4_IDCalo")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_StoppingG4",          "ISF_Kernel_StoppingG4")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_MC12G4",              "ISF_Kernel_MC12G4")# Legacy only
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_MC12G4_longLived",    "ISF_Kernel_MC12G4_longLived")# Legacy only
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_MC12G4_IDOnly",       "ISF_Kernel_MC12G4_IDOnly")# Legacy only
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_MC12G4_IDCalo",       "ISF_Kernel_MC12G4_IDCalo")# Legacy only
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_PassBackG4",          "ISF_Kernel_PassBackG4")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_CosmicsG4",           "ISF_Kernel_CosmicsG4")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_ATLFASTII",           "ISF_Kernel_ATLFASTII")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_ATLFASTIIF",          "ISF_Kernel_ATLFASTIIF")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_ATLFASTIIF_PileUp",   "ISF_Kernel_ATLFASTIIF_PileUp")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_ATLFASTII_PileUp",   "ISF_Kernel_ATLFASTII_PileUp")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_G4HS_FastPileup",     "ISF_Kernel_G4HS_FastPileup")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_ATLFASTIIF_IDOnly",   "ISF_Kernel_ATLFASTIIF_IDOnly")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_ATLFASTIIF_IDCalo",   "ISF_Kernel_ATLFASTIIF_IDCalo")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_G4FastCalo",          "ISF_Kernel_G4FastCalo")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_G4FastCaloDNN",          "ISF_Kernel_G4FastCaloDNN")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_Fatras_newExtrapolation","ISF_Kernel_Fatras_newExtrapolation")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_Fatras_newExtrapolation_IDOnly","ISF_Kernel_Fatras_newExtrapolation_IDOnly")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_FastOnly",            "ISF_Kernel_FastOnly")
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_DNNOnly",            "ISF_Kernel_DNNOnly")
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
addAlgorithm("ISF_Config.ISF_MainConfig.getKernel_ParametricSimulation",                 "ISF_Kernel_ParametricSimulation"                )
