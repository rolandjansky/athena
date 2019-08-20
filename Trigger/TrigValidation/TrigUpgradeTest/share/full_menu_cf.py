#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
__log = logging.getLogger('full_menu')

# import flags
from RecExConfig.RecFlags  import rec
rec.doESD=True
rec.doWriteESD=True

include("TrigUpgradeTest/testHLT_MT.py")

##########################################
# menu
###########################################
# note : L1 Seeds must be declared in CTP file: for example
# nightly/Athena/22.0.1/InstallArea/x86_64-slc6-gcc62-opt/XML/TriggerMenuXML/LVL1config_Physics_pp_v7.xml
##########################################


from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep, RecoFragmentsPool

testChains = []



##################################################################
# egamma chains
##################################################################
if opt.doElectronSlice == True:
    from TriggerMenuMT.HLTMenuConfig.Egamma.ElectronDef import electronFastCaloCfg, fastElectronSequenceCfg, precisionCaloSequenceCfg
    fastCaloSeq = RecoFragmentsPool.retrieve( electronFastCaloCfg, None )
    electronSeq = RecoFragmentsPool.retrieve( fastElectronSequenceCfg, None )
    precisionCaloSeq = RecoFragmentsPool.retrieve( precisionCaloSequenceCfg, None )

    FastCaloStep = ChainStep("ElectronFastCaloStep", [fastCaloSeq])
    FastElectronStep = ChainStep("ElectronFastTrackStep", [electronSeq])
    PrecisionCaloStep = ChainStep("ElectronPrecisionCaloStep", [precisionCaloSeq])

    egammaChains  = [
        Chain(name='HLT_e3_etcut1step_L1EM3',  L1Thresholds=["EM3"],  ChainSteps=[FastCaloStep]  ),
        Chain(name='HLT_e3_etcut_L1EM3',       L1Thresholds=["EM3"],  ChainSteps=[FastCaloStep, FastElectronStep, PrecisionCaloStep]  ),
        Chain(name='HLT_e5_etcut_L1EM3',       L1Thresholds=["EM3"],  ChainSteps=[FastCaloStep, FastElectronStep, PrecisionCaloStep]  ),
        Chain(name='HLT_e7_etcut_L1EM3',       L1Thresholds=["EM3"],  ChainSteps=[FastCaloStep, FastElectronStep, PrecisionCaloStep]  )
        ]

#    DiEleStep1=ChainStep("DiEleStep1",[fastCaloSeq, fastCaloSeq], multiplicity=2) #same step
#    DiEleStep2=ChainStep("DiEleStep2",[electronSeq, electronSeq], multiplicity=2) #need to be: one leg with only one step, one with 3 steps!
    
#    egammaChains += [Chain(name='HLT_e5_etcut1step_e8_etcut',  ChainSteps=[DiEleStep1, DiEleStep2 ]  )]
    testChains += egammaChains

##################################################################
# photon chains
##################################################################
if opt.doPhotonSlice == True:

    from TriggerMenuMT.HLTMenuConfig.Egamma.PhotonDef import fastPhotonCaloSequenceCfg, fastPhotonSequenceCfg, precisionPhotonCaloSequenceCfg

    fastCaloSeq = RecoFragmentsPool.retrieve( fastPhotonCaloSequenceCfg, None )
    fastPhotonSeq = RecoFragmentsPool.retrieve( fastPhotonSequenceCfg, None )
    precisionCaloPhotonSeq = RecoFragmentsPool.retrieve( precisionPhotonCaloSequenceCfg, None)

    FastCaloStep = ChainStep("PhotonFastCaloStep", [fastCaloSeq])
    fastPhotonStep = ChainStep("PhotonStep2", [fastPhotonSeq])
    precisionCaloPhotonStep = ChainStep("precisionCaloPhotonStep", [precisionCaloPhotonSeq])

    photonChains = [
        Chain(name='HLT_g5_etcut_L1EM3',    L1Thresholds=["EM3"],  ChainSteps=[ FastCaloStep,  fastPhotonStep, precisionCaloPhotonStep]  )
        ]

    testChains += photonChains

##################################################################
# muon chains
##################################################################
if opt.doMuonSlice == True:
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSequenceSetup import muFastSequence, muCombSequence, muEFSASequence, muIsoSequence, muEFCBSequence, muEFSAFSSequence, muEFCBFSSequence

    MuonChains  = []

    # step1
    step1mufast=ChainStep("Step1_muFast", [ muFastSequence() ])
    # step2
    step2muComb=ChainStep("Step2_muComb", [ muCombSequence() ])

    # step3
    step3muEFSA=ChainStep("Step3_muEFSA", [ muEFSASequence() ])
    step3muIso =ChainStep("Step3_muIso",  [ muIsoSequence() ])
    # step4
    step4muEFCB=ChainStep("Step4_muEFCB", [ muEFCBSequence() ])

    emptyStep=ChainStep("Step2_empty")

    ## single muon trigger  
    MuonChains += [Chain(name='HLT_mu6fast_L1MU6',     L1Thresholds=["MU6"], ChainSteps=[ step1mufast ])]
    MuonChains += [Chain(name='HLT_mu6Comb_L1MU6',     L1Thresholds=["MU6"], ChainSteps=[ step1mufast, step2muComb ])]
    MuonChains += [Chain(name='HLT_mu6_L1MU6',         L1Thresholds=["MU6"], ChainSteps=[ step1mufast, step2muComb, step3muEFSA, step4muEFCB ])]
    MuonChains += [Chain(name='HLT_mu6msonly_L1MU6',   L1Thresholds=["MU6"], ChainSteps=[ step1mufast, emptyStep,   step3muEFSA ])] # removed due to muEFSA isuue(?)
    MuonChains += [Chain(name='HLT_mu20_ivar_L1MU6',   L1Thresholds=["MU6"], ChainSteps=[ step1mufast, step2muComb, step3muIso ])]

    # multi muon trigger
    # 2muons symmetric
    step1_2mufast_sym= ChainStep("Step1_2muFast_sym", [ muFastSequence()], multiplicity=2)
    step2_2muComb_sym= ChainStep("Step1_2muComb_sym", [ muCombSequence()], multiplicity=2)
    
    step3_2muEFSA_sym= ChainStep("Step3_2muEFSA_sym", [ muEFSASequence()], multiplicity=2)
    step4_2muEFCB_sym= ChainStep("Step4_2muEFCB_sym", [ muEFCBSequence()], multiplicity=2)
 
    MuonChains += [Chain(name='HLT_2mu6Comb_L12MU6',  L1Thresholds=["MU6"], ChainSteps=[ step1_2mufast_sym, step2_2muComb_sym ])]

    # 2muons asymmetric (this will change): 2 sequences, 2 seeds
    step1_2mufast_asym= ChainStep("Step1_2muFast_asym", [ muFastSequence(), muFastSequence()], multiplicity=2)
    step2_2muComb_asym= ChainStep("Step1_2muComb_asym", [ muCombSequence(), muCombSequence()], multiplicity=2)
    
    step3_2muEFSA_asym= ChainStep("Step3_2muEFSA_asym", [ muEFSASequence(), muEFSASequence()], multiplicity=2)
    step4_2muEFCB_asym= ChainStep("Step4_2muEFCB_asym", [ muEFCBSequence(), muEFCBSequence()], multiplicity=2)
    
    MuonChains += [Chain(name='HLT_mu6_mu4_L12MU4',
                             L1Thresholds=["MU4", "MU4"],
                             ChainSteps=[ step1_2mufast_asym, step2_2muComb_asym ])]
            
    MuonChains += [Chain(name='HLT_mu6_mu4_L1MU4_MU6',
                             L1Thresholds=["MU4", "MU6"],
                             ChainSteps=[ step1_2mufast_asym, step2_2muComb_asym ])]        

    
    #FS Muon trigger
    # Full scan MS tracking step
    stepFSmuEFSA=ChainStep("Step_FSmuEFSA", [muEFSAFSSequence()])
    stepFSmuEFCB=ChainStep("Step_FSmuEFCB", [muEFCBFSSequence()])
    MuonChains += [Chain(name='HLT_mu6nol1_L1MU6', L1Thresholds=["MU6"],  ChainSteps=[stepFSmuEFSA, stepFSmuEFCB])] 

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
        Chain(name='HLT_j45_L1J20',  L1Thresholds=["J20"],   ChainSteps=[step_a4_tc_em]  ),
        Chain(name='HLT_j85_L1J20',  L1Thresholds=["J20"],ChainSteps=[step_a4_tc_em]  ),
        Chain(name='HLT_j420_L1J20', L1Thresholds=["J20"], ChainSteps=[step_a4_tc_em]  ),
        Chain(name='HLT_j260_320eta490_L1J20',  L1Thresholds=["J20"], ChainSteps=[step_a4_tc_em]  ),
      #  Chain(name='HLT_j225_gsc420_boffperf_split',   ChainSteps=[step_a4_tc_em]  ),
        Chain(name='HLT_j0_vbenfSEP30etSEP34mass35SEP50fbet_L1J20',  L1Thresholds=["J20"], ChainSteps=[step_a4_tc_em]  ),
        Chain(name='HLT_j460_a10_lcw_subjes_L1J20', L1Thresholds=["J20"], ChainSteps=[step_a10_tc_lcw_subjes]  ),
        Chain(name='HLT_j460_a10r_L1J20', L1Thresholds=["J20"], ChainSteps=[step_a10r]  ),
        Chain(name='HLT_3j200_L1J20', L1Thresholds=["J20"], ChainSteps=[step_a4_tc_em]  ),
        Chain(name='HLT_5j70_0eta240_L1J20', L1Thresholds=["J20"], ChainSteps=[step_a4_tc_em]  ), # 5j70_0eta240_L14J15 (J20 until multi-object L1 seeds supported)
        ]

    testChains += jetChains


##################################################################
# bjet chains
##################################################################
if opt.doBjetSlice == True:
    from TriggerMenuMT.HLTMenuConfig.Bjet.BjetSequenceSetup import getBJetSequence

    step1 = ChainStep("Step1_bjet", [getBJetSequence('j')])
    step2 = ChainStep("Step2_bjet", [getBJetSequence('gsc')])

    bjetChains  = [                                                                                                                                                                         
          Chain(name='HLT_j35_gsc45_boffperf_split_L1J20' , L1Thresholds=["J20"], ChainSteps=[step1,step2] ),
          Chain(name='HLT_j35_gsc45_bmv2c1070_split_L1J20', L1Thresholds=["J20"], ChainSteps=[step1,step2] ),
          Chain(name='HLT_j35_gsc45_bmv2c1070_L1J20'      , L1Thresholds=["J20"], ChainSteps=[step1,step2] )
        ]
    testChains += bjetChains
    
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
      Chain(name='HLT_tau0_perf_ptonly_L1TAU12',              L1Thresholds=["TAU12"], ChainSteps=[step1, step2] ),
      Chain(name='HLT_tau25_medium1_tracktwo_L1TAU12IM',      L1Thresholds=["TAU12IM"],  ChainSteps=[step1, step2TP] ),
      Chain(name='HLT_tau35_mediumRNN_tracktwoMVA_L1TAU12IM', L1Thresholds=["TAU20IM"], ChainSteps=[step1MVA, step2PT]),
      ]
  testChains += tauChains

##################################################################
# MET chains
##################################################################
if opt.doMETSlice == True:
    from TriggerMenuMT.HLTMenuConfig.MET.METMenuSequences import metCellMenuSequence, metClusterPufitMenuSequence

    metCellSeq = metCellMenuSequence()
    metClusterPufitSeq = metClusterPufitMenuSequence()

    metCellStep = ChainStep("Step1_met_cell", [metCellSeq])
    metClusterPufitStep          = ChainStep("Step1_met_clusterpufit", [metClusterPufitSeq])
    comboStep_cell_clusterpufit  = ChainStep("Step1_combo_cell_clusterpufit", [metCellSeq, metClusterPufitSeq], multiplicity=2)

    metChains = [
        Chain(name="HLT_xe65_L1XE50",         L1Thresholds=["XE50"], ChainSteps=[metCellStep]),
        Chain(name="HLT_xe30_L1XE10",         L1Thresholds=["XE10"], ChainSteps=[metCellStep]),
        Chain(name="HLT_xe30_tcpufit_L1XE10", L1Thresholds=["XE10"], ChainSteps=[metClusterPufitStep]),
        Chain(name='HLT_xe30_cell_xe30_tcpufit_L12XE10',  L1Thresholds=["XE10","XE10"], ChainSteps=[comboStep_cell_clusterpufit ]) 
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

    BphysChains += [Chain(name='HLT_2mu4_bDimu_L12MU4',     L1Thresholds=["MU4"], ChainSteps=[ step1mufast, step2L2Dimu, step3muEFSA, step4muEFCB, step5EFDimu])]
    BphysChains += [Chain(name='HLT_2mu6_bJpsimumu_L12MU6', L1Thresholds=["MU6"], ChainSteps=[ step1mufast, step2L2Dimu])]
    BphysChains += [Chain(name='HLT_2mu4_bBmumu_L12MU4',    L1Thresholds=["MU4"], ChainSteps=[ step1mufast, step2L2Dimu])]
    BphysChains += [Chain(name='HLT_2mu4_bUpsimumu_L12MU4', L1Thresholds=["MU4"], ChainSteps=[ step1mufast, step2L2Dimu])]
    BphysChains += [Chain(name='HLT_2mu4_bJpsimumu_L12MU4', L1Thresholds=["MU4"], ChainSteps=[ step1mufast, step2L2Dimu])]

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

    comboStep_et_mufast           = ChainStep("Step1_et_mufast", [fastCaloSeq, muFastSequence()], multiplicity=2)
    comboStep_mufast_etcut1_step1 = ChainStep("Step1_mufast_etcut1", [muFastSequence(), fastCaloSeq], multiplicity=2)


    comboChains =  [Chain(name='HLT_e3_etcut_mu6_L1EM8I_MU10', L1Thresholds=["EM8I", "MU10"],  ChainSteps=[comboStep_et_mufast ])]
 #   comboChains += [Chain(name='HLT_mu8fast_e8_etcut1step',   ChainSteps=[ comboStep_mufast_etcut1_step1 ])]
    testChains += comboChains



##########################################
# CF construction
##########################################

##### Make all HLT #######
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import makeHLTTree
makeHLTTree(testChains)


##########################################
# Some debug
##########################################
from AthenaCommon.AlgSequence import dumpSequence, AthSequencer
dumpSequence(topSequence)


import DecisionHandling
from AthenaCommon.CFElements import findAlgorithm,findSubSequence
for a in findSubSequence(topSequence, "HLTAllSteps").getChildren():
    if isinstance(a, DecisionHandling.DecisionHandlingConf.TriggerSummaryAlg):
        a.OutputLevel = DEBUG


# this part uses parts from the NewJO configuration, it is very hacky for the moment

from TriggerJobOpts.TriggerConfig import collectHypos, collectFilters, collectDecisionObjects, triggerOutputStreamCfg
hypos = collectHypos(findSubSequence(topSequence, "HLTAllSteps"))
filters = collectFilters(findSubSequence(topSequence, "HLTAllSteps"))

# try to find L1Decoder
l1decoder = findAlgorithm(topSequence,'L1Decoder')
if not l1decoder:
    l1decoder = findAlgorithm(topSequence,'L1EmulationTest')

if l1decoder:
    decObj = collectDecisionObjects( hypos, filters, l1decoder )
    __log.debug("Decision Objects to export to ESD [hack method - should be replaced with triggerRunCfg()]")
    __log.debug(decObj)

    from TrigEDMConfig.TriggerEDMRun3 import TriggerHLTListRun3
    ItemList  = [ 'xAOD::TrigCompositeContainer#{}'.format(d) for d in decObj ]
    ItemList += [ 'xAOD::TrigCompositeAuxContainer#{}Aux.'.format(d) for d in decObj ]
    ItemList += [ k[0] for k in TriggerHLTListRun3 if 'ESD' in k[1] and "TrigComposite" not in k[0] ]
    ItemList += [ k[0] for k in TriggerHLTListRun3 if 'ESD' in k[1] and "TrigComposite" in k[0] ]
    ItemList += [ 'xAOD::TrigCompositeAuxContainer#{}Aux.'.format(k[0].split("#")[1]) for k in TriggerHLTListRun3 if 'ESD' in k[1] and "TrigComposite" in k[0] ]
    ItemList += [ "xAOD::EventInfo#EventInfo" ]

    ItemList = list(set(ItemList))

else:
    ItemList = []


import AthenaPoolCnvSvc.WriteAthenaPool
from OutputStreamAthenaPool.OutputStreamAthenaPool import  createOutputStream
StreamESD=createOutputStream("StreamESD","myESD.pool.root",True)
StreamESD.ItemList = ItemList


HLTTop = findSubSequence(topSequence, "HLTTop")
