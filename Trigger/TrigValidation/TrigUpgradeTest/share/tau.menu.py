#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

# import flags
include("TrigUpgradeTest/testHLT_MT.py")

##########################################
# menu
##########################################
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep
from TrigUpgradeTest.tauMenuDefs import inDetSetup

inDetSetup()

from TrigUpgradeTest.tauMenuDefs import getTauSequence
step1=ChainStep("Step1_tau", [getTauSequence('calo')])
step2=ChainStep("Step2_tau", [getTauSequence('track_core')])


testChains  = [
    Chain(name='HLT_tau0_perf_ptonly_L1TAU12',  Seed="L1_TAU12",  ChainSteps=[step1, step2] ),
    Chain(name='HLT_tau25_medium1_tracktwo', Seed="L1_TAU12IM",  ChainSteps=[step1, step2] ),
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



