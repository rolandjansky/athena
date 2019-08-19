#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

# import flags
include("TrigUpgradeTest/testHLT_MT.py")

##########################################
# menu
##########################################
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep

# We should retrieve all the steps here
from TriggerMenuMT.HLTMenuConfig.Bjet.BjetSequenceSetup import getBJetSequence

step1 = ChainStep("Step1ALLTE_bjet", [getBJetSequence('jALLTE')])
step2 = ChainStep("Step2ALLTE_bjet", [getBJetSequence('gscALLTE')])

testChains  = [                                                                                                                                                                         
    Chain(name='HLT_j35_gsc45_boffperf_split_L1J20' ,  L1Thresholds=["J20"], ChainSteps=[step1,step2] ),
    Chain(name='HLT_j35_gsc45_bmv2c1070_split_L1J20',  L1Thresholds=["J20"], ChainSteps=[step1,step2] ),
    Chain(name='HLT_j35_gsc45_bmv2c1070_L1J20'      ,  L1Thresholds=["J20"], ChainSteps=[step1,step2] )
    ]


#################################
# Configure L1Decoder
#################################
topSequence.L1Decoder.prescaler.Prescales = ["HLT_j35_gsc45_boffperf_split_L1J20:1",
                                             "HLT_j35_gsc45_bmv2c1070_split_L1J20:1",
                                             "HLT_j35_gsc45_bmv2c1070_L1J20:1"]


##### Make all HLT #######
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import makeHLTTree
makeHLTTree(testChains)

##########################################  
# Some debug
##########################################  
from AthenaCommon.AlgSequence import dumpSequence
dumpSequence(topSequence)



