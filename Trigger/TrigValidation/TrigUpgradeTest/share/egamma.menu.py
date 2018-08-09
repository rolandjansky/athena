#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

# import flags
include("TrigUpgradeTest/testHLT_MT.py")

##########################################
# menu
##########################################
from TrigUpgradeTest.MenuComponents import Chain, ChainStep
from TrigUpgradeTest.egammaMenuDefs import fastCaloSequence, electronSequence

fastCaloStep= fastCaloSequence()
electronStep= electronSequence()

step1=ChainStep("Step1_etcut", [fastCaloStep])
step2=ChainStep("Step2_etcut", [electronStep])
testChains  = [
    Chain(name='HLT_e3_etcut1step', Seed="L1_EM3",  ChainSteps=[step1]  ),
    Chain(name='HLT_e3_etcut',      Seed="L1_EM3",  ChainSteps=[step1, step2]  ),
    Chain(name='HLT_e5_etcut',      Seed="L1_EM3",  ChainSteps=[step1, step2]  ),
    Chain(name='HLT_e7_etcut',      Seed="L1_EM3",  ChainSteps=[step1, step2]  )
    ]
    



#################################
# Configure L1Decoder
#################################

# provide a minimal menu information
if globalflags.InputFormat.is_bytestream():
   topSequence.L1DecoderTest.ctpUnpacker.OutputLevel=DEBUG
   topSequence.L1DecoderTest.roiUnpackers[0].OutputLevel=DEBUG

# map L1 decisions for menu
for unpack in topSequence.L1DecoderTest.roiUnpackers:
    if unpack.name() is "EMRoIsUnpackingTool":
        unpack.Decisions="L1EM"
        emUnpacker=unpack
    if unpack.name() is "MURoIsUnpackingTool":
        unpack.Decisions="L1MU"
        
for unpack in topSequence.L1DecoderTest.rerunRoiUnpackers:
    if unpack.name() is "EMRerunRoIsUnpackingTool":
        unpack.Decisions="RerunL1EM"
        unpack.SourceDecisions="L1EM"

for unpack in topSequence.L1DecoderTest.rerunRoiUnpackers:
    if unpack.name() is "EMRerunRoIsUnpackingTool":
        unpack.SourceDecisions="L1EM"
    if unpack.name() is "MURerunRoIsUnpackingTool":
        unpack.SourceDecisions="L1MU"
        
# this is a temporary hack to include new test chains
EnabledChainNamesToCTP = [str(n)+":"+c.name for n,c in enumerate(testChains)]
topSequence.L1DecoderTest.ctpUnpacker.CTPToChainMapping = EnabledChainNamesToCTP

EnabledElChains= [c.seed.strip().split("_")[1] +" : "+ c.name for c in testChains]
emUnpacker.ThresholdToChainMapping = EnabledElChains

topSequence.L1DecoderTest.prescaler.Prescales = ["HLT_e3_etcut:2", "HLT_2e3_etcut:2.5"]



##########################################
# CF construction
##########################################

##### Make all HLT #######
from TrigUpgradeTest.HLTCFConfig import makeHLTTree
makeHLTTree(testChains)



##########################################  
# Some debug
##########################################  
from AthenaCommon.AlgSequence import dumpSequence
dumpSequence(topSequence)



