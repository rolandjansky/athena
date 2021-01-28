#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#




##########################################
# This is the menu:
###########################################
# note : L1 Seeds must be declared in CTP file: for example
# nightly/Athena/22.0.1/InstallArea/x86_64-slc6-gcc62-opt/XML/TriggerMenuXML/LVL1config_Physics_pp_v7.xml
##########################################

def generateChains():
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool
    from DecisionHandling.TestUtils import makeChain, makeChainStep
    
    testChains = []
    ##################################################################
    # egamma chains
    ##################################################################
    if opt.doEgammaSlice is True:
        from TriggerMenuMT.HLTMenuConfig.Egamma.ElectronDef import electronFastCaloCfg, fastElectronSequenceCfg, precisionCaloSequenceCfg
        fastCaloSeq = RecoFragmentsPool.retrieve( electronFastCaloCfg, None )
        electronSeq = RecoFragmentsPool.retrieve( fastElectronSequenceCfg, None )
        precisionCaloSeq = RecoFragmentsPool.retrieve( precisionCaloSequenceCfg, None )
        
        FastCaloStep = makeChainStep("ElectronFastCaloStep", [fastCaloSeq])
        FastElectronStep = makeChainStep("ElectronFastTrackStep", [electronSeq])
        PrecisionCaloStep = makeChainStep("ElectronPrecisionCaloStep", [precisionCaloSeq])
        
        electronChains  = [
            makeChain(name='HLT_e3_etcut1step_L1EM3',  L1Thresholds=["EM3"],  ChainSteps=[FastCaloStep]  ),
            makeChain(name='HLT_e3_etcut_L1EM3',       L1Thresholds=["EM3"],  ChainSteps=[FastCaloStep, FastElectronStep, PrecisionCaloStep]  ),
            makeChain(name='HLT_e5_etcut_L1EM3',       L1Thresholds=["EM3"],  ChainSteps=[FastCaloStep, FastElectronStep, PrecisionCaloStep]  ),
            makeChain(name='HLT_e7_etcut_L1EM3',       L1Thresholds=["EM3"],  ChainSteps=[FastCaloStep, FastElectronStep, PrecisionCaloStep]  )
            ]
        testChains += electronChains

        from TriggerMenuMT.HLTMenuConfig.Egamma.PhotonDef import fastPhotonCaloSequenceCfg, fastPhotonSequenceCfg, precisionPhotonCaloSequenceCfg
        fastCaloSeq = RecoFragmentsPool.retrieve( fastPhotonCaloSequenceCfg, None )
        fastPhotonSeq = RecoFragmentsPool.retrieve( fastPhotonSequenceCfg, None )
        precisionCaloPhotonSeq = RecoFragmentsPool.retrieve( precisionPhotonCaloSequenceCfg, None)
        
        FastCaloStep = makeChainStep("PhotonFastCaloStep", [fastCaloSeq])
        fastPhotonStep = makeChainStep("PhotonStep2", [fastPhotonSeq])
        precisionCaloPhotonStep = makeChainStep("precisionCaloPhotonStep", [precisionCaloPhotonSeq])
        
        photonChains = [
            makeChain(name='HLT_g5_etcut_L1EM3',    L1Thresholds=["EM3"],  ChainSteps=[ FastCaloStep,  fastPhotonStep, precisionCaloPhotonStep]  )
            ]
            
        testChains += photonChains
        
    ##################################################################
    # muon chains
    ##################################################################
    if opt.doMuonSlice is True:
        from TriggerMenuMT.HLTMenuConfig.Muon.MuonSequenceSetup import muFastSequence, muCombSequence, muEFSASequence, muEFCBSequence, muEFSAFSSequence, muEFCBFSSequence

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
        MuonChains += [ makeChain(name='HLT_mu6fast_L1MU6',     L1Thresholds=["MU6"], ChainSteps=[ step1mufast ])]
        MuonChains += [ makeChain(name='HLT_mu6Comb_L1MU6',     L1Thresholds=["MU6"], ChainSteps=[ step1mufast, step2muComb ])]
        MuonChains += [ makeChain(name='HLT_mu6_L1MU6',         L1Thresholds=["MU6"], ChainSteps=[ step1mufast, step2muComb, step3muEFSA, step4muEFCB ])]
        MuonChains += [ makeChain(name='HLT_mu6msonly_L1MU6',   L1Thresholds=["MU6"], ChainSteps=[ step1mufast, emptyStep,   step3muEFSA ])] # removed due to muEFSA isuue(?)

        # multi muon trigger
        # 2muons symmetric
        step1_2mufast_sym= makeChainStep("Step1_2muFast_sym", [ mufastS], multiplicity=[2])
        step2_2muComb_sym= makeChainStep("Step2_2muComb_sym", [ mucombS], multiplicity=[2])
    
        MuonChains += [ makeChain(name='HLT_2mu6Comb_L12MU6',  L1Thresholds=["MU6"], ChainSteps=[ step1_2mufast_sym, step2_2muComb_sym ])]

        # 2muons asymmetric (this will change): 2 sequences, 2 seeds
        step1_2mufast_asym= makeChainStep("Step1_2muFast_asym", [ mufastS, mufastS], multiplicity=[1,1])
        step2_2muComb_asym= makeChainStep("Step1_2muComb_asym", [ mucombS, mucombS], multiplicity=[1,1])
    
        MuonChains += [ makeChain(name='HLT_mu6_mu4_L12MU4',
                                L1Thresholds=["MU4", "MU6"],
                                ChainSteps=[ step1_2mufast_asym, step2_2muComb_asym ])]        
        
        
        #FS Muon trigger
        # Full scan MS tracking step
        stepFSmuEFSA=makeChainStep("Step_FSmuEFSA", [muEFSAFSSequence()])
        stepFSmuEFCB=makeChainStep("Step_FSmuEFCB", [muEFCBFSSequence()])
        MuonChains += [ makeChain(name='HLT_mu6noL1_L1MU6', L1Thresholds=["FSNOSEED"],  ChainSteps=[stepFSmuEFSA, stepFSmuEFCB])] 
        
        testChains += MuonChains
        
        
    ##################################################################
    # jet chains
    ##################################################################

    from TriggerMenuMT.HLTMenuConfig.Jet.JetRecoConfiguration import jetRecoDictFromString
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    def jetCaloHypoMenuSequenceFromString(jet_def_str):
        jetRecoDict = jetRecoDictFromString(jet_def_str)
        from TriggerMenuMT.HLTMenuConfig.Jet.JetMenuSequences import jetCaloHypoMenuSequence
        return jetCaloHypoMenuSequence(ConfigFlags, **jetRecoDict)

    def jetCaloPreselMenuSequenceFromString(jet_def_str):
        jetRecoDict = jetRecoDictFromString(jet_def_str)
        from TriggerMenuMT.HLTMenuConfig.Jet.JetMenuSequences import jetCaloPreselMenuSequence
        return jetCaloPreselMenuSequence(ConfigFlags, **jetRecoDict)

    def jetTrackingHypoMenuSequenceFromString(jet_def_str,clustersKey):
        jetRecoDict = jetRecoDictFromString(jet_def_str)
        from TriggerMenuMT.HLTMenuConfig.Jet.JetMenuSequences import jetTrackingHypoMenuSequence
        return jetTrackingHypoMenuSequence(ConfigFlags, clustersKey=clustersKey, **jetRecoDict)

    if opt.doJetSlice is True:

        # small-R jets
        jetSeq_a4_tc_em = jetCaloHypoMenuSequenceFromString("a4_tc_em_subjesIS")
        step_a4_tc_em = makeChainStep("Step_jet_a4_tc_em", [jetSeq_a4_tc_em])
        
        # large-R jets
        jetSeq_a10_tc_lcw_subjes = jetCaloHypoMenuSequenceFromString("a10_tc_lcw_subjes")
        step_a10_tc_lcw_subjes = makeChainStep("Step_jet_a10_subjes_tc_lcw", [jetSeq_a10_tc_lcw_subjes])
        
        jetSeq_a10r = jetCaloHypoMenuSequenceFromString("a10r_tc_em_subjesIS")
        step_a10r = makeChainStep("Step_jet_a10r", [jetSeq_a10r])

        jetSeq_a10t = jetCaloHypoMenuSequenceFromString("a10t_tc_lcw_jes")
        step_a10t = makeChainStep("Step_jet_a10t", [jetSeq_a10t])
        
        # Jet chains with tracking
        jetSeq_a4_tc_em_presel, emclusters = jetCaloPreselMenuSequenceFromString("a4_tc_em_subjesIS")
        step_a4_tc_em_presel = makeChainStep("Step_jet_a4_tc_em_presel", [jetSeq_a4_tc_em_presel])
        jetSeq_a4_pf_em_ftf = jetTrackingHypoMenuSequenceFromString("a4_tc_em_subresjesgscIS_ftf",emclusters)
        step_a4_pf_em_ftf = makeChainStep("Step_jet_a4_pf_em_ftf", [jetSeq_a4_pf_em_ftf])

        jetChains  = [
            makeChain(name='HLT_j45_L1J20',  L1Thresholds=["J20"], ChainSteps=[step_a4_tc_em]  ),
            makeChain(name='HLT_j85_L1J20',  L1Thresholds=["J20"], ChainSteps=[step_a4_tc_em]  ),
            makeChain(name='HLT_j420_L1J20', L1Thresholds=["J20"], ChainSteps=[step_a4_tc_em]  ),
            makeChain(name='HLT_j260_320eta490_L1J20',  L1Thresholds=["J20"], ChainSteps=[step_a4_tc_em]  ),
            makeChain(name='HLT_j0_vbenfSEP30etSEP34mass35SEP50fbet_L1J20',  L1Thresholds=["J20"], ChainSteps=[step_a4_tc_em]  ),
            makeChain(name='HLT_j460_a10_lcw_subjes_L1J20', L1Thresholds=["J20"], ChainSteps=[step_a10_tc_lcw_subjes]  ),
            makeChain(name='HLT_j460_a10r_L1J20', L1Thresholds=["J20"], ChainSteps=[step_a10r]  ),
            makeChain(name='HLT_j460_a10t_L1J20', L1Thresholds=["J20"], ChainSteps=[step_a10t]  ),
            makeChain(name='HLT_3j200_L1J20', L1Thresholds=["J20"], ChainSteps=[step_a4_tc_em]  ),
            makeChain(name='HLT_5j70_0eta240_L1J20', L1Thresholds=["J20"], ChainSteps=[step_a4_tc_em]  ), # 5j70_0eta240_L14J15 (J20 until multi-object L1 seeds supported)
            makeChain(name='HLT_j45_pf_subresjesgscIS_ftf_L1J20',  L1Thresholds=["J20"], ChainSteps=[step_a4_tc_em_presel,step_a4_pf_em_ftf]  ),
            ]

        testChains += jetChains


    ##################################################################
    # bjet chains
    ##################################################################
    if opt.doBjetSlice is True:
        from TriggerMenuMT.HLTMenuConfig.Bjet.BjetSequenceSetup import getBJetSequence

        jetSeq_a4_tc_em_presel, emclusters = jetCaloPreselMenuSequenceFromString("a4_tc_em_subjesIS")
        jetSeq_a4_tc_em_gsc_ftf = jetTrackingHypoMenuSequenceFromString("a4_tc_em_subjesgscIS_ftf",emclusters)
        
        step1 = makeChainStep("Step_jet_a4_tc_em_presel", [jetSeq_a4_tc_em_presel])
        step2 = makeChainStep("Step_jet_a4_tc_em_gsc_ftf", [jetSeq_a4_tc_em_gsc_ftf])
        step3 = makeChainStep("Step3_bjet", [getBJetSequence()])
        
        bjetChains  = [
            makeChain(name='HLT_j45_ftf_subjesgscIS_boffperf_split_L1J20' , L1Thresholds=["J20"], ChainSteps=[step1,step2,step3] ),
            makeChain(name='HLT_j45_ftf_subjesgscIS_bmv2c1070_split_L1J20', L1Thresholds=["J20"], ChainSteps=[step1,step2,step3] ),
            makeChain(name='HLT_j45_ftf_subjesgscIS_bmv2c1070_L1J20'      , L1Thresholds=["J20"], ChainSteps=[step1,step2,step3] )
            ]
        testChains += bjetChains
    
   
        ##################################################################
    # tau chains
    ##################################################################
    if opt.doTauSlice is True and False:  # not working at the moment
        from TriggerMenuMT.HLTMenuConfig.Tau.TauMenuSequences import getTauSequence

        step1=makeChainStep("Step1_tau", [getTauSequence('calo')])
        step1MVA=makeChainStep("Step1MVA_tau", [getTauSequence('calo_mva')])

        #This runs the tau-preselection(TP) step
        step2TP=makeChainStep("Step2TP_tau", [getTauSequence('track_core')])

        #This runs the EFTauMV hypo on top of fast tracks 
        step2PT=makeChainStep("Step2PT_tau", [getTauSequence('precision')])    
  
        tauChains  = [
            makeChain(name='HLT_tau0_perf_ptonly_L1TAU12',              L1Thresholds=["TAU12"], ChainSteps=[step1, step2] ),
            makeChain(name='HLT_tau25_medium1_tracktwo_L1TAU12IM',      L1Thresholds=["TAU12IM"],  ChainSteps=[step1, step2TP] ),
            makeChain(name='HLT_tau35_mediumRNN_tracktwoMVA_L1TAU12IM', L1Thresholds=["TAU20IM"], ChainSteps=[step1MVA, step2PT])
        ]
        testChains += tauChains


    ##################################################################
    # MET chains
    ##################################################################
    if opt.doMETSlice is True:
        from TriggerMenuMT.HLTMenuConfig.MET.METChainConfiguration import extractMETRecoDict
        from TriggerMenuMT.HLTMenuConfig.MET.ConfigHelpers import AlgConfig

        cellRecoDict = extractMETRecoDict({'EFrecoAlg': "cell"})
        metCellConf = AlgConfig.fromRecoDict(**cellRecoDict)
        metCellSeqs = metCellConf.menuSequences()

        pufitRecoDict = extractMETRecoDict({'EFrecoAlg': "tcpufit"})
        metClusterPufitConf = AlgConfig.fromRecoDict(**pufitRecoDict)
        metClusterPufitSeqs = metClusterPufitConf.menuSequences()
        
        metCellStep = makeChainStep("Step1_met_cell", metCellSeqs)
        metClusterPufitStep          = makeChainStep("Step1_met_clusterpufit", metClusterPufitSeqs)
        comboStep_cell_clusterpufit  = makeChainStep("Step1_combo_cell_clusterpufit", metCellSeqs + metClusterPufitSeqs, multiplicity=[1,1])

        metChains = [
            makeChain(name="HLT_xe65_L1XE50",         L1Thresholds=["XE50"], ChainSteps=[metCellStep]),
            makeChain(name="HLT_xe30_L1XE10",         L1Thresholds=["XE10"], ChainSteps=[metCellStep]),
            makeChain(name="HLT_xe30_tcpufit_L1XE10", L1Thresholds=["XE10"], ChainSteps=[metClusterPufitStep]),
            makeChain(name='HLT_xe30_cell_xe30_tcpufit_L1XE10',  L1Thresholds=["XE10","XE10"], ChainSteps=[comboStep_cell_clusterpufit ]) 
            ]

        testChains += metChains

    ##################################################################
    # B-physics and light states chains
    ##################################################################
    if opt.doBphysicsSlice is True:
        from TriggerMenuMT.HLTMenuConfig.Muon.MuonSequenceSetup import muFastSequence, muCombSequence, muEFSASequence, muEFCBSequence
        from TrigBphysHypo.TrigMultiTrkComboHypoConfig import DimuL2ComboHypoCfg, DimuEFComboHypoCfg
        
        step1_dimufast=makeChainStep("Step1_dimuFast", [muFastSequence()], multiplicity=[2])
        step2_dimuComb=makeChainStep("Step2_dimuComb", [muCombSequence()], multiplicity=[2], comboHypoCfg=DimuL2ComboHypoCfg)
        step3_dimuEFSA=makeChainStep("Step3_dimuEFSA", [muEFSASequence()], multiplicity=[2])
        step4_dimuEFCB=makeChainStep("Step4_dimuEFCB", [muEFCBSequence()], multiplicity=[2], comboHypoCfg=DimuEFComboHypoCfg)
        steps = [step1_dimufast, step2_dimuComb, step3_dimuEFSA, step4_dimuEFCB]
        
        BphysChains = [
            makeChain(name='HLT_2mu4_bBmumu_L12MU4', L1Thresholds=["MU4"], ChainSteps=steps),
            makeChain(name='HLT_2mu4_bDimu_L12MU4', L1Thresholds=["MU4"], ChainSteps=steps),
            makeChain(name='HLT_2mu4_bJpsimumu_L12MU4', L1Thresholds=["MU4"], ChainSteps=steps),
            makeChain(name='HLT_2mu6_bJpsimumu_L12MU6', L1Thresholds=["MU6"], ChainSteps=steps),
            makeChain(name='HLT_2mu4_bUpsimumu_L12MU4', L1Thresholds=["MU4"], ChainSteps=steps)
            ]

        testChains += BphysChains

    ##################################################################
    # combined chains
    ##################################################################
    if opt.doCombinedSlice is True:
        from TriggerMenuMT.HLTMenuConfig.Egamma.ElectronDef import electronFastCaloCfg
        fastCaloSeq = RecoFragmentsPool.retrieve( electronFastCaloCfg, None )
        
        from TriggerMenuMT.HLTMenuConfig.Muon.MuonSequenceSetup import muFastSequence
        
        comboStep_et_mufast           = makeChainStep("Step1_et_mufast", [fastCaloSeq, muFastSequence()], multiplicity=[1,1])
    #   comboStep_mufast_etcut1_step1 = makeChainStep("Step1_mufast_etcut1", [muFastSequence(), fastCaloSeq], multiplicity=[1,1])
        
        comboChains =  [ makeChain(name='HLT_e3_etcut_mu6_L1EM8I_MU10', L1Thresholds=["EM8I", "MU10"],  ChainSteps=[comboStep_et_mufast ])]
    #   comboChains += [Chain(name='HLT_mu8fast_e8_etcut1step',   ChainSteps=[ comboStep_mufast_etcut1_step1 ])]
        testChains += comboChains
        

### commands here:

from AthenaCommon.Logging import logging
__log = logging.getLogger('full_menu')

from TriggerJobOpts.TriggerFlags import TriggerFlags
createHLTMenuExternally=True # menu will be build up explicitly here 
doWriteRDOTrigger = False
doWriteBS = False
forceEnableAllChains=True
TriggerFlags.triggerMenuSetup = "LS2_v1"

include("TriggerJobOpts/runHLT_standalone.py")

# make menu manually here:
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import makeHLTTree
from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT

generateChains()
makeHLTTree( triggerConfigHLT=TriggerConfigHLT )


from TriggerMenuMT.HLTMenuConfig.Menu.HLTMenuJSON import generateJSON
generateJSON()

from TriggerMenuMT.HLTMenuConfig.Menu.HLTPrescaleJSON import generateJSON as generatePrescaleJSON
generatePrescaleJSON()
   
from TriggerMenuMT.HLTMenuConfig.Menu.HLTMonitoringJSON import generateDefaultMonitoringJSON
generateDefaultMonitoringJSON()

from AthenaCommon.AlgSequence import dumpSequence
dumpSequence(topSequence)
