# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# Configure the scheduler
from AthenaCommon.AlgScheduler import AlgScheduler
from AthenaCommon.CFElements import parOR
from AthenaCommon.Logging import logging
from L1Decoder.L1DecoderConf import CTPUnpackingEmulationTool, RoIsUnpackingEmulationTool, L1Decoder

log = logging.getLogger('EmuStepProcessingConfig')


def generateL1DecoderAndChains():
    AlgScheduler.ShowControlFlow( True )
    AlgScheduler.ShowDataFlow( True )

    # 4 events

    data = {'noreco': [';', ';', ';',';']}  # in the lists there are the events

    data['emclusters'] = [ ';',
                        'eta:1,phi:1,et:180000; eta:1,phi:-1.2,et:35000;',
                        'eta:0.5,phi:0,et:120000; eta:1,phi:-1.2,et:65000;',
                        'eta:-0.6,phi:1.7,et:9000;']

    data['msmu']  = [';',
                     ';',
                     'eta:-1.2,phi:0.7,pt:6500,pt2:8500; eta:-1.1,phi:0.6,pt:8500,pt2:8500;',
                     'eta:-1.7,phi:-0.2,pt:29500,pt2:8500;']

    #data['ctp'] = [ 'HLT_e20 HLT_e5_e8 HLT_e5 HLT_e8 HLT_e5v22 HLT_g5',
    data['ctp'] = [ 'HLT_e20 HLT_e5_e8 HLT_e5 HLT_e8 HLT_g5',
                    'HLT_e20 HLT_e5_e8 HLT_e5 HLT_e8 HLT_g5 HLT_e5_v3',
                    'HLT_mu6 HLT_mu8 HLT_mu10 HLT_mu8_1step HLT_e20 HLT_e8 HLT_mu8_e8 HLT_e3_e5 HLT_2mu6 HLT_2mu6Comb HLT_2mu4_bDimu_L12MU4',
                    'HLT_mu20 HLT_mu10 HLT_mu8 HLT_mu8_1step HLT_2mu8 HLT_e8' ]


    data['l1emroi'] = [ ';',
                        '1,1,0,EM3,EM7,EM15,EM20,EM50,EM100,2EM3; 1,-1.2,0,EM3,EM7,2EM3',
                        '-0.6,0.2,0,EM3,EM7,EM15,EM20,EM50,EM100; 1,-1.1,0,EM3,EM7,EM15,EM20,EM50',
                        '-0.6,1.5,0,EM3,EM7,EM7']

    data['l1muroi'] = [';',
                       '0,0,0,MU0;',
                       '-1,0.5,0,MU6,MU8,2MU6; 1,0.5,0,MU6,MU8,MU10,2MU6',
                       '-1.5,-0.1,0,MU6,MU8,MU10']

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



    from TrigUpgradeTest.TestUtils import writeEmulationFiles
    writeEmulationFiles(data)

    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep


    doMuon=True
    doElectron=True
    doCombo=True

    HLTChains = []
    EnabledElChains = []
    EnabledMuChains = []
    EnabledMuComboChains = []
    EnabledElComboChains = []


    # muon chains
    if doMuon:
        from TrigUpgradeTest.HLTSignatureConfig import  muMenuSequence
        #step1
        mu11 = muMenuSequence(step="1",reconame="v1", hyponame="v1")
        #step2
        mu21 = muMenuSequence(step="2",reconame="v1", hyponame="v1")
        mu22 = muMenuSequence(step="2",reconame="v2", hyponame="v2")
        #step3
        mu31 = muMenuSequence(step="3",reconame="v1", hyponame="v1")
        mu32 = muMenuSequence(step="3",reconame="v2", hyponame="v2")
        #step4
        mu41 = muMenuSequence(step="4",reconame="v1", hyponame="v1")

        step_mu11  = ChainStep("Step_mu11", [mu11])
        step_mu21  = ChainStep("Step_mu21", [mu21] )
        step_mu22  = ChainStep("Step_mu22", [mu22] )
        step_mu31  = ChainStep("Step_mu31", [mu31] )
        step_mu32  = ChainStep("Step_mu32", [mu32] )
        step_mu41  = ChainStep("Step_mu41", [mu41] )
        
        step_empy= ChainStep("Step_empty")


        MuChains  = [
            Chain(name='HLT_mu8_1step', Seed="L1_MU6",  ChainSteps=[step_mu11 ]) ,
            Chain(name='HLT_mu20', Seed="L1_MU10",      ChainSteps=[step_mu11 , step_mu21 , step_mu31 ]) ,
            Chain(name='HLT_mu10', Seed="L1_MU10",      ChainSteps=[step_mu11 , step_mu22 , step_mu31 ]) ,
            Chain(name='HLT_mu8',  Seed="L1_MU10",      ChainSteps=[step_mu11 , step_mu21 , step_mu32, step_mu41]) ,
            Chain(name='HLT_mu6',  Seed="L1_MU6",       ChainSteps=[step_mu11 , step_empy , step_mu32, step_mu41]),
 #           Chain(name='HLT_mu6_1step',  Seed="L1_MU6",  ChainSteps=[step_mu11 , step_empy , step_mu31]),
#            Chain(name='HLT_2mu6_L1MU6',  Seed="L1_2MU6",      ChainSteps=[ChainStep("Step_2muSA", [muSA,muSA])  ] )
            ]
            

        HLTChains += MuChains
        EnabledMuChains= [c.seed.strip().split("_")[1] +" : "+ c.name for c in MuChains]




    ## #electron chains
    if doElectron:
        from TrigUpgradeTest.HLTSignatureConfig import  elMenuSequence, gamMenuSequence
        el11 = elMenuSequence(step="1",reconame="v1", hyponame="v1")
        el21 = elMenuSequence(step="2",reconame="v1", hyponame="v1")
        el22 = elMenuSequence(step="2",reconame="v2", hyponame="v2")
        el23 = elMenuSequence(step="2",reconame="v2", hyponame="v3")
        el31 = elMenuSequence(step="3",reconame="v1", hyponame="v1")

        # gamma
        gamm11 = gamMenuSequence("1", reconame="v1", hyponame="v1")

    
        ElChains  = [
            Chain(name='HLT_e5'   , Seed="L1_EM7", ChainSteps=[ ChainStep("Step_em11", [el11]), ChainStep("Step_em21",  [el21]) ] ),
            Chain(name='HLT_e5_v2', Seed="L1_EM7", ChainSteps=[ ChainStep("Step_em11", [el11]), ChainStep("Step_em22",  [el22]) ] ),
            Chain(name='HLT_e5_v3', Seed="L1_EM7", ChainSteps=[ ChainStep("Step_em11", [el11]), ChainStep("Step_em23",  [el23]) ] ),
            Chain(name='HLT_e8'   , Seed="L1_EM7", ChainSteps=[ ChainStep("Step_em11", [el11]), ChainStep("Step_em21",  [el21]), ChainStep("Step_em31",  [el31]) ] ),
            Chain(name='HLT_g5'   , Seed="L1_EM7", ChainSteps=[ ChainStep("Step_gam11", [gamm11]) ] )
        ]

        HLTChains += ElChains
        EnabledElChains= [c.seed.strip().split("_")[1] +" : "+ c.name for c in ElChains]


    # combined chain
    if doCombo:
        if not doElectron:
            from TrigUpgradeTest.HLTSignatureConfig import elMenuSequence        
            el11 = elMenuSequence(step="1",reconame="v1", hyponame="v1")    
            el21 = elMenuSequence(step="2",reconame="v1", hyponame="v1")
            
        if not doMuon:
            from TrigUpgradeTest.HLTSignatureConfig import muMenuSequence
            #step1
            mu11 = muMenuSequence(step="1",reconame="v1", hyponame="v1")
            #step2
            mu21 = muMenuSequence(step="2",reconame="v1", hyponame="v1")
            mu22 = muMenuSequence(step="2",reconame="v2", hyponame="v2")
            #step3
            mu31 = muMenuSequence(step="3",reconame="v1", hyponame="v1")
            mu32 = muMenuSequence(step="3",reconame="v2", hyponame="v2")
            #step4
            mu41 = muMenuSequence(step="4",reconame="v1", hyponame="v1")

           
            step_mu22  = ChainStep("Step_mu22", [mu22] )
           

            
           
        # multiplicity here indicates the number of objects to be combined:
        # for the chain dictionary, get the sum of the multiplicity in the multiplicy array
        # in symmetric chains, multiplicity=2 but only one sequence is used
        
        CombChains =[
            Chain(name='HLT_mu8_e8',  Seed="L1_MU6_EM7", ChainSteps=[ ChainStep("Step1_mu_em",  [mu11, el11], multiplicity=2),
                                                                      ChainStep("Step2_mu_em",  [mu21, el21], multiplicity=2)] ),
            Chain(name='HLT_e5_e8',   Seed="L1_2EM3",    ChainSteps=[ ChainStep("Step1_2em",    [el11, el11], multiplicity=2),
                                                                      ChainStep("Step2_2em",    [el21, el21], multiplicity=2) ]),
            Chain(name='HLT_2mu6',    Seed="L1_MU6",     ChainSteps=[ ChainStep("Step1_2mu",    [mu11], multiplicity=2),
                                                                      ChainStep("Step2_2mu",    [mu21], multiplicity=2) ]), ## L1 seed to be set correctly
            Chain(name='HLT_2mu6Comb',Seed="L1_MU6",     ChainSteps=[  ChainStep("Step1_2mu_empty", multiplicity=2),
                                                                       ChainStep("Step2_2mu",  [mu21], multiplicity=2) ]), ## L1 seed to be set correctly
            Chain(name='HLT_2mu4_bDimu_L12MU4', Seed="L1_MU4", ChainSteps=[ ChainStep("Step1_2mu",         [mu11], multiplicity=2),\
                                                                            step_mu22,\
                                                                            ChainStep("Step3_2mu",         [mu31], multiplicity=2)] ) ## L1 seed to be set correctly
            ]


        HLTChains += CombChains
        for c in CombChains:
            seeds=c.seed.split("_")
            seeds.pop(0) #remove first L1 string
            for s in seeds:
                if "MU" in s: EnabledMuComboChains.append(s +" : "+ c.name)
                if "EM" in s: EnabledElComboChains.append(s +" : "+ c.name)

        log.debug("enabled Combo chains: %s, %s", EnabledMuComboChains, EnabledElComboChains)


    # this is a temporary hack to include new test chains
    EnabledChainNamesToCTP = dict([ (c.name, c.seed)  for c in HLTChains])

    ########################## L1 #################################################

    L1UnpackingSeq = parOR("L1UnpackingSeq")

    l1Decoder = L1Decoder( RoIBResult="" )
    l1Decoder.prescaler.EventInfo=""
    l1Decoder.ChainToCTPMapping = EnabledChainNamesToCTP
    l1Decoder.L1DecoderSummaryKey = "L1DecoderSummary"

    ctpUnpacker = CTPUnpackingEmulationTool( ForceEnableAllChains=False , InputFilename="ctp.dat" )
    l1Decoder.ctpUnpacker = ctpUnpacker

    emUnpacker = RoIsUnpackingEmulationTool("EMRoIsUnpackingTool", InputFilename="l1emroi.dat", OutputTrigRoIs="L1EMRoIs", Decisions="L1EM" )
    emUnpacker.ThresholdToChainMapping = EnabledElChains + EnabledElComboChains
    emUnpacker.Decisions="L1EM"
    log.debug("EMRoIsUnpackingTool enables chians:")
    log.debug(emUnpacker.ThresholdToChainMapping)

    muUnpacker = RoIsUnpackingEmulationTool("MURoIsUnpackingTool", InputFilename="l1muroi.dat",  OutputTrigRoIs="L1MURoIs", Decisions="L1MU" )
    muUnpacker.ThresholdToChainMapping = EnabledMuChains + EnabledMuComboChains
    muUnpacker.Decisions="L1MU"
    log.debug("MURoIsUnpackingTool enables chians:")
    log.debug(muUnpacker.ThresholdToChainMapping)

    l1Decoder.roiUnpackers = [emUnpacker, muUnpacker]

    #print l1Decoder
    L1UnpackingSeq += l1Decoder
    log.debug(L1UnpackingSeq)

    ########################## L1 #################################################

    return l1Decoder, HLTChains
