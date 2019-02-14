#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

include("TrigUpgradeTest/testHLT_MT.py")


##########################################
# menu
##########################################
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep
from TrigUpgradeTest.CaloMenuDefs import fastCaloMenuSequence
from TrigUpgradeTest.photonMenuDefs import photonMenuSequence

calostep=fastCaloMenuSequence("Gamma")
photonstep= photonMenuSequence()


photonChains = [
   Chain(name='HLT_g5_etcut', Seed="L1_EM3",   \
             ChainSteps=[ ChainStep("Step1_g5_etcut", [calostep]),
                          ChainStep("Step2_g5_etcut", [photonstep])]  )
      ]

testChains = photonChains


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



