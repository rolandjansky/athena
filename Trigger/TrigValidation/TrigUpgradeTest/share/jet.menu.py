#
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#

# import flags
include("TrigUpgradeTest/testHLT_MT.py")

##########################################
# menu
##########################################
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep
from TrigUpgradeTest.jetMenuDefs import jetMenuSequence

jetSeq1 = jetMenuSequence()
step1=ChainStep("Step1_jet", [jetSeq1])

testChains  = [
    Chain(name='HLT_j85',  Seed="L1_J20",  ChainSteps=[step1]  ),
    Chain(name='HLT_j45', Seed="L1_J20",  ChainSteps=[step1]  )
    ]
    

##########################################
# CF construction
##########################################

##### Make all HLT #######
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import makeHLTTree
makeHLTTree(testChains)



##########################################  
# Some debug
##########################################  
from AthenaCommon.AlgSequence import dumpSequence
dumpSequence(topSequence)



