#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

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


from TrigUpgradeTest.InDetSetup import inDetSetup
inDetSetup()


##################################################################
# egamma chains
##################################################################
if opt.doElectronSlice == True:
    from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import fastCaloMenuSequence
    from TriggerMenuMT.HLTMenuConfig.Egamma.ElectronSequenceSetup import electronMenuSequence
    from TriggerMenuMT.HLTMenuConfig.Egamma.ElectronDef import fastCaloSequenceCfg, electronSequenceCfg, precisionCaloSequenceCfg
    fastCaloStep = RecoFragmentsPool.retrieve( fastCaloSequenceCfg, None )
    electronStep = RecoFragmentsPool.retrieve( electronSequenceCfg, None )
    precisionCaloStep = RecoFragmentsPool.retrieve( precisionCaloSequenceCfg, None )

    step1=ChainStep("ElectronFastCalo", [fastCaloStep])
    step2=ChainStep("ElectronFastTrack", [electronStep])
    step3=ChainStep("ElectronPrecisionCalo", [precisionCaloStep])

    egammaChains  = [
        Chain(name='HLT_e3_etcut1step', Seed="L1_EM3",  ChainSteps=[step1]  ),
        Chain(name='HLT_e3_etcut',      Seed="L1_EM3",  ChainSteps=[step1, step2, step3]  ),
        Chain(name='HLT_e5_etcut',      Seed="L1_EM3",  ChainSteps=[step1, step2, step3]  ),
        Chain(name='HLT_e7_etcut',      Seed="L1_EM3",  ChainSteps=[step1, step2, step3]  )
        ]
    testChains += egammaChains

##################################################################
# photon chains
##################################################################
if opt.doPhotonSlice == True:
    from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import fastCaloMenuSequence
    from TriggerMenuMT.HLTMenuConfig.Egamma.PhotonSequenceSetup import photonMenuSequence

    fastCaloStep = fastCaloMenuSequence("Gamma")
    photonstep   = photonMenuSequence()

    photonChains = [
        Chain(name='HLT_g5_etcut', Seed="L1_EM3",  ChainSteps=[ ChainStep("Step1_g5_etcut", [fastCaloStep]),  ChainStep("Step2_g5_etcut", [photonstep])]  )
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
    # Full scan MS tracking step
    stepFSmuEFSA=ChainStep("Step_FSmuEFSA", [muEFSAFSSequence()])
    stepFSmuEFCB=ChainStep("Step_FSmuEFCB", [muEFCBFSSequence()])


    # 2muons
    step1_2mufast= ChainStep("Step1_2muFast", [ muFastSequence(),   muFastSequence()])
    step2_2muComb= ChainStep("Step1_2muComb", [ muCombSequence(),   muCombSequence()])
    step3_2muEFSA= ChainStep("Step3_2muEFSA", [ muEFSASequence(),   muEFSASequence()])
    step4_2muEFCB= ChainStep("Step4_2muEFCB", [ muEFCBSequence(),   muEFCBSequence()])
    

    emptyStep=ChainStep("Step2_empty")

    ## single muon trigger  
    MuonChains += [Chain(name='HLT_mu6fast',   Seed="L1_MU6",  ChainSteps=[ step1mufast ])]
    MuonChains += [Chain(name='HLT_mu6Comb',   Seed="L1_MU6",  ChainSteps=[ step1mufast, step2muComb ])]
    MuonChains += [Chain(name='HLT_mu6',       Seed="L1_MU6",  ChainSteps=[ step1mufast, step2muComb, step3muEFSA, step4muEFCB ])]
    MuonChains += [Chain(name='HLT_mu6msonly', Seed="L1_MU6",  ChainSteps=[ step1mufast, emptyStep,   step3muEFSA ])] # removed due to muEFSA isuue(?)
    MuonChains += [Chain(name='HLT_mu20_ivar', Seed="L1_MU6",  ChainSteps=[ step1mufast, step2muComb, step3muIso ])]

    # multi muon trigger 
    MuonChains += [Chain(name='HLT_2mu6Comb', Seed="L1_MU6", ChainSteps=[ step1mufast, step2muComb ])]
    MuonChains += [Chain(name='HLT_2mu6',     Seed="L1_MU6", ChainSteps=[ step1mufast, step2muComb, step3muEFSA, step4muEFCB ])]        

  ##  MuonChains += [Chain(name='HLT_2mu6Comb', Seed="L1_2MU6", ChainSteps=[ step1_2mufast, step2_2muComb ])]
  ##  MuonChains += [Chain(name='HLT_2mu6',     Seed="L1_2MU6", ChainSteps=[ step1_2mufast, step2_2muComb, step3_2muEFSA, step4_2muEFCB ])]        

    #FS Muon trigger
    MuonChains += [Chain(name='HLT_mu6nol1', Seed="L1_MU6", ChainSteps=[stepFSmuEFSA, stepFSmuEFCB])] 

    testChains += MuonChains


##################################################################
# jet chains
##################################################################
if opt.doJetSlice == True:
    #from TriggerMenuMT.HLTMenuConfig.Jet.JetSequenceSetup import jetMenuSequence
    from TrigUpgradeTest.jetMenuDefs import jetMenuSequence

    # small-R jets, different calibrations
    jetSeq_a4_emtopo = jetMenuSequence("a4_emtopo_subjesis", "TrigJetHypoAlgMT_a4_emtopo")
    step_a4_emtopo =ChainStep("Step_jet_a4_emtopo", [jetSeq_a4_emtopo])

    jetSeq_a4_emtopo_subjes = jetMenuSequence("a4_emtopo_subjes", "TrigJetHypoAlgMT_a4_emtopo_subjes")
    step_a4_emtopo_subjes = ChainStep("Step_jet_a4_subjes_emtopo", [jetSeq_a4_emtopo_subjes])

    jetSeq_a4_emtopo_nocalib = jetMenuSequence("a4_emtopo_nocalib", "TrigJetHypoAlgMT_a4_emtopo_nocalib")
    step_a4_emtopo_nocalib=ChainStep("Step_jet_a4_nocalib_emtopo", [jetSeq_a4_emtopo_nocalib])

    jetSeq_a4_lcw = jetMenuSequence("a4_lcw_subjesis", "TrigJetHypoAlgMT_a4_lcw")
    step_a4_lcw=ChainStep("Step_jet_a4_lcw", [jetSeq_a4_lcw])

    # large-R jets
    jetSeq_a10_lcw_subjes = jetMenuSequence("a10_lcw_subjes", "TrigJetHypoAlgMT_a10_lcw_subjes")
    step_a10_lcw_subjes=ChainStep("Step_jet_a10_subjes_lcw", [jetSeq_a10_lcw_subjes])

    jetSeq_a10r = jetMenuSequence("a10r_emtopo_subjesis", "TrigJetHypoAlgMT_a10r")
    step_a10r=ChainStep("Step_jet_a10r", [jetSeq_a10r])
    
    jetChains  = [
        Chain(name='HLT_j45', Seed="L1_J20",  ChainSteps=[step_a4_emtopo]  ),
        Chain(name='HLT_j85', Seed="L1_J20",  ChainSteps=[step_a4_emtopo]  ),
        Chain(name='HLT_j420', Seed="L1_J20",  ChainSteps=[step_a4_emtopo]  ),
        Chain(name='HLT_j260_320eta490', Seed="L1_J20",  ChainSteps=[step_a4_emtopo]  ),
        Chain(name='HLT_j225_gsc420_boffperf_split', Seed="L1_J20",  ChainSteps=[step_a4_emtopo]  ),
        Chain(name='HLT_j0_vbenfSEP30etSEP34mass35SEP50fbet', Seed="L1_J20",  ChainSteps=[step_a4_emtopo]  ),
        Chain(name='HLT_j460_a10_lcw_subjes', Seed="L1_J20",  ChainSteps=[step_a10_lcw_subjes]  ),
        Chain(name='HLT_j460_a10r', Seed="L1_J20",  ChainSteps=[step_a10r]  ),
        Chain(name='HLT_3j200', Seed="L1_J20",  ChainSteps=[step_a4_emtopo]  ),
        Chain(name='HLT_5j70_0eta240', Seed="L1_J20",  ChainSteps=[step_a4_emtopo]  ), # 5j70_0eta240_L14J15 (J20 until multi-object L1 seeds supported) 
        ]

    testChains += jetChains


##################################################################
# bjet chains
##################################################################
if opt.doBJetSlice == True:
    from TriggerMenuMT.HLTMenuConfig.Bjet.BjetSequenceSetup import getBJetSequence

    step1 = ChainStep("Step1_bjet", [getBJetSequence('j')])
    step2 = ChainStep("Step2_bjet", [getBJetSequence('gsc')])

    bjetChains  = [                                                                                                                                                                         
          Chain(name='HLT_j35_gsc45_boffperf_split' , Seed="L1_J20",  ChainSteps=[step1,step2] ),
          Chain(name='HLT_j35_gsc45_bmv2c1070_split', Seed="L1_J20",  ChainSteps=[step1,step2] ),
          Chain(name='HLT_j35_gsc45_bmv2c1070'      , Seed="L1_J20",  ChainSteps=[step1,step2] )
        ]
    testChains += bjetChains
    
##################################################################
# tau chains
##################################################################
if opt.doTauSlice == True:
  from TriggerMenuMT.HLTMenuConfig.Tau.TauMenuSequences import getTauSequence
  step1=ChainStep("Step1_tau", [getTauSequence('calo')])
  step2=ChainStep("Step2_tau", [getTauSequence('track_core')])
  
  
  tauChains  = [
      Chain(name='HLT_tau0_perf_ptonly_L1TAU12',  Seed="L1_TAU12",  ChainSteps=[step1, step2] ),
      Chain(name='HLT_tau25_medium1_tracktwo', Seed="L1_TAU12IM",  ChainSteps=[step1, step2] ),
      ]
  testChains += tauChains

##################################################################
# MET chains
##################################################################
if opt.doMETSlice == True:
    from TriggerMenuMT.HLTMenuConfig.MET.METMenuSequences import metCellMenuSequence

    metCellSeq = metCellMenuSequence()
    metCellStep = ChainStep("Step1_met_cell", [metCellSeq])
    metChains = [
        Chain(name="HLT_xe65_L1XE50", Seed="L1_XE50", ChainSteps=[metCellStep]),
        Chain(name="HLT_xe30_L1XE10", Seed="L1_XE10", ChainSteps=[metCellStep])
        ]

    testChains += metChains

##################################################################
# B-physics and light states chains
##################################################################
if opt.doBphysicsSlice == True:
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSequenceSetup import muFastSequence, muEFSASequence, muEFCBSequence
    from TriggerMenuMT.HLTMenuConfig.Bphysics.BphysicsSequenceSetup import dimuL2Sequence
	
    BphysChains  = []

    step1mufast=ChainStep("Step1_muFast",  [muFastSequence()])
    step2L2Dimu=ChainStep("Step2_L2Dimu",  [dimuL2Sequence()])
    
    #still to come
    #step3muEFSA=ChainStep("Step3_muEFSA",   [ muEFSASequence() ])
    #step4muEFCB=ChainStep("Step4_muEFCB",   [ muEFCBSequence() ])
    #step5EFJpsi=ChainStep("Step5_L2Jpsimumu",[jpsimumuEFSequence()])

    BphysChains += [Chain(name='HLT_2mu6_bJpsimumu_L12MU6', Seed="L1_MU6", ChainSteps=[ step1mufast, step2L2Dimu])]
    BphysChains += [Chain(name='HLT_2mu4_bBmumu_L12MU4',    Seed="L1_MU4", ChainSteps=[ step1mufast, step2L2Dimu])]
    BphysChains += [Chain(name='HLT_2mu4_bUpsimumu_L12MU4', Seed="L1_MU4", ChainSteps=[ step1mufast, step2L2Dimu])]
    BphysChains += [Chain(name='HLT_2mu4_bJpsimumu_L12MU4', Seed="L1_MU4", ChainSteps=[ step1mufast, step2L2Dimu])]

    #BphysChains += [Chain(name='HLT_mu6_mu4_bJpsimumu_L1MU6_2MU4', Seed="L1_MU4", ChainSteps=[ step1mufast, step2L2Dimu])]
                                        #to come: step3muEFSA, step4muEFCB, step5EFJpsi])]
    testChains += BphysChains  

##################################################################
# combined chains
##################################################################
if opt.doComboSlice == True:
    # combo chains
    comboStep=ChainStep("Step1_mufast_et", [fastCaloStep,muFastSequence()])

    comboChains =  [Chain(name='HLT_e3_etcut_mu6', Seed="L1_EM8I_MU10",  ChainSteps=[comboStep ])]
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
for a in AthSequencer("HLTAllSteps").getChildren():
    if isinstance(a, DecisionHandling.DecisionHandlingConf.TriggerSummaryAlg):
        a.OutputLevel = DEBUG


# this part uses parts from the NewJO configuration, it is very hacky for the moment

from TriggerJobOpts.TriggerConfig import collectHypos, collectFilters, collectDecisionObjects, triggerOutputStreamCfg
hypos = collectHypos(topSequence)
filters = collectFilters(topSequence)

# try to find L1Decoder
from AthenaCommon.CFElements import findAlgorithm,findSubSequence
l1decoder = findAlgorithm(topSequence,'L1Decoder')
if not l1decoder:
    l1decoder = findAlgorithm(topSequence,'L1EmulationTest')

if l1decoder:
    decObj = collectDecisionObjects( hypos, filters, l1decoder )
    print decObj

    from TrigEDMConfig.TriggerEDMRun3 import TriggerHLTList
    ItemList  = [ 'xAOD::TrigCompositeContainer#{}'.format(d) for d in decObj ]
    ItemList += [ 'xAOD::TrigCompositeAuxContainer#{}Aux.'.format(d) for d in decObj ]
    ItemList += [ k[0] for k in TriggerHLTList if 'ESD' in k[1] and "TrigComposite" not in k[0] ]
    ItemList += [ k[0] for k in TriggerHLTList if 'ESD' in k[1] and "TrigComposite" in k[0] ]
    ItemList += [ 'xAOD::TrigCompositeAuxContainer#{}Aux.'.format(k[0].split("#")[1]) for k in TriggerHLTList if 'ESD' in k[1] and "TrigComposite" in k[0] ]
    ItemList += [ "xAOD::EventInfo#EventInfo" ]

    ItemList = list(set(ItemList))

else:
    ItemList = []


import AthenaPoolCnvSvc.WriteAthenaPool
from OutputStreamAthenaPool.OutputStreamAthenaPool import  createOutputStream
StreamESD=createOutputStream("StreamESD","myESD.pool.root",True)
StreamESD.ItemList = ItemList


HLTTop = findSubSequence(topSequence, "HLTTop")
