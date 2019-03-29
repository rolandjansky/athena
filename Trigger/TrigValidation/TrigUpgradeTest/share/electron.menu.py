#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

# import flags
include("TrigUpgradeTest/testHLT_MT.py")

##########################################
# menu
##########################################
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep
from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import fastCaloMenuSequence
from TriggerMenuMT.HLTMenuConfig.Egamma.ElectronSequenceSetup import electronMenuSequence, inDetSetup
from TriggerMenuMT.HLTMenuConfig.CommonSequences.precisionCaloSequenceSetup import precisionCaloMenuSequence

inDetSetup()

fastCaloStep= fastCaloMenuSequence("Ele")
electronStep= electronMenuSequence()
electronPrecisionStep= precisionCaloMenuSequence()

step1=ChainStep("Step1_etcut", [fastCaloStep])
step2=ChainStep("Step2_etcut", [electronStep])
step3=ChainStep("Step3_etcut", [electronPrecisionStep])

testChains  = [
    Chain(name='HLT_e3_etcut',      Seed="L1_EM3",  ChainSteps=[step1, step2, step3]  ),
    Chain(name='HLT_e15_etcut',      Seed="L1_EM3",  ChainSteps=[step1, step2, step3]  )
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







