# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgScheduler import AlgScheduler
from AthenaCommon.Logging import logging
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from HLTSeeding.HLTSeedingConf import CTPUnpackingEmulationTool, RoIsUnpackingEmulationTool
from TriggerMenuMT.HLT.Menu.MenuComponents import EmptyMenuSequence
log = logging.getLogger('EmuStepProcessingConfig')

###########################################################################    
def thresholdToChains( chains ):
    """
    Produces list "threshod : chain" for all chains passed. Uses the L1Thresholds/vseeds Chain property
    """
    ret = []
    for c in chains:
        for t in c.vseeds:
            ret.append(t+ " : " + c.name)
    return ret

###########################################################################    
def generateHLTSeedingAndChainsManually(topSequence):
    log.info( "generateHLTSeedingAndChainsManually")
    generateEmuEvents()
    emulateHLTSeeding(topSequence)
    generateChainsManually()
    

###########################################################################    
def generateHLTSeedingAndChainsByMenu(topSequence):
    log.info("generateHLTSeedingAndChainsByMenu")
    generateEmuEvents()
    emulateHLTSeeding(topSequence)
    generateEmuMenu()



###########################################################################    
def generateEmuMenu(): 
    """ 
     set Emu menu and reproduce generateMT
    """
    log.info("generateEmuMenu")  
    from TriggerMenuMT.HLT.Menu import Dev_pp_run3_v1
    from TriggerMenuMT.HLT.Menu import Dev_pp_run3_emu_v1 
    from TriggerMenuMT.HLT.Menu.GenerateMenuMT import GenerateMenuMT

    # overwrite Dev_pp_run3_v1
    Dev_pp_run3_v1.setupMenu = Dev_pp_run3_emu_v1.setupMenu

    # Generate the menu
    menu = GenerateMenuMT()
    menu.generateAllChainConfigs(ConfigFlags)
    #menu.generateMT()



###########################################################################    
def generateEmuEvents():
    log.info("generateEmuEvents")
    AlgScheduler.ShowControlFlow( True )
    AlgScheduler.ShowDataFlow( True )

    # 4 events
    data = {
        'noreco': [';', ';', ';',';'],
        'emclusters': [';', ';', ';',';'],
        'msmu': [';', ';', ';',';'],
        'ctp': [';', ';', ';',';'],
        'l1emroi': [';', ';', ';',';'],
        'l1muroi': [';', ';', ';',';']
        }  # in the lists there are the events

    # event 0: empty
    data['ctp'] [0]      =  'HLT_TestChain5_ev1_L1EM3 \
                             HLT_TestChain8_ev1_L1EM3 \
                             HLT_g5_EM7'
    data['l1emroi'][0]   = ';'
    data['emclusters'][0]= ';'
    data['l1muroi'][0]   = ';'
    data['msmu'][0]      = ';'


    #event 1: 3e (1 not passing at L1, 1 not passing at step1) + 2mu (2 not passing) - HLT_e5_e8_L12EM3 HLT_2TestChain6_muv1_L12MU5VF
    data['ctp'] [1]      =  'HLT_TestChain5_ev1_L1EM3 \
                             HLT_TestChain8_ev1_L1EM3 \
                             HLT_TestChain5_gv1_L1EM7 \
                             HLT_TestChain5_ev3_L1EM7 \
                             HLT_2TestChain6_muv1_L12MU5VF \
                             HLT_TestChain10_muv2_L1MU8F \
                             HLT_TestChain6_muv1_TestChain10_muv1_L12MU5VF \
                             HLT_2TestChain4_muv1_dr_L12MU5VF'
    data['l1emroi'][1]   =  '1,1,0,EM3,EM7,EM20,EM30,EM100; 2.,-1.2,0,EM3,EM7; 3.,0.2,0,EM3;'
    data['emclusters'][1]=  'eta:1,phi:1,et:180000; eta:1,phi:-1.2,et:6000; eta:0.5,phi:-1.2,et:3000;'
    data['l1muroi'][1]   =  '2,0.5,0,MU5VF; 3,0.5,0,MU5VF;'
    data['msmu'][1]      = 'eta:-1.2,phi:0.7,pt:1500,pt2:1500; eta:-1.1,phi:0.6,pt:1500,pt2:1500;'

    # event 2: 2e+ 3mu : HLT_TestChain5_ev1_TestChain8_ev1_2TestChain6_muv1_L1_2EM8VH_MU8F, HLT_TestChain6_muv1_TestChain10_ev1_L1EM7_MU8F
    data['ctp'] [2]      =  'HLT_TestChain6_muv1_L1MU5VF \
                             HLT_TestChain8_muv1_L1MU8F \
                             HLT_TestChain10_muv2_L1MU8F \
                             HLT_TestChain8_muv1step_L1MU5VF \
                             HLT_TestChain5_ev1_L1EM3 \
                             HLT_TestChain8_ev1_L1EM3 \
                             HLT_TestChain6_muEmpty2_L1MU5VF \
                             HLT_TestChain6_muv1_TestChain10_ev1_L1EM7_MU8F \
                             HLT_TestChain6_muv2_TestChain8_ev2_L1EM7_MU8F \
                             HLT_2TestChain6_muv1_L12MU5VF \
                             HLT_2TestChain6_muEmpty1_L12MU5VF \
                             HLT_TestChain6_muv1_TestChain10_muv1_L12MU5VF \
                             HLT_2TestChain4_muv1_dr_L12MU5VF HLT_e5_e8_L12EM3 \
                             HLT_TestChain5_ev1_TestChain8_ev1_2TestChain6_muv1_L12EM8VH_MU8F \
                             HLT_TestChain10_muEmpty1_TestChain6_muEmpty1_L12MU5VF \
                             HLT_TestChain10_muv1_TestChain6_muEmpty1_L1MU5VF \
                             HLT_TestChain5_ev1_TestChain8_ev1_merge_L12EM3 \
                             HLT_TestChain5_ev1_TestChain8_ev1_L12EM3  \
                             HLT_TestChain6_muv1_TestChain5_ev1_dr_L12MU5VF'
    data['l1emroi'][2]   =  '2,0.2,0,EM3,EM7,EM15,EM20,EM30,EM100; 1,-1.1,0,EM3,EM7,EM15,EM20,EM30;'
    data['emclusters'][2]=  'eta:0.5,phi:0,et:120000; eta:1,phi:-1.2,et:65000;'
    data['l1muroi'][2]   =  '2,0.5,0,MU5VF,MU8VF; 3,0.5,0,MU5VF,MU8F,MU8VF;2.2,0.6,0,MU5VF;'
    data['msmu'][2]      =  'eta:-1.2,phi:0.7,pt:6500,pt2:8500; eta:-1.1,phi:0.6,pt:10500,pt2:8500;eta:-1.1,phi:0.6,pt:8500,pt2:8500;'

    #event 3: 1e + 1mu; HLT_TestChain6_muv1_TestChain10_ev1_L1EM7_MU8F does not pass because of e10
    data['ctp'] [3]      =  'HLT_TestChain20_muv1_L1MU8F \
                             HLT_TestChain10_muv1_L1MU8F \
                             HLT_TestChain8_muv1_L1MU8F \
                             HLT_TestChain8_muv1step_L1MU5VF \
                             HLT_TestChain8_ev1_L1EM3 \
                             HLT_TestChain6_muv1_TestChain10_ev1_L1EM7_MU8F\
                             HLT_TestChain6_muv2_TestChain8_ev2_L1EM7_MU8F'
    data['l1emroi'][3]   =  '1,1.5,0,EM3,EM7;'
    data['emclusters'][3]=  'eta:-0.6,phi:1.7,et:9000;'
    data['l1muroi'][3]   =  '2,-0.1,0,MU5VF,MU8F,MU8VF;'
    data['msmu'][3]      =  'eta:-1.7,phi:-0.2,pt:29500,pt2:8500;'

    # otehr vectors

    data['tracks'] = ['eta:1,phi:1,pt:120000; eta:1,phi:-1.2,et:32000;',
                      'eta:1,phi:1,pt:120000; eta:1,phi:-1.2,et:32000;',
                      'eta:0.5,phi:0,pt:130000; eta:1,phi:-1.2,pt:60000;eta:-1.2,phi:0.7,pt:6700; eta:-1.1,phi:0.6,pt:8600;',
                      'eta:-0.6,phi:1.7,et:9000;'] # no MU track for MS candidate 'eta:-1.7,phi:-0.2,pt:9500;'

    data['mucomb'] = [';',
                      ';',
                      'eta:-1.2,phi:0.7,pt:6600; eta:-1.1,phi:0.6,pt:8600;',
                      ';']

    data['electrons'] = [';',
                         'eta:1,phi:1,pt:120000; eta:1,phi:-1.2,et:32000;',
                         ';',
                         ';']
    data['photons'] = [';',
                       'eta:1,phi:1,pt:130000;',
                       ';',
                       ';']

    from DecisionHandling.TestUtils import writeEmulationFiles
    writeEmulationFiles(data)


###########################################################################    
def generateChainsManually():
    log.info("generateChainsManually")
    from DecisionHandling.TestUtils import makeChain, makeChainStep    
    doMuon     = True
    doElectron = True
    doCombo    = True

    HLTChains = []

    # muon chains
    if doMuon:
        from DecisionHandling.HLTSignatureConfig import  muMenuSequence
        #step1
        mu11 = muMenuSequence(step="1",reconame="v1", hyponame="v1")
        mu12 = muMenuSequence(step="1",reconame="v2", hyponame="v2")
                    
        #step2
        mu21 = muMenuSequence(step="2",reconame="v1", hyponame="v1")
        mu22 = muMenuSequence(step="2",reconame="v2", hyponame="v2")
        #step3
        mu31 = muMenuSequence(step="3",reconame="v1", hyponame="v1")
        mu32 = muMenuSequence(step="3",reconame="v2", hyponame="v2")
        #step4
        mu41 = muMenuSequence(step="4",reconame="v1", hyponame="v1")

        step_mu11  = makeChainStep("Step1_mu11", [mu11] )
        step_mu21  = makeChainStep("Step2_mu21", [mu21] )
        step_mu22  = makeChainStep("Step2_mu22", [mu22] )
        step_mu31  = makeChainStep("Step3_mu31", [mu31] )
        step_mu32  = makeChainStep("Step3_mu32", [mu32] )
        step_mu41  = makeChainStep("Step4_mu41", [mu41] )
        
        step_empy= makeChainStep("Step2_mu1empty", multiplicity=[])

        MuChains  = [
            makeChain(name='HLT_TestChain8_muv1step_L1MU5VF', L1Thresholds=["MU5VF"],    ChainSteps=[step_mu11]),
            makeChain(name='HLT_TestChain8_muv1_L1MU8F',    L1Thresholds=["MU8F"],   ChainSteps=[step_mu11 , step_mu21 , step_mu31, step_mu41] ),
            makeChain(name='HLT_TestChain20_muv1_L1MU8F',   L1Thresholds=["MU8F"],   ChainSteps=[step_mu11 , step_mu21 , step_mu31, step_mu41] ),
            makeChain(name='HLT_TestChain10_muv2_L1MU8F',   L1Thresholds=["MU8F"],   ChainSteps=[step_mu11 , step_mu22 , step_mu31] ), 
            makeChain(name='HLT_TestChain6_muEmpty2_L1MU5VF', L1Thresholds=["MU5VF"],    ChainSteps=[step_mu11 , step_empy , step_mu32, step_mu41] ), 
            ]
            

        HLTChains += MuChains


    ## #electron chains
    if doElectron:
        from DecisionHandling.HLTSignatureConfig import  elMenuSequence, gamMenuSequence
        el11 = elMenuSequence(step="1",reconame="v1", hyponame="v1")
        el21 = elMenuSequence(step="2",reconame="v1", hyponame="v1")
        el22 = elMenuSequence(step="2",reconame="v2", hyponame="v2")
        el23 = elMenuSequence(step="2",reconame="v2", hyponame="v3")
        el31 = elMenuSequence(step="3",reconame="v1", hyponame="v1")
        el41 = elMenuSequence(step="4",reconame="v1", hyponame="v1")

        # gamma
        gamm11 = gamMenuSequence("1", reconame="v1", hyponame="v1")
    
        ElChains  = [
            makeChain(name='HLT_TestChain5_ev1_L1EM3', L1Thresholds=["EM3"], ChainSteps=[ makeChainStep("Step1_em11", [el11]), makeChainStep("Step2_em21",  [el21]), makeChainStep("Step3_em31",  [el31])] ),
            makeChain(name='HLT_TestChain8_ev1_L1EM3', L1Thresholds=["EM3"], ChainSteps=[ makeChainStep("Step1_em11", [el11]), makeChainStep("Step2_em21",  [el21]), makeChainStep("Step3_em31",  [el31]) ] ),
            makeChain(name='HLT_TestChain5_ev2_L1EM7', L1Thresholds=["EM7"], ChainSteps=[ makeChainStep("Step1_em11", [el11]), makeChainStep("Step2_em22",  [el22]) ] ),
            makeChain(name='HLT_TestChain5_ev3_L1EM7', L1Thresholds=["EM7"], ChainSteps=[ makeChainStep("Step1_em11", [el11]), makeChainStep("Step2_em23",  [el23]) ] ),
            makeChain(name='HLT_TestChain5_gv1_L1EM7', L1Thresholds=["EM7"], ChainSteps=[ makeChainStep("Step1_gam11", [gamm11]) ] )
        ]

        HLTChains += ElChains
        

    # combined chain
    if doCombo:
        emptySeq1 = EmptyMenuSequence("step1EmptySeqence")
        emptySeq2 = EmptyMenuSequence("step2EmptySeqence")
        
        if not doElectron:
            from DecisionHandling.HLTSignatureConfig import elMenuSequence        
            el11 = elMenuSequence(step="1",reconame="v1", hyponame="v1")    
            el21 = elMenuSequence(step="2",reconame="v1", hyponame="v1")
            el41 = elMenuSequence(step="4",reconame="v1", hyponame="v1")
            
        if not doMuon:
            from DecisionHandling.HLTSignatureConfig import muMenuSequence
            #step1
            mu11 = muMenuSequence(step="1",reconame="v1", hyponame="v1")
            mu12 = muMenuSequence(step="1",reconame="v2", hyponame="v2")
            #step2
            mu21 = muMenuSequence(step="2",reconame="v1", hyponame="v1")
            mu22 = muMenuSequence(step="2",reconame="v2", hyponame="v2")
            #step3
            mu31 = muMenuSequence(step="3",reconame="v1", hyponame="v1")
            mu32 = muMenuSequence(step="3",reconame="v2", hyponame="v2")
            #step4
            mu41 = muMenuSequence(step="4",reconame="v1", hyponame="v1")
           
           
        from DecisionHandling.HLTSignatureHypoTools import dimuDrComboHypoTool
                       
        # multiplicity here indicates the number of objects to be combined:
        # for the chain dictionary, get the sum of the multiplicity in the multiplicy array
        # in symmetric chains, multiplicity=2 but only one sequence is used
     
        CombChains =[
            # This is an example of a chain running in "serial"
            makeChain(name='HLT_TestChain6_muv1_TestChain10_ev1_L1EM7_MU8F',  L1Thresholds=["MU5VF","EM3"], ChainSteps=[
                makeChainStep("Step1_mu_em_serial", [mu11, emptySeq1], multiplicity=[1,1]),
                makeChainStep("Step2_mu_em_serial", [emptySeq2, el21], multiplicity=[1,1]),
                makeChainStep("Step3_mu_em_serial", multiplicity=[]),
                makeChainStep("Step4_mu_em_serial", [mu41, el41],  multiplicity=[1,1])] ),

            makeChain(name='HLT_TestChain6_muv2_TestChain8_ev2_L1EM7_MU8F', L1Thresholds=["MU5VF","EM3"], ChainSteps=[
                makeChainStep("Step1_mu2_em", [mu12, el11], multiplicity=[1,1]),
                makeChainStep("Step2_mu_em", [mu21, el21], multiplicity=[1,1])] ),

            makeChain(name='HLT_TestChain5_ev1_TestChain8_ev1_L12EM3',   L1Thresholds=["EM3","EM3"], ChainSteps=[ #norun
                makeChainStep("Step1_2emAs",   [el11, el11], multiplicity=[1,1]),
                makeChainStep("Step2_2emAs",   [el21, el21], multiplicity=[1,1]) ]),
                
            makeChain(name='HLT_TestChain5_ev1_TestChain8_ev1_2TestChain6_muv1_L12EM8VH_MU8F',   L1Thresholds=["EM8VH","EM8VH","MU8F"], ChainSteps=[
                makeChainStep("Step1_2em_2mu",   [el11,el11,mu11], multiplicity=[1,1,2]),
                makeChainStep("Step2_2em_2mu",   [el21,el21,mu21], multiplicity=[1,1,2]) ]),

            makeChain(name='HLT_2TestChain6_muv1_L12MU5VF',       L1Thresholds=["MU5VF"], ChainSteps=[
                makeChainStep("Step1_2mu",   [mu11], multiplicity=[2]),
                makeChainStep("Step2_2mu",   [mu21], multiplicity=[2]) ]),

            makeChain(name='HLT_3TestChain6_muv1_L12MU5VF',       L1Thresholds=["MU5VF"], ChainSteps=[
                makeChainStep("Step1_2mu",   [mu11], multiplicity=[3]),
                makeChainStep("Step2_2mu",   [mu21], multiplicity=[3]) ]),

            makeChain(name='HLT_TestChain6_muv1_TestChain10_muv1_L12MU5VF',       L1Thresholds=["MU5VF", "MU5VF"], ChainSteps=[
                makeChainStep("Step1_2muAs",   [mu11,mu11], multiplicity=[1,1]),
                makeChainStep("Step2_2muAs",   [mu21,mu21], multiplicity=[1,1]) ]),
                
            makeChain(name='HLT_2TestChain6_muEmpty1_L12MU5VF',   L1Thresholds=["MU5VF"], ChainSteps=[
                makeChainStep("Step1_2mu_empty",  multiplicity=[]),#[2]
                makeChainStep("Step2_2mu", [mu21], multiplicity=[2]) ]),

            makeChain(name='HLT_TestChain6_muv1_TestChain5_ev1dr_L12MU5VF',  L1Thresholds=["MU5VF","EM3"], ChainSteps=[
                makeChainStep("Step1_mu_em", [mu11, el11], multiplicity=[1,1], comboToolConfs=[dimuDrComboHypoTool]),
                makeChainStep("Step2_mu_em", [mu21, el21], multiplicity=[1,1], comboToolConfs=[dimuDrComboHypoTool])] ),
                                                                             
           
            makeChain(name='HLT_2TestChain4_muv1dr_L12MU5VF', L1Thresholds=["MU5VF"], ChainSteps=[
                makeChainStep("Step1_2mu",    [mu11], multiplicity=[2], comboToolConfs=[dimuDrComboHypoTool]),
                makeChainStep("Step2_2mu22",  [mu22], multiplicity=[2]) ] ),

            # FSNOSEED not implemented in emulation
            #  L1Thresholds=["MU5VF", "MU5VF"],
            makeChain(name='HLT_TestChain10_muEmpty1_TestChain6_muEmpty1_L12MU5VF', L1Thresholds=["MU5VF", "MU5VF"],  ChainSteps=[
                 makeChainStep("Step1_2muAs_empty", multiplicity=[]),
                 makeChainStep("Step2_2muAs",   [mu21, mu21], multiplicity=[1,1]) ])
            ]
            # Note: There used to be a test here of change of multiplicity signature, e.g. from multiplicity = [1]
            # in Step1 to multiplicity = [1,1] in Step2. This is no longer supported.

        HLTChains += CombChains



########################## L1 #################################################        
def emulateHLTSeeding(topSequence):
    log.info("emulateHLTSeeding")
    
    # modify hltSeeding already in the Tree
    hltSeeding=topSequence.HLTTop.HLTBeginSeq.HLTSeeding    
    
    ctpUnpacker = CTPUnpackingEmulationTool( ForceEnableAllChains=False , InputFilename="ctp.dat" )
    hltSeeding.ctpUnpacker = ctpUnpacker

    ## hack to solve the PS crash:
    from HLTSeeding.HLTSeedingConf import PrescalingEmulationTool
    psEmulation = PrescalingEmulationTool()
    hltSeeding.prescaler = psEmulation

    from HLTSeeding.HLTSeedingConfig import mapThresholdToL1RoICollection, mapThresholdToL1DecisionCollection
    emUnpacker = RoIsUnpackingEmulationTool("EMRoIsUnpackingTool", InputFilename="l1emroi.dat", OutputTrigRoIs=mapThresholdToL1RoICollection("EM"), Decisions=mapThresholdToL1DecisionCollection("EM"), ThresholdPrefix="EM" )
    muUnpacker = RoIsUnpackingEmulationTool("MURoIsUnpackingTool", InputFilename="l1muroi.dat",  OutputTrigRoIs=mapThresholdToL1RoICollection("MU"), Decisions=mapThresholdToL1DecisionCollection("MU"), ThresholdPrefix="MU" )

    hltSeeding.RoIBRoIUnpackers = [emUnpacker, muUnpacker]



