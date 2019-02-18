#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

# import flags
include("TrigUpgradeTest/testHLT_MT.py")

##########################################
# menu
##########################################
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep
# from TrigUpgradeTest.egammaMenuDefs import fastCaloSequence, electronSequence

# fastCaloStep= fastCaloSequence()
# electronStep= electronSequence()

# step1=ChainStep("Step1_etcut", [fastCaloStep])
# step2=ChainStep("Step2_etcut", [electronStep])
# testChains  = [
#     Chain(name='HLT_e3_etcut1step', Seed="L1_EM3",  ChainSteps=[step1]  ),
#     Chain(name='HLT_e3_etcut',      Seed="L1_EM3",  ChainSteps=[step1, step2]  ),
#     Chain(name='HLT_e5_etcut',      Seed="L1_EM3",  ChainSteps=[step1, step2]  ),
#     Chain(name='HLT_e7_etcut',      Seed="L1_EM3",  ChainSteps=[step1, step2]  )
#     ]

from TrigUpgradeTest.tauMenuDefs import tauCaloSequence
#, tauCaloRecSequence
step1=ChainStep("Step1_tau", [tauCaloSequence()])
#step2=ChainStep("Step2_taucalorec", [tauCaloRecSequence()])




testChains  = [
    Chain(name='HLT_tau0_perf_ptonly_L1TAU12',  Seed="L1_TAU12",  ChainSteps=[step1] ),
    Chain(name='HLT_tau25_medium1_tracktwo', Seed="L1_TAU12IM",  ChainSteps=[step1] ),
    ]
    



# ##########################################
# # CF construction
# ##########################################

# ##### Make all HLT #######
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import makeHLTTree
makeHLTTree(testChains)



# ##########################################  
# # Some debug
# ##########################################  
from AthenaCommon.AlgSequence import dumpSequence
dumpSequence(topSequence)



