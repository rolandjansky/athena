#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
__log = logging.getLogger('full_menu')

createHLTMenuExternally=True # menu will be build up explicitly here 
include("TrigUpgradeTest/testHLT_MT.py")

##########################################
# menu
###########################################
# note : L1 Seeds must be declared in CTP file: for example
# nightly/Athena/22.0.1/InstallArea/x86_64-slc6-gcc62-opt/XML/TriggerMenuXML/LVL1config_Physics_pp_v7.xml
##########################################


from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep, RecoFragmentsPool

testChains = []
from TrigUpgradeTest.TestUtils import makeChain

##################################################################
# egamma chains
##################################################################
if opt.doEgammaSlice == True:
    from TriggerMenuMT.HLTMenuConfig.Egamma.ElectronDef import electronFastCaloCfg, fastElectronSequenceCfg, precisionCaloSequenceCfg
    fastCaloSeq = RecoFragmentsPool.retrieve( electronFastCaloCfg, None )
    electronSeq = RecoFragmentsPool.retrieve( fastElectronSequenceCfg, None )
    precisionCaloSeq = RecoFragmentsPool.retrieve( precisionCaloSequenceCfg, None )

    FastCaloStep = ChainStep("ElectronFastCaloStep", [fastCaloSeq])
    FastElectronStep = ChainStep("ElectronFastTrackStep", [electronSeq])
    PrecisionCaloStep = ChainStep("ElectronPrecisionCaloStep", [precisionCaloSeq])

    electronChains  = [
        makeChain(name='HLT_e3_etcut1step_L1EM3',  L1Thresholds=["EM3"],  ChainSteps=[FastCaloStep]  ),
        makeChain(name='HLT_e3_etcut_L1EM3',       L1Thresholds=["EM3"],  ChainSteps=[FastCaloStep, FastElectronStep, PrecisionCaloStep]  ),
        makeChain(name='HLT_e5_etcut_L1EM3',       L1Thresholds=["EM3"],  ChainSteps=[FastCaloStep, FastElectronStep, PrecisionCaloStep]  ),
        makeChain(name='HLT_e7_etcut_L1EM3',       L1Thresholds=["EM3"],  ChainSteps=[FastCaloStep, FastElectronStep, PrecisionCaloStep]  )
        ]

#    DiEleStep1=ChainStep("DiEleStep1",[fastCaloSeq, fastCaloSeq], multiplicity=2) #same step
#    DiEleStep2=ChainStep("DiEleStep2",[electronSeq, electronSeq], multiplicity=2) #need to be: one leg with only one step, one with 3 steps!
    
#    electronChains += [Chain(name='HLT_e5_etcut1step_e8_etcut',  ChainSteps=[DiEleStep1, DiEleStep2 ]  )]
    testChains += electronChains

    from TriggerMenuMT.HLTMenuConfig.Egamma.PhotonDef import fastPhotonCaloSequenceCfg, fastPhotonSequenceCfg, precisionPhotonCaloSequenceCfg

    fastCaloSeq = RecoFragmentsPool.retrieve( fastPhotonCaloSequenceCfg, None )
    fastPhotonSeq = RecoFragmentsPool.retrieve( fastPhotonSequenceCfg, None )
    precisionCaloPhotonSeq = RecoFragmentsPool.retrieve( precisionPhotonCaloSequenceCfg, None)

    FastCaloStep = ChainStep("PhotonFastCaloStep", [fastCaloSeq])
    fastPhotonStep = ChainStep("PhotonStep2", [fastPhotonSeq])
    precisionCaloPhotonStep = ChainStep("precisionCaloPhotonStep", [precisionCaloPhotonSeq])

    photonChains = [
        makeChain(name='HLT_g5_etcut_L1EM3',    L1Thresholds=["EM3"],  ChainSteps=[ FastCaloStep,  fastPhotonStep, precisionCaloPhotonStep]  )
        ]

    testChains += photonChains

##################################################################
# muon chains
##################################################################
if opt.doMuonSlice == True:
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSequenceSetup import muFastSequence, muCombSequence, muEFSASequence, muIsoSequence, muEFCBSequence, muEFSAFSSequence, muEFCBFSSequence

    MuonChains  = []

    # step1
    mufastS= muFastSequence()
    step1mufast=ChainStep("Step1_muFast", [ mufastS ])
    # step2
    mucombS = muCombSequence()
    step2muComb=ChainStep("Step2_muComb", [ mucombS ])

    # step3
    muEFSAS = muEFSASequence()
    step3muEFSA=ChainStep("Step3_muEFSA", [ muEFSAS ])
    step3muIso =ChainStep("Step3_muIso",  [ muIsoSequence() ])
    # step4
    muEFCBS = muEFCBSequence()
    step4muEFCB=ChainStep("Step4_muEFCB", [ muEFCBS ])

    emptyStep=ChainStep("Step2_empty")

    ## single muon trigger  
    MuonChains += [ makeChain(name='HLT_mu6fast_L1MU6',     L1Thresholds=["MU6"], ChainSteps=[ step1mufast ])]
    MuonChains += [ makeChain(name='HLT_mu6Comb_L1MU6',     L1Thresholds=["MU6"], ChainSteps=[ step1mufast, step2muComb ])]
    MuonChains += [ makeChain(name='HLT_mu6_L1MU6',         L1Thresholds=["MU6"], ChainSteps=[ step1mufast, step2muComb, step3muEFSA, step4muEFCB ])]
    MuonChains += [ makeChain(name='HLT_mu6msonly_L1MU6',   L1Thresholds=["MU6"], ChainSteps=[ step1mufast, emptyStep,   step3muEFSA ])] # removed due to muEFSA isuue(?)
    MuonChains += [ makeChain(name='HLT_mu20_ivar_L1MU6',   L1Thresholds=["MU6"], ChainSteps=[ step1mufast, step2muComb, step3muIso ])]

    # multi muon trigger
    # 2muons symmetric
    step1_2mufast_sym= ChainStep("Step1_2muFast_sym", [ mufastS], multiplicity=[2])
    step2_2muComb_sym= ChainStep("Step2_2muComb_sym", [ mucombS], multiplicity=[2])
    
    step3_2muEFSA_sym= ChainStep("Step3_2muEFSA_sym", [ muEFSAS], multiplicity=[2])
    step4_2muEFCB_sym= ChainStep("Step4_2muEFCB_sym", [ muEFCBS], multiplicity=[2])
 
    MuonChains += [ makeChain(name='HLT_2mu6Comb_L12MU6',  L1Thresholds=["MU6"], ChainSteps=[ step1_2mufast_sym, step2_2muComb_sym ])]

    # 2muons asymmetric (this will change): 2 sequences, 2 seeds
    step1_2mufast_asym= ChainStep("Step1_2muFast_asym", [ mufastS, mufastS], multiplicity=[1,1])
    step2_2muComb_asym= ChainStep("Step1_2muComb_asym", [ mucombS, mucombS], multiplicity=[1,1])
    
    step3_2muEFSA_asym= ChainStep("Step3_2muEFSA_asym", [ muEFSAS, muEFSAS], multiplicity=[1,1])
    step4_2muEFCB_asym= ChainStep("Step4_2muEFCB_asym", [ muEFCBS, muEFCBS], multiplicity=[1,1])
    
    
    MuonChains += [ makeChain(name='HLT_mu6_mu4_L12MU4',
                              L1Thresholds=["MU4", "MU6"],
                              ChainSteps=[ step1_2mufast_asym, step2_2muComb_asym ])]        
    
    
    #FS Muon trigger
    # Full scan MS tracking step
    stepFSmuEFSA=ChainStep("Step_FSmuEFSA", [muEFSAFSSequence()])
    stepFSmuEFCB=ChainStep("Step_FSmuEFCB", [muEFCBFSSequence()])
    MuonChains += [ makeChain(name='HLT_mu6noL1_L1MU6', L1Thresholds=[""],  ChainSteps=[stepFSmuEFSA, stepFSmuEFCB])] 

    testChains += MuonChains


##################################################################
# jet chains
##################################################################
if opt.doJetSlice == True:
    from TrigUpgradeTest.jetMenuHelper import jetMenuSequenceFromString

    # small-R jets, different calibrations HLT_AntiKt4EMTopoJets_subjesIS
    jetSeq_a4_tc_em = jetMenuSequenceFromString("a4_tc_em_subjesIS")
    step_a4_tc_em =ChainStep("Step_jet_a4_tc_em", [jetSeq_a4_tc_em])

    jetSeq_a4_tc_em_subjes = jetMenuSequenceFromString("a4_tc_em_subjes")
    step_a4_tc_em_subjes = ChainStep("Step_jet_a4_subjes_tc_em", [jetSeq_a4_tc_em_subjes])

    jetSeq_a4_tc_em_nocalib = jetMenuSequenceFromString("a4_tc_em_nojcalib")
    step_a4_tc_em_nocalib=ChainStep("Step_jet_a4_nojcalib_tc_em", [jetSeq_a4_tc_em_nocalib])

#    jetSeq_a4_tc_lcw = jetMenuSequenceFromString("a10_tc_lcw_subjesIS")
#    step_a4_tc_lcw=ChainStep("Step_jet_a10_tc_lcw", [jetSeq_a4_tc_lcw])

    # large-R jets
    jetSeq_a10_tc_lcw_subjes = jetMenuSequenceFromString("a10_tc_lcw_subjes")
    step_a10_tc_lcw_subjes=ChainStep("Step_jet_a10_subjes_tc_lcw", [jetSeq_a10_tc_lcw_subjes])

    jetSeq_a10r = jetMenuSequenceFromString("a10r_tc_em_subjesIS")
    step_a10r=ChainStep("Step_jet_a10r", [jetSeq_a10r])
    
    jetChains  = [
        makeChain(name='HLT_j45_L1J20',  L1Thresholds=["J20"], ChainSteps=[step_a4_tc_em]  ),
        makeChain(name='HLT_j85_L1J20',  L1Thresholds=["J20"], ChainSteps=[step_a4_tc_em]  ),
        makeChain(name='HLT_j420_L1J20', L1Thresholds=["J20"], ChainSteps=[step_a4_tc_em]  ),
        makeChain(name='HLT_j260_320eta490_L1J20',  L1Thresholds=["J20"], ChainSteps=[step_a4_tc_em]  ),
      # makeChain(name='HLT_j225_gsc420_boffperf_split',   ChainSteps=[step_a4_tc_em]  ),
        makeChain(name='HLT_j0_vbenfSEP30etSEP34mass35SEP50fbet_L1J20',  L1Thresholds=["J20"], ChainSteps=[step_a4_tc_em]  ),
        makeChain(name='HLT_j460_a10_lcw_subjes_L1J20', L1Thresholds=["J20"], ChainSteps=[step_a10_tc_lcw_subjes]  ),
        makeChain(name='HLT_j460_a10r_L1J20', L1Thresholds=["J20"], ChainSteps=[step_a10r]  ),
        makeChain(name='HLT_3j200_L1J20', L1Thresholds=["J20"], ChainSteps=[step_a4_tc_em]  ),
        makeChain(name='HLT_5j70_0eta240_L1J20', L1Thresholds=["J20"], ChainSteps=[step_a4_tc_em]  ), # 5j70_0eta240_L14J15 (J20 until multi-object L1 seeds supported)
        ]

    testChains += jetChains


##################################################################
# bjet chains
##################################################################
if opt.doBjetSlice == True:
    from TrigUpgradeTest.jetMenuHelper import jetMenuSequenceFromString
    from TriggerMenuMT.HLTMenuConfig.Bjet.BjetSequenceSetup import getBJetSequence

    jetSequence = jetMenuSequenceFromString("a4_tc_em_subjesgscIS_ftf")

    step1 = ChainStep("Step1_bjet", [jetSequence] )
    step2 = ChainStep("Step2_bjet", [getBJetSequence('j')])
    step3 = ChainStep("Step3_bjet", [getBJetSequence('btag')])

    bjetChains  = [
          makeChain(name='HLT_j45_ftf_subjesgscIS_boffperf_split_L1J20' , L1Thresholds=["J20"], ChainSteps=[step1,step2,step3] ),
          makeChain(name='HLT_j45_ftf_subjesgscIS_bmv2c1070_split_L1J20', L1Thresholds=["J20"], ChainSteps=[step1,step2,step3] ),
          makeChain(name='HLT_j45_ftf_subjesgscIS_bmv2c1070_L1J20'      , L1Thresholds=["J20"], ChainSteps=[step1,step2,step3] )
        ]
    testChains += bjetChains

"""
Commenting out tau chains for now    
##################################################################
# tau chains
##################################################################
if opt.doTauSlice == True:
    from TriggerMenuMT.HLTMenuConfig.Tau.TauMenuSequences import getTauSequence

    step1=ChainStep("Step1_tau", [getTauSequence('calo')])
    step1MVA=ChainStep("Step1MVA_tau", [getTauSequence('calo_mva')])

    #This runs the tau-preselection(TP) step
    step2TP=ChainStep("Step2TP_tau", [getTauSequence('track_core')])

    #This runs the EFTauMV hypo on top of fast tracks 
    step2PT=ChainStep("Step2PT_tau", [getTauSequence('precision')])    
  
    tauChains  = [
          makeChain(name='HLT_tau0_perf_ptonly_L1TAU12',              L1Thresholds=["TAU12"], ChainSteps=[step1, step2] ),
          makeChain(name='HLT_tau25_medium1_tracktwo_L1TAU12IM',      L1Thresholds=["TAU12IM"],  ChainSteps=[step1, step2TP] ),
          makeChain(name='HLT_tau35_mediumRNN_tracktwoMVA_L1TAU12IM', L1Thresholds=["TAU20IM"], ChainSteps=[step1MVA, step2PT])
      ]
    testChains += tauChains
"""

##################################################################
# MET chains
##################################################################
if opt.doMETSlice == True:
    from TriggerMenuMT.HLTMenuConfig.MET.METMenuSequences import metMenuSequence
    from TriggerMenuMT.HLTMenuConfig.MET.ConfigHelpers import extractMETRecoDict

    cellRecoDict = extractMETRecoDict({'EFrecoAlg': "cell"})
    metCellSeq = metMenuSequence(None, **cellRecoDict)

    pufitRecoDict = extractMETRecoDict({'EFrecoAlg': "tcpufit"})
    metClusterPufitSeq = metMenuSequence(None, **pufitRecoDict)

    metCellStep = ChainStep("Step1_met_cell", [metCellSeq])
    metClusterPufitStep          = ChainStep("Step1_met_clusterpufit", [metClusterPufitSeq])
    comboStep_cell_clusterpufit  = ChainStep("Step1_combo_cell_clusterpufit", [metCellSeq, metClusterPufitSeq], multiplicity=[2])

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
if opt.doBphysicsSlice == True:
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSequenceSetup import muFastSequence, muEFSASequence, muEFCBSequence
    from TriggerMenuMT.HLTMenuConfig.Bphysics.BphysicsSequenceSetup import dimuL2Sequence, dimuEFSequence
	
    BphysChains  = []

    step1mufast=ChainStep("Step1_muFast",  [muFastSequence()])
    step2L2Dimu=ChainStep("Step2_L2Dimu",  [dimuL2Sequence()])
    
    #still to come
    step3muEFSA=ChainStep("Step3_muEFSA",   [ muEFSASequence() ])
    step4muEFCB=ChainStep("Step4_muEFCB",   [ muEFCBSequence() ])
    step5EFDimu=ChainStep("Step5_EFDimu",   [ dimuEFSequence() ])

    BphysChains += [ makeChain(name='HLT_2mu4_bDimu_L12MU4',     L1Thresholds=["MU4"], ChainSteps=[ step1mufast, step2L2Dimu, step3muEFSA, step4muEFCB, step5EFDimu])]
    BphysChains += [ makeChain(name='HLT_2mu6_bJpsimumu_L12MU6', L1Thresholds=["MU6"], ChainSteps=[ step1mufast, step2L2Dimu])]
    BphysChains += [ makeChain(name='HLT_2mu4_bBmumu_L12MU4',    L1Thresholds=["MU4"], ChainSteps=[ step1mufast, step2L2Dimu])]
    BphysChains += [ makeChain(name='HLT_2mu4_bUpsimumu_L12MU4', L1Thresholds=["MU4"], ChainSteps=[ step1mufast, step2L2Dimu])]
    BphysChains += [ makeChain(name='HLT_2mu4_bJpsimumu_L12MU4', L1Thresholds=["MU4"], ChainSteps=[ step1mufast, step2L2Dimu])]

    #BphysChains += [Chain(name='HLT_mu6_mu4_bJpsimumu_L1MU6_2MU4',  ChainSteps=[ step1mufast, step2L2Dimu])]
                                        #to come: step3muEFSA, step4muEFCB, step5EFJpsi])]
    testChains += BphysChains  




##################################################################
# combined chains
##################################################################
if opt.doCombinedSlice == True:
    # combo chains

    from TriggerMenuMT.HLTMenuConfig.Egamma.ElectronDef import electronFastCaloCfg
    fastCaloSeq = RecoFragmentsPool.retrieve( electronFastCaloCfg, None )

    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSequenceSetup import muFastSequence

    comboStep_et_mufast           = ChainStep("Step1_et_mufast", [fastCaloSeq, muFastSequence()], multiplicity=[2])
    comboStep_mufast_etcut1_step1 = ChainStep("Step1_mufast_etcut1", [muFastSequence(), fastCaloSeq], multiplicity=[2])


    comboChains =  [ makeChain(name='HLT_e3_etcut_mu6_L1EM8I_MU10', L1Thresholds=["EM8I", "MU10"],  ChainSteps=[comboStep_et_mufast ])]
 #   comboChains += [Chain(name='HLT_mu8fast_e8_etcut1step',   ChainSteps=[ comboStep_mufast_etcut1_step1 ])]
    testChains += comboChains



##########################################
# CF construction
##########################################

##### Make all HLT #######
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import makeHLTTree
from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT
makeHLTTree( triggerConfigHLT=TriggerConfigHLT )

##########################################
# Configure trigger output using parts of the NewJO configuration
# in a somewhat hacky way - copy-pasted from full_menu.py
##########################################
from TriggerJobOpts.TriggerConfig import collectHypos, collectFilters, collectDecisionObjects, collectHypoDecisionObjects, triggerOutputCfg
from AthenaCommon.CFElements import findAlgorithm,findSubSequence
hypos = collectHypos(findSubSequence(topSequence, "HLTAllSteps"))
filters = collectFilters(findSubSequence(topSequence, "HLTAllSteps"))

# find DecisionSummaryMakerAlg
summaryMakerAlg = findAlgorithm(topSequence, "DecisionSummaryMakerAlg")
if not summaryMakerAlg:
    __log.warning("Failed to find DecisionSummaryMakerAlg")

# try to find L1Decoder
l1decoder = findAlgorithm(topSequence,'L1Decoder')
if not l1decoder:
    l1decoder = findAlgorithm(topSequence,'L1EmulationTest')
if l1decoder and summaryMakerAlg:
    decObj = collectDecisionObjects( hypos, filters, l1decoder, summaryMakerAlg )
    decObjHypoOut = collectHypoDecisionObjects(hypos, inputs=False, outputs=True)
    __log.debug("Decision Objects to write to output [hack method - should be replaced with triggerRunCfg()]")
    __log.debug(decObj)
else:
    __log.warning("Failed to find L1Decoder or summaryMakerAlg, cannot determine Decision names for output configuration")
    decObj = []
    decObjHypoOut = []


from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior+=1
acc, edmSet = triggerOutputCfg(ConfigFlags, decObj, decObjHypoOut, summaryMakerAlg)
Configurable.configurableRun3Behavior-=1
acc.appendToGlobals()

##########################################
# Print top sequence for debugging
##########################################
from AthenaCommon.AlgSequence import dumpSequence, AthSequencer
dumpSequence(topSequence)

##########################################
# Write menu JSON
##########################################
HLTTop = findSubSequence(topSequence, "HLTTop")
from TriggerMenuMT.HLTMenuConfig.Menu.HLTMenuJSON import generateJSON
generateJSON()

# the generation of the prescale set file from the menu is temporary
from TrigConfigSvc.TrigConfigSvcCfg import createHLTPrescalesFileFromMenu
createHLTPrescalesFileFromMenu( ConfigFlags )
