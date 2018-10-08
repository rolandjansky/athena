# 
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration 
# 
#  OutputLevel: INFO < DEBUG < VERBOSE 
# 


# import flags
include("TrigUpgradeTest/testHLT_MT.py")

### for Desplaying StoreGate Dump ###
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.StoreGateSvc=Service("StoreGateSvc") 
ServiceMgr.StoreGateSvc.Dump=True 


### for Control Flow ###
 
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.CheckDependencies( True )
AlgScheduler.OutputLevel( DEBUG )
AlgScheduler.ShowDataDependencies( True )
AlgScheduler.setDataLoaderAlg( 'SGInputLoader' )


# ===============================================================================================
#               Setup CF(Control Flow)
# ===============================================================================================


# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import decisionTree_From_Chains
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, Chain, ChainStep


if  TriggerFlags.doMuon==True:
    ##########################################
    # menu
    ##########################################

    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep
    from TrigUpgradeTest.muMenuDefs import muFastStep, muCombStep, muonEFSAStep, muIsoStep, doL2SA, doL2CB, doEFSA, doL2ISO

    MenuChains  = []

    # step1
    step1mufast=ChainStep("Step1_mufast", [muFastStep])
    # step2
    step2muComb=ChainStep("Step2_muComb", [muCombStep])
    # step3
    step3muEFSA=ChainStep("Step3_muEFSA", [muonEFSAStep])
    step3muIso =ChainStep("Step3_muIso",  [muIsoStep])

    # add one chain wihtout tracking
    MenuChains += [Chain(name='HLT_mu6fast', Seed="L1_MU6",  ChainSteps=[step1mufast])]

    
    
    if TriggerFlags.doID==False:
        if doL2SA==True  and doL2CB==False and doEFSA==False and doL2ISO==False:           
            MenuChains += [Chain(name='HLT_mu6', Seed="L1_MU6",  ChainSteps=[step1mufast ])]
            MenuChains += [Chain(name='HLT_2mu6', Seed="L1_MU6", ChainSteps=[step1mufast ])]
        if doL2SA==True  and doL2CB==False and doEFSA==True  and doL2ISO==False:           
            MenuChains += [Chain(name='HLT_mu6', Seed="L1_MU6",  ChainSteps=[step1mufast, step3muEFSA ])]
            MenuChains += [Chain(name='HLT_2mu6', Seed="L1_MU6", ChainSteps=[step1mufast, step3muEFSA ])]

    elif TriggerFlags.doID==True:
        if doL2SA==True  and doL2CB==True  and doEFSA==False and doL2ISO==False:
            MenuChains += [Chain(name='HLT_mu6', Seed="L1_MU6",  ChainSteps=[step1mufast, step2muComb ])]
            MenuChains += [Chain(name='HLT_2mu6', Seed="L1_MU6", ChainSteps=[step1mufast, step2muComb ])]
        if doL2SA==True  and doL2CB==True  and doEFSA==True  and doL2ISO==False:
            MenuChains += [Chain(name='HLT_mu6', Seed="L1_MU6",  ChainSteps=[step1mufast, step2muComb, step3muEFSA ])]
            MenuChains += [Chain(name='HLT_2mu6', Seed="L1_MU6", ChainSteps=[step1mufast, step2muComb, step3muEFSA ])]           
        if doL2SA==True  and doL2CB==True  and doEFSA==False and doL2ISO==True:
            MenuChains += [Chain(name='HLT_mu6', Seed="L1_MU6",  ChainSteps=[step1mufast, step2muComb, step3muIso ])]
            MenuChains += [Chain(name='HLT_2mu6', Seed="L1_MU6", ChainSteps=[step1mufast, step2muComb, step3muIso ])]
            
        if doL2SA==True  and doL2CB==True  and doEFSA==True and doL2ISO==True:
            MenuChains += [Chain(name='HLT_mu6', Seed="L1_MU6",  ChainSteps=[step1mufast, step2muComb, step3muIso ])]
            MenuChains += [Chain(name='HLT_2mu6', Seed="L1_MU6", ChainSteps=[step1mufast, step2muComb, step3muIso ])]


    #################################
    # configure L1Decoder
    #################################
      

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

    # map L1 decisions for menu
    for unpack in topSequence.L1DecoderTest.roiUnpackers:
        if unpack.name() is "EMRoIsUnpackingTool":
            unpack.Decisions="L1EM"
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
    EnabledChainNamesToCTP = dict([ (c.name, c.seed)  for c in MenuChains])
    topSequence.L1DecoderTest.ChainToCTPMapping = EnabledChainNamesToCTP

    #EnabledMuChains= [c.seed.strip().split("_")[1] +" : "+ c.name for c in MenuChains]
    #muUnpacker.ThresholdToChainMapping = EnabledMuChains


    ##########################################
    # CF construction
    ##########################################

    ##### Make all HLT #######
    from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import makeHLTTree
    makeHLTTree(MenuChains)
