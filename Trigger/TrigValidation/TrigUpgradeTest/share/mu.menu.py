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
    from TrigUpgradeTest.muMenuDefs import muFastStep, muCombStep, muEFMSStep, muEFSAStep, muIsoStep, muEFCBStep, inDetSetup

    inDetSetup()

    MenuChains  = []

    # step1
    step1mufast=ChainStep("Step1_mufast", [ muFastStep() ])
    # step2
    step2muComb=ChainStep("Step2_muComb", [ muCombStep() ])
    step2muEFMS=ChainStep("Step2_muEFMS", [ muEFMSStep() ])
    # step3
    step3muEFSA=ChainStep("Step3_muEFSA", [ muEFSAStep() ])
    step3muIso =ChainStep("Step3_muIso",  [ muIsoStep() ])
    # step4
    step4muEFCB=ChainStep("Step4_muEFCB", [ muEFCBStep() ])

    ## single muon trigger  
    MenuChains += [Chain(name='HLT_mu6fast',   Seed="L1_MU6",  ChainSteps=[ step1mufast ])]
    MenuChains += [Chain(name='HLT_mu6Comb',   Seed="L1_MU6",  ChainSteps=[ step1mufast, step2muComb ])]
    #MenuChains += [Chain(name='HLT_mu6msonly', Seed="L1_MU6",  ChainSteps=[ step1mufast, step2muEFMS ])] # removed due to muEFSA isuue(?)
    MenuChains += [Chain(name='HLT_mu6',       Seed="L1_MU6",  ChainSteps=[ step1mufast, step2muComb, step3muEFSA, step4muEFCB ])]
    MenuChains += [Chain(name='HLT_mu20_ivar', Seed="L1_MU6", ChainSteps=[ step1mufast, step2muComb, step3muIso ])]

    # multi muon trigger 
    MenuChains += [Chain(name='HLT_2mu6Comb', Seed="L1_MU6", ChainSteps=[ step1mufast, step2muComb ])]
    MenuChains += [Chain(name='HLT_2mu6',     Seed="L1_MU6", ChainSteps=[ step1mufast, step2muComb, step3muEFSA, step4muEFCB ])]        
 
    
    #################################
    # configure L1Decoder
    #################################
      

    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    ### If inputFile is BS(ByteStream), the bool is true. ###
    isData = False 
    if globalflags.InputFormat.is_bytestream():
        isData = True
 
           
    # this is a temporary hack to include new test chains
    EnabledChainNamesToCTP = dict([ (c.name, c.seed)  for c in MenuChains])
    topSequence.L1DecoderTest.ChainToCTPMapping = EnabledChainNamesToCTP
  

    ##########################################
    # CF construction
    ##########################################

    ##### Make all HLT #######
    from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import makeHLTTree
    makeHLTTree(MenuChains)
