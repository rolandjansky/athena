#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

# import flags
include("TrigUpgradeTest/testHLT_MT.py")

##########################################
# menu
##########################################
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep

from TrigUpgradeTest.CaloMenuDefs import fastCaloMenuSequence
from TrigUpgradeTest.electronMenuDefs import electronMenuSequence,  inDetSetup

inDetSetup()

fastCaloStep= fastCaloMenuSequence("Ele")
electronStep= electronMenuSequence()

step1=ChainStep("Step1_etcut", [fastCaloStep])
step2=ChainStep("Step2_etcut", [electronStep])
testChains  = [
    Chain(name='HLT_e3_etcut1step', Seed="L1_EM3",  ChainSteps=[step1]  ),
    Chain(name='HLT_e3_etcut',      Seed="L1_EM3",  ChainSteps=[step1, step2]  ),
    Chain(name='HLT_e5_etcut',      Seed="L1_EM3",  ChainSteps=[step1, step2]  ),
    Chain(name='HLT_e7_etcut',      Seed="L1_EM3",  ChainSteps=[step1, step2]  )
    ]
    


topSequence.L1DecoderTest.prescaler.Prescales = ["HLT_e3_etcut:2", "HLT_2e3_etcut:2.5"]



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



