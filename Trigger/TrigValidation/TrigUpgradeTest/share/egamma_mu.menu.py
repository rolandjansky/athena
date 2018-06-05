#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

include("TrigUpgradeTest/testHLT_MT.py")


 
# provide a minimal menu information
if globalflags.InputFormat.is_bytestream():
    topSequence.L1DecoderTest.ctpUnpacker.OutputLevel=DEBUG
    topSequence.L1DecoderTest.roiUnpackers[0].OutputLevel=DEBUG
    topSequence.L1DecoderTest.roiUnpackers[1].OutputLevel=DEBUG

    # map L1 decisions for menu
for unpack in topSequence.L1DecoderTest.roiUnpackers:
    if unpack.name() is "EMRoIsUnpackingTool":
        unpack.Decisions="L1EM"
    if unpack.name() is "MURoIsUnpackingTool":
        unpack.Decisions="L1MU"
        
for unpack in topSequence.L1DecoderTest.rerunRoiUnpackers:
    if unpack.name() is "EMRerunRoIsUnpackingTool":
        unpack.Decisions="RerunL1EM"
        unpack.SourceDecisions="L1EM"
   
    if unpack.name() is "MURerunRoIsUnpackingTool":
        unpack.Decisions="RerunL1MU"
        unpack.SourceDecisions="L1MU"
  



##########################################
# menu
##########################################
from TrigUpgradeTest.MenuComponents import Chain, ChainStep
from TrigUpgradeTest.egammaMenuDefs import fastCaloSequence, electronSequence
from TrigUpgradeTest.muMenuDefs import muFastStep, doL2SA, doL2CB, doEFSA

egammaChains  = [
   Chain(name='HLT_e3_etcut', Seed="L1_EM3",   \
             ChainSteps=[ ChainStep("Step1_e3_etcut", [fastCaloSequence]),
                          ChainStep("Step2_e3_etcut", [electronSequence])]  ),
    Chain(name='HLT_e5_etcut', Seed="L1_EM3",   \
              ChainSteps=[ChainStep("Step1_e5_etcut", [fastCaloSequence])]),
    Chain(name='HLT_e7_etcut', Seed="L1_EM3",   \
              ChainSteps=[ChainStep("Step1_e7_etcut", [fastCaloSequence])]),
    ]

MuonChains = []
if TriggerFlags.doID==False:
    if doL2SA==True and doL2CB==False and doEFSA==False:           
        MuonChains += [Chain(name='HLT_mu6', Seed="L1_MU6",  ChainSteps=[ChainStep("Step1_mufast", [muFastStep]) ])]
        MuonChains += [Chain(name='HLT_2mu6', Seed="L1_MU6",  ChainSteps=[ChainStep("Step1_mufast", [muFastStep]) ])]
    if doEFSA==True and doL2SA==False and doL2CB==False:
        MuonChains += [Chain(name='HLT_mu6', Seed="L1_MU6",  ChainSteps=[ChainStep("Step1_mufast", [muFastStep]) ])]
elif TriggerFlags.doID==True:    
    if doL2SA==True and doL2CB==True and doEFSA==False:
        MuonChains += [Chain(name='HLT_mu6', Seed="L1_MU6",  ChainSteps=[ChainStep("Step1_mufast", [muFastStep]) ])]
    if doL2SA==True and doEFSA==True and doL2CB==True:
        MuonChains += [Chain(name='HLT_mu6', Seed="L1_MU6",  ChainSteps=[ChainStep("Step1_mufast", [muFastStep]) ])]
            

testChains = egammaChains + MuonChains
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



