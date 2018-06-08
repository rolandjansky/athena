# 
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration 
# 
#  OutputLevel: INFO < DEBUG < VERBOSE 
# 
  
include("TrigUpgradeTest/testHLT_MT.py") 


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

### If inputFile is BS(ByteStream), the bool is true. ###
isData = False 
if globalflags.InputFormat.is_bytestream():
  isData = True

### provide a minimal menu information ###
if isData:
  topSequence.L1DecoderTest.ctpUnpacker.OutputLevel=DEBUG
  topSequence.L1DecoderTest.roiUnpackers[0].OutputLevel=DEBUG
  topSequence.L1DecoderTest.roiUnpackers[1].OutputLevel=DEBUG
else:
  pass
 
### for Desplaying StoreGate Dump ###
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.StoreGateSvc=Service("StoreGateSvc") 
ServiceMgr.StoreGateSvc.Dump=True 


# menu components   
from TrigUpgradeTest.HLTCFConfig import decisionTree_From_Chains
from TrigUpgradeTest.MenuComponents import HLTRecoSequence, MenuSequence, Chain, ChainStep

### for Control Flow ###
from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq
 
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.CheckDependencies( True )
AlgScheduler.OutputLevel( DEBUG )
AlgScheduler.ShowDataDependencies( True )
AlgScheduler.setDataLoaderAlg( 'SGInputLoader' )


from AthenaCommon.CfgGetter import getPublicTool, getPublicToolClone
from AthenaCommon import CfgMgr



# ===============================================================================================
#               Setup CF(Control Flow)
# ===============================================================================================

  ### CF construction ###
from TrigUpgradeTest.MenuComponents import Chain, ChainStep
from TrigUpgradeTest.egammaMenuDefs import fastCaloSequence, electronSequence




if  TriggerFlags.doMuon==True:
    ##########################################
    # menu
    ##########################################

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


    # menu
    from TrigUpgradeTest.MenuComponents import Chain, ChainStep
    from TrigUpgradeTest.muMenuDefs import muFastStep, doL2SA, doL2CB, doEFSA

    MenuChains  = []

    
    if TriggerFlags.doID==False:
        if doL2SA==True and doL2CB==False and doEFSA==False:           
            MenuChains += [Chain(name='HLT_mu6', Seed="L1_MU6",  ChainSteps=[ChainStep("Step1_mufast", [muFastStep]) ])]
            MenuChains += [Chain(name='HLT_2mu6', Seed="L1_MU6",  ChainSteps=[ChainStep("Step1_mufast", [muFastStep]) ])]
        if doEFSA==True and doL2SA==False and doL2CB==False:
            MenuChains += [Chain(name='HLT_mu6', Seed="L1_MU6",  ChainSteps=[ChainStep("Step1_mufast", [muFastStep]) ])]
    elif TriggerFlags.doID==True:    
        if doL2SA==True and doL2CB==True and doEFSA==False:
            MenuChains += [Chain(name='HLT_mu6', Seed="L1_MU6",  ChainSteps=[ChainStep("Step1_mufast", [muFastStep]) ])]
        if doL2SA==True and doEFSA==True and doL2CB==True:
            MenuChains += [Chain(name='HLT_mu6', Seed="L1_MU6",  ChainSteps=[ChainStep("Step1_mufast", [muFastStep]) ])]
            
    
    
            


    ##########################################
    # CF construction
    ##########################################

    ##### Make all HLT #######
    from TrigUpgradeTest.HLTCFConfig import makeHLTTree
    makeHLTTree(MenuChains)
