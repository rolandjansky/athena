#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

# import flags
include("TrigUpgradeTest/testHLT_MT.py")


##########################################
# menu
##########################################
from TrigUpgradeTest.MenuComponents import Chain, ChainStep


EnabledElChains = []
EnabledMuChains = []
EnabledMuComboChains = []
EnabledElComboChains = []


# egamma chains
from TrigUpgradeTest.egammaMenuDefs import fastCaloSequence, electronSequence
fastCaloStep=fastCaloSequence()
electronStep=electronSequence()

step1=ChainStep("Step1_etcut", [fastCaloStep])
step2=ChainStep("Step2_etcut", [electronStep])
egammaChains  = [
    Chain(name='HLT_e3_etcut1step', Seed="L1_EM3",  ChainSteps=[step1]  ),
    Chain(name='HLT_e3_etcut',      Seed="L1_EM3",  ChainSteps=[step1, step2]  ),
    Chain(name='HLT_e5_etcut',      Seed="L1_EM3",  ChainSteps=[step1, step2]  ),
    Chain(name='HLT_e7_etcut',      Seed="L1_EM3",  ChainSteps=[step1, step2]  )
    ]
 


# muon chains
from TrigUpgradeTest.muMenuDefs import muFastStep, muCombStep, doL2SA, doL2CB, doEFSA
MuonChains  = []
step1mufast=ChainStep("Step1_mufast", [muFastStep])

MuonChains += [Chain(name='HLT_mu6', Seed="L1_MU6",  ChainSteps=[step1mufast ])]
MuonChains += [Chain(name='HLT_2mu6', Seed="L1_MU6", ChainSteps=[step1mufast ])]
if TriggerFlags.doID==True:
    step2muComb=ChainStep("Step2_muComb", [muCombStep])
    MuonChains += [Chain(name='HLT_mu6Comb', Seed="L1_MU6",  ChainSteps=[step1mufast, step2muComb ])]
    MuonChains += [Chain(name='HLT_2mu6Comb', Seed="L1_MU6", ChainSteps=[step1mufast, step2muComb ])]

    
# combo chains
comboChains= []
comboStep=ChainStep("Step1_mufast_et", [muFastStep,fastCaloStep])
comboChains +=  [Chain(name='HLT_mu6_e3_etcut', Seed="L1_MU6_EM3",  ChainSteps=[comboStep ])]


# sum all
testChains = egammaChains + MuonChains
#+ comboChains


#################################
# Configure L1Decoder
#################################


# provide a minimal menu information
if globalflags.InputFormat.is_bytestream():
    topSequence.L1DecoderTest.ctpUnpacker.OutputLevel=DEBUG
    topSequence.L1DecoderTest.roiUnpackers[0].OutputLevel=DEBUG
    topSequence.L1DecoderTest.roiUnpackers[1].OutputLevel=DEBUG


    # map L1 decisions for menu
for unpack in topSequence.L1DecoderTest.roiUnpackers:
    if unpack.name() is "EMRoIsUnpackingTool":
        unpack.Decisions="L1EM"
        emUnpacker=unpack
    if unpack.name() is "MURoIsUnpackingTool":
        unpack.Decisions="L1MU"
        muUnpacker=unpack
        
for unpack in topSequence.L1DecoderTest.rerunRoiUnpackers:
    if unpack.name() is "EMRerunRoIsUnpackingTool":
        unpack.Decisions="RerunL1EM"
        unpack.SourceDecisions="L1EM"
   
    if unpack.name() is "MURerunRoIsUnpackingTool":
        unpack.Decisions="RerunL1MU"
        unpack.SourceDecisions="L1MU"
  
# this is a temporary hack to include new test chains
EnabledChainNamesToCTP = [str(n)+":"+c.name for n,c in enumerate(testChains)]
topSequence.L1DecoderTest.ctpUnpacker.CTPToChainMapping = EnabledChainNamesToCTP

EnabledElChains= [c.seed.strip().split("_")[1] +" : "+ c.name for c in egammaChains]
EnabledMuChains= [c.seed.strip().split("_")[1] +" : "+ c.name for c in MuonChains]
for c in comboChains:
        seeds=c.seed.split("_")
        seeds.pop(0) #remove first L1 string
        for s in seeds:
            if "MU" in s: EnabledMuComboChains.append(s +" : "+ c.name)
            if "EM" in s: EnabledElComboChains.append(s +" : "+ c.name) 

muUnpacker.ThresholdToChainMapping = EnabledMuChains + EnabledMuComboChains
emUnpacker.ThresholdToChainMapping = EnabledElChains + EnabledElComboChains
#topSequence.L1DecoderTest.prescaler.Prescales = ["HLT_e3_etcut:2", "HLT_2e3_etcut:2.5"]


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



