#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#


##########################################
# generateCFChains generates some menu-like chains, outside the menu generation framework,  
# using the Control-flow framework alone
###########################################



def generateCFChains(opt):
    from TriggerMenuMT.HLT.Menu.MenuComponents import RecoFragmentsPool
    from TriggerMenuMT.HLT.Menu.SignatureDicts import ChainStore
    from TriggerMenuMT.HLT.Config.GenerateMenuMT import GenerateMenuMT
    from DecisionHandling.TestUtils import makeChain, makeChainStep
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.lock()
    menu = GenerateMenuMT()
    menu.chainsInMenu = ChainStore()
    ##################################################################
    # egamma chains
    ##################################################################
    if opt.doEgammaSlice is True:
        from TriggerMenuMT.HLT.Electron.ElectronChainConfiguration import electronFastCaloCfg, fastElectronSequenceCfg, precisionCaloSequenceCfg
        fastCaloSeq = RecoFragmentsPool.retrieve( electronFastCaloCfg, ConfigFlags )
        electronSeq = RecoFragmentsPool.retrieve( fastElectronSequenceCfg, ConfigFlags )
        precisionCaloSeq = RecoFragmentsPool.retrieve( precisionCaloSequenceCfg, ConfigFlags )
        
        FastCaloStep      = makeChainStep("ElectronFastCaloStep", [fastCaloSeq])
        FastElectronStep  = makeChainStep("ElectronFastTrackStep", [electronSeq])
        PrecisionCaloStep = makeChainStep("ElectronPrecisionCaloStep", [precisionCaloSeq])
        
        electronChains  = [
            makeChain(name='HLT_e3_etcut1step_L1EM3',  L1Thresholds=["EM3"],  ChainSteps=[FastCaloStep]  ),
            makeChain(name='HLT_e3_etcut_L1EM3',       L1Thresholds=["EM3"],  ChainSteps=[FastCaloStep, FastElectronStep, PrecisionCaloStep]  ),
            makeChain(name='HLT_e5_etcut_L1EM3',       L1Thresholds=["EM3"],  ChainSteps=[FastCaloStep, FastElectronStep, PrecisionCaloStep]  ),
            makeChain(name='HLT_e7_etcut_L1EM3',       L1Thresholds=["EM3"],  ChainSteps=[FastCaloStep, FastElectronStep, PrecisionCaloStep]  )
            ]
        menu.chainsInMenu['Egamma'] += electronChains

        from TriggerMenuMT.HLT.Photon.PhotonChainConfiguration import fastPhotonCaloSequenceCfg, fastPhotonSequenceCfg, precisionPhotonCaloSequenceCfg
        fastCaloSeq            = RecoFragmentsPool.retrieve( fastPhotonCaloSequenceCfg, ConfigFlags )
        fastPhotonSeq          = RecoFragmentsPool.retrieve( fastPhotonSequenceCfg, ConfigFlags )
        precisionCaloPhotonSeq = RecoFragmentsPool.retrieve( precisionPhotonCaloSequenceCfg, ConfigFlags)
        
        FastCaloStep            = makeChainStep("PhotonFastCaloStep", [fastCaloSeq])
        fastPhotonStep          = makeChainStep("PhotonStep2", [fastPhotonSeq])
        precisionCaloPhotonStep = makeChainStep("precisionCaloPhotonStep", [precisionCaloPhotonSeq])
        
        photonChains = [
            makeChain(name='HLT_g5_etcut_L1EM3',    L1Thresholds=["EM3"],  ChainSteps=[ FastCaloStep,  fastPhotonStep, precisionCaloPhotonStep]  )
            ]
        menu.chainsInMenu['Egamma'] += photonChains

    ##################################################################
    # muon chains
    ##################################################################
    if opt.doMuonSlice is True:
        from TriggerMenuMT.HLT.Muon.MuonMenuSequences import muFastSequence, muCombSequence, muEFSASequence, muEFCBSequence, muEFSAFSSequence, muEFCBFSSequence

        MuonChains  = []
        # step1
        mufastS= muFastSequence()
        step1mufast=makeChainStep("Step1_muFast", [ mufastS ])
        # step2
        mucombS = muCombSequence()
        step2muComb=makeChainStep("Step2_muComb", [ mucombS ])
        # step3
        muEFSAS = muEFSASequence()
        step3muEFSA=makeChainStep("Step3_muEFSA", [ muEFSAS ])
        #/step3muIso =makeChainStep("Step3_muIso",  [ muIsoSequence() ])
        # step4
        muEFCBS = muEFCBSequence()
        step4muEFCB=makeChainStep("Step4_muEFCB", [ muEFCBS ])

        emptyStep=makeChainStep("Step2_empty", multiplicity=[])

        ## single muon trigger  
        MuonChains += [ makeChain(name='HLT_mu6fast_L1MU5VF',     L1Thresholds=["MU5VF"], ChainSteps=[ step1mufast ])]
        MuonChains += [ makeChain(name='HLT_mu6Comb_L1MU5VF',     L1Thresholds=["MU5VF"], ChainSteps=[ step1mufast, step2muComb ])]
        MuonChains += [ makeChain(name='HLT_mu6_L1MU5VF',         L1Thresholds=["MU5VF"], ChainSteps=[ step1mufast, step2muComb, step3muEFSA, step4muEFCB ])]
        MuonChains += [ makeChain(name='HLT_mu6msonly_L1MU5VF',   L1Thresholds=["MU5VF"], ChainSteps=[ step1mufast, emptyStep,   step3muEFSA ])] # removed due to muEFSA isuue(?)

        # multi muon trigger
        # 2muons symmetric
        step1_2mufast_sym= makeChainStep("Step1_2muFast_sym", [ mufastS], multiplicity=[2])
        step2_2muComb_sym= makeChainStep("Step2_2muComb_sym", [ mucombS], multiplicity=[2])
    
        MuonChains += [ makeChain(name='HLT_2mu6Comb_L12MU5VF',  L1Thresholds=["MU5VF"], ChainSteps=[ step1_2mufast_sym, step2_2muComb_sym ])]

        # 2muons asymmetric (this will change): 2 sequences, 2 seeds
        step1_2mufast_asym= makeChainStep("Step1_2muFast_asym", [ mufastS, mufastS], multiplicity=[1,1])
        step2_2muComb_asym= makeChainStep("Step1_2muComb_asym", [ mucombS, mucombS], multiplicity=[1,1])
    
        MuonChains += [ makeChain(name='HLT_mu6_mu4_L12MU3V',
                                L1Thresholds=["MU3V", "MU3V"],
                                ChainSteps=[ step1_2mufast_asym, step2_2muComb_asym ])]        
        
        
        #FS Muon trigger
        # Full scan MS tracking step
        stepFSmuEFSA=makeChainStep("Step_FSmuEFSA", [muEFSAFSSequence()])
        stepFSmuEFCB=makeChainStep("Step_FSmuEFCB", [muEFCBFSSequence()])
        MuonChains += [ makeChain(name='HLT_mu6noL1_L1MU5VF', L1Thresholds=["FSNOSEED"],  ChainSteps=[stepFSmuEFSA, stepFSmuEFCB])] 

        menu.chainsInMenu['Muon'] += MuonChains

        
    ##################################################################
    # jet chains
    ##################################################################

    from TriggerMenuMT.HLT.Jet.JetRecoCommon import jetRecoDictFromString
    def jetCaloHypoMenuSequenceFromString(jet_def_str):
        jetRecoDict = jetRecoDictFromString(jet_def_str)
        from TriggerMenuMT.HLT.Jet.JetMenuSequences import jetCaloHypoMenuSequence
        return jetCaloHypoMenuSequence(ConfigFlags, isPerf=False, **jetRecoDict)

    def jetCaloPreselMenuSequenceFromString(jet_def_str):
        jetRecoDict = jetRecoDictFromString(jet_def_str)
        from TriggerMenuMT.HLT.Jet.JetMenuSequences import jetCaloPreselMenuSequence
        return jetCaloPreselMenuSequence(ConfigFlags, **jetRecoDict)

    def jetTrackingHypoMenuSequenceFromString(jet_def_str,clustersKey):
        jetRecoDict = jetRecoDictFromString(jet_def_str)
        from TriggerMenuMT.HLT.Jet.JetMenuSequences import jetFSTrackingHypoMenuSequence
        return jetFSTrackingHypoMenuSequence(ConfigFlags, clustersKey=clustersKey, isPerf=False, **jetRecoDict)

    if opt.doJetSlice is True:

        # small-R jets
        jetSeq_a4_tc_em, jetDef = jetCaloHypoMenuSequenceFromString("a4_tc_em_subjesIS")
        step_a4_tc_em = makeChainStep("Step_jet_a4_tc_em", [jetSeq_a4_tc_em])
        
        # large-R jets
        jetSeq_a10_tc_lcw_subjes, jetDef = jetCaloHypoMenuSequenceFromString("a10_tc_lcw_subjes")
        step_a10_tc_lcw_subjes = makeChainStep("Step_jet_a10_subjes_tc_lcw", [jetSeq_a10_tc_lcw_subjes])
        
        jetSeq_a10r, jetDef = jetCaloHypoMenuSequenceFromString("a10r_tc_em_subjesIS")
        step_a10r = makeChainStep("Step_jet_a10r", [jetSeq_a10r])

        jetSeq_a10t, jetDef = jetCaloHypoMenuSequenceFromString("a10t_tc_lcw_jes")
        step_a10t = makeChainStep("Step_jet_a10t", [jetSeq_a10t])
        
        # Jet chains with tracking
        jetSeq_a4_tc_em_presel, jetDef, emclusters = jetCaloPreselMenuSequenceFromString("a4_tc_em_subjesIS")
        step_a4_tc_em_presel = makeChainStep("Step_jet_a4_tc_em_presel", [jetSeq_a4_tc_em_presel])
        jetSeq_a4_pf_em_ftf, jetDef = jetTrackingHypoMenuSequenceFromString("a4_tc_em_subresjesgscIS_ftf",emclusters)
        step_a4_pf_em_ftf = makeChainStep("Step_jet_a4_pf_em_ftf", [jetSeq_a4_pf_em_ftf])

        menu.chainsInMenu['Jet'] = [
            makeChain(name='HLT_j45_L1J20',  L1Thresholds=["FSNOSEED"], ChainSteps=[step_a4_tc_em]  ),
            makeChain(name='HLT_j85_L1J20',  L1Thresholds=["FSNOSEED"], ChainSteps=[step_a4_tc_em]  ),
            makeChain(name='HLT_j420_L1J20', L1Thresholds=["FSNOSEED"], ChainSteps=[step_a4_tc_em]  ),
            makeChain(name='HLT_j260_320eta490_L1J20',  L1Thresholds=["FSNOSEED"], ChainSteps=[step_a4_tc_em]  ),
            makeChain(name='HLT_j460_a10_lcw_subjes_L1J20', L1Thresholds=["FSNOSEED"], ChainSteps=[step_a10_tc_lcw_subjes]  ),
            makeChain(name='HLT_j460_a10r_L1J20', L1Thresholds=["FSNOSEED"], ChainSteps=[step_a10r]  ),
            makeChain(name='HLT_j460_a10t_L1J20', L1Thresholds=["FSNOSEED"], ChainSteps=[step_a10t]  ),
            makeChain(name='HLT_3j200_L1J20', L1Thresholds=["FSNOSEED"], ChainSteps=[step_a4_tc_em]  ),
            makeChain(name='HLT_5j70_0eta240_L1J20', L1Thresholds=["FSNOSEED"], ChainSteps=[step_a4_tc_em]  ), # 5j70_0eta240_L14J15 (J20 until multi-object L1 seeds supported)
            makeChain(name='HLT_j45_pf_subresjesgscIS_ftf_preselj20_L1J20',  L1Thresholds=["FSNOSEED"], ChainSteps=[step_a4_tc_em_presel,step_a4_pf_em_ftf]  ),
            ]


    ##################################################################
    # bjet chains
    ##################################################################
    if opt.doBjetSlice is True:
        from TriggerMenuMT.HLT.Bjet.BjetMenuSequences import getBJetSequence

        jetSeq_a4_tc_em_presel, jetDef, emclusters = jetCaloPreselMenuSequenceFromString("a4_tc_em_subjesIS")
        jetSeq_a4_tc_em_gsc_ftf, jetDef = jetTrackingHypoMenuSequenceFromString("a4_tc_em_subjesgscIS_ftf",emclusters)
        jc_name = "HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf"
        
        step1 = makeChainStep("Step_jet_a4_tc_em_presel", [jetSeq_a4_tc_em_presel])
        step2 = makeChainStep("Step_jet_a4_tc_em_gsc_ftf", [jetSeq_a4_tc_em_gsc_ftf])
        step3 = makeChainStep("Step3_bjet", [getBJetSequence(ConfigFlags, jc_name)])
        
        menu.chainsInMenu['Bjet']  = [
            makeChain(name='HLT_j45_ftf_subjesgscIS_boffperf_preselj20_L1J20', L1Thresholds=["FSNOSEED"], ChainSteps=[step1,step2,step3] ),
            makeChain(name='HLT_j45_ftf_subjesgscIS_bdl1r70_preselj20_L1J20',  L1Thresholds=["FSNOSEED"], ChainSteps=[step1,step2,step3] ),
            ]

   
    ##################################################################
    # tau chains
    ##################################################################
    if opt.doTauSlice is True and False:  # not working at the moment
        from TriggerMenuMT.HLT.Tau.TauMenuSequences import getTauSequence

        step1=makeChainStep("Step1_tau", [getTauSequence('calo')])
        step1MVA=makeChainStep("Step1MVA_tau", [getTauSequence('calo_mva')])

        #This runs the tau-preselection(TP) step
        step2TP=makeChainStep("Step2TP_tau", [getTauSequence('track_core')])

        #This runs the EFTauMV hypo on top of fast tracks 
        step2PT=makeChainStep("Step2PT_tau", [getTauSequence('precision')])    
  
        menu.chainsInMenu['Tau'] = [
            makeChain(name='HLT_tau0_perf_ptonly_L1TAU8',              L1Thresholds=["TAU8"], ChainSteps=[step1, step2] ),
            makeChain(name='HLT_tau25_medium1_tracktwo_L1TAU12IM',      L1Thresholds=["TAU12IM"],  ChainSteps=[step1, step2TP] ),
            makeChain(name='HLT_tau35_mediumRNN_tracktwoMVA_L1TAU12IM', L1Thresholds=["TAU20IM"], ChainSteps=[step1MVA, step2PT])
        ]


    ##################################################################
    # MET chains
    ##################################################################
    if opt.doMETSlice is True:
        from TriggerMenuMT.HLT.MET.METChainConfiguration import extractMETRecoDict
        from TriggerMenuMT.HLT.MET.ConfigHelpers import AlgConfig

        cellRecoDict = extractMETRecoDict({'EFrecoAlg': "cell"})
        metCellConf = AlgConfig.fromRecoDict(**cellRecoDict)
        metCellSeqs = metCellConf.menuSequences()

        pufitRecoDict = extractMETRecoDict({'EFrecoAlg': "tcpufit"})
        metClusterPufitConf = AlgConfig.fromRecoDict(**pufitRecoDict)
        metClusterPufitSeqs = metClusterPufitConf.menuSequences()
        
        metCellStep = makeChainStep("Step1_met_cell", metCellSeqs)
        metClusterPufitStep          = makeChainStep("Step1_met_clusterpufit", metClusterPufitSeqs)
        comboStep_cell_clusterpufit  = makeChainStep("Step1_combo_cell_clusterpufit", metCellSeqs + metClusterPufitSeqs, multiplicity=[1,1])

        menu.chainsInMenu['MET'] = [
            makeChain(name="HLT_xe65_L1XE50",         L1Thresholds=["FSNOSEED"], ChainSteps=[metCellStep]),
            makeChain(name="HLT_xe30_L1XE30",         L1Thresholds=["FSNOSEED"], ChainSteps=[metCellStep]),
            makeChain(name="HLT_xe30_tcpufit_L1XE30", L1Thresholds=["FSNOSEED"], ChainSteps=[metClusterPufitStep]),
            makeChain(name='HLT_xe30_cell_xe30_tcpufit_L1XE30',  L1Thresholds=["FSNOSEED","FSNOSEED"], ChainSteps=[comboStep_cell_clusterpufit ]) 
            ]

    ##################################################################
    # B-physics and light states chains
    ##################################################################
    if opt.doBphysicsSlice is True:
        from TriggerMenuMT.HLT.Muon.MuonMenuSequences import muFastSequence, muCombSequence, muEFSASequence, muEFCBSequence
        from TrigBphysHypo.TrigMultiTrkComboHypoConfig import StreamerDimuL2ComboHypoCfg, DimuEFComboHypoCfg
        
        step1_dimufast=makeChainStep("Step1_dimuFast", [muFastSequence()], multiplicity=[2])
        step2_dimuComb=makeChainStep("Step2_dimuComb", [muCombSequence()], multiplicity=[2], comboHypoCfg=StreamerDimuL2ComboHypoCfg)
        step3_dimuEFSA=makeChainStep("Step3_dimuEFSA", [muEFSASequence()], multiplicity=[2])
        step4_dimuEFCB=makeChainStep("Step4_dimuEFCB", [muEFCBSequence()], multiplicity=[2], comboHypoCfg=DimuEFComboHypoCfg)
        steps = [step1_dimufast, step2_dimuComb, step3_dimuEFSA, step4_dimuEFCB]

        menu.chainsInMenu['Bphysics'] = [
            makeChain(name='HLT_2mu4_bBmumu_L12MU3V', L1Thresholds=["MU3V"], ChainSteps=steps),
            makeChain(name='HLT_2mu4_bDimu_L12MU3V', L1Thresholds=["MU3V"], ChainSteps=steps),
            makeChain(name='HLT_2mu4_bJpsimumu_L12MU3V', L1Thresholds=["MU3V"], ChainSteps=steps),
            makeChain(name='HLT_2mu6_bJpsimumu_L12MU5VF', L1Thresholds=["MU5VF"], ChainSteps=steps),
            makeChain(name='HLT_2mu4_bUpsimumu_L12MU3V', L1Thresholds=["MU3V"], ChainSteps=steps)
            ]

    ##################################################################
    # combined chains
    ##################################################################
    if opt.doCombinedSlice is True:
        from TriggerMenuMT.HLT.Electron.ElectronChainConfiguration import electronFastCaloCfg
        fastCaloSeq = RecoFragmentsPool.retrieve( electronFastCaloCfg, ConfigFlags )
        
        from TriggerMenuMT.HLT.Muon.MuonMenuSequences import muFastSequence
        
        comboStep_et_mufast           = makeChainStep("Step1_et_mufast", [fastCaloSeq, muFastSequence()], multiplicity=[1,1])

        menu.chainsInMenu['Combined'] = [
            makeChain(name='HLT_e3_etcut_mu6_L1EM7_MU8F', L1Thresholds=["EM7", "MU8F"],  ChainSteps=[comboStep_et_mufast ])]
