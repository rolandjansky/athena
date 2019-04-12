#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

# import flags
from RecExConfig.RecFlags  import rec
rec.doESD=True
rec.doWriteESD=True

include("TrigUpgradeTest/testHLT_MT.py")

#Currently only runs egamma and mu chains but expect to expand


##########################################
# menu
###########################################
# note : L1 Seeds must be declared in CTP file: for example
# nightly/Athena/22.0.1/InstallArea/x86_64-slc6-gcc62-opt/XML/TriggerMenuXML/LVL1config_Physics_pp_v7.xml
##########################################


from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep

testChains = []


def inDetSetup():
    from InDetRecExample.InDetJobProperties import InDetFlags
    InDetFlags.doCaloSeededBrem = False
    InDetFlags.InDet25nsec = True 
    InDetFlags.doPrimaryVertex3DFinding = False 
    InDetFlags.doPrintConfigurables = False
    InDetFlags.doResolveBackTracks = True 
    InDetFlags.doSiSPSeededTrackFinder = True
    InDetFlags.doTRTPhaseCalculation = True
    InDetFlags.doTRTSeededTrackFinder = True
    InDetFlags.doTruth = False
    InDetFlags.init()

    ### PixelLorentzAngleSvc and SCTLorentzAngleSvc ###
    include("InDetRecExample/InDetRecConditionsAccess.py")


inDetSetup()


##################################################################
# egamma chains
##################################################################
if opt.doElectronSlice == True:
    from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import fastCaloMenuSequence
    from TriggerMenuMT.HLTMenuConfig.Egamma.ElectronSequenceSetup import electronMenuSequence
    fastCaloStep=fastCaloMenuSequence("Ele")
    electronStep=electronMenuSequence()

    step1=ChainStep("Step1_etcut", [fastCaloStep])
    step2=ChainStep("Step2_etcut", [electronStep])

    egammaChains  = [
        Chain(name='HLT_e3_etcut1step', Seed="L1_EM3",  ChainSteps=[step1]  ),
        Chain(name='HLT_e3_etcut',      Seed="L1_EM3",  ChainSteps=[step1, step2]  ),
        Chain(name='HLT_e5_etcut',      Seed="L1_EM3",  ChainSteps=[step1, step2]  ),
        Chain(name='HLT_e7_etcut',      Seed="L1_EM3",  ChainSteps=[step1, step2]  )
        ]
    testChains += egammaChains

##################################################################
# photon chains
##################################################################
if opt.doPhotonSlice == True:
    from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import fastCaloMenuSequence
    from TrigUpgradeTest.photonMenuDefs import photonMenuSequence

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
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSequenceSetup import muFastSequence, muCombSequence, muEFMSSequence, muEFSASequence, muIsoSequence, muEFCBSequence, muEFSAFSSequence, muEFCBFSSequence, inDetSetup

    inDetSetup()

    MuonChains  = []

    # step1
    step1mufast=ChainStep("Step1_muFast", [ muFastSequence() ])
    # step2
    step2muComb=ChainStep("Step2_muComb", [ muCombSequence() ])
    step2muEFMS=ChainStep("Step2_muEFMS", [ muEFMSSequence() ])
    # step3
    step3muEFSA=ChainStep("Step3_muEFSA", [ muEFSASequence() ])
    step3muIso =ChainStep("Step3_muIso",  [ muIsoSequence() ])
    # step4
    step4muEFCB=ChainStep("Step4_muEFCB", [ muEFCBSequence() ])
    # Full scan MS tracking step
    stepFSmuEFSA=ChainStep("Step_FSmuEFSA", [muEFSAFSSequence()])
    stepFSmuEFCB=ChainStep("Step_FSmuEFCB", [muEFCBFSSequence()])


    ## single muon trigger  
    MuonChains += [Chain(name='HLT_mu6fast',   Seed="L1_MU6",  ChainSteps=[ step1mufast ])]
    MuonChains += [Chain(name='HLT_mu6Comb',   Seed="L1_MU6",  ChainSteps=[ step1mufast, step2muComb ])]
    #MuonChains += [Chain(name='HLT_mu6msonly', Seed="L1_MU6",  ChainSteps=[ step1mufast, step2muEFMS ])] # removed due to muEFSA isuue(?)
    MuonChains += [Chain(name='HLT_mu6',       Seed="L1_MU6",  ChainSteps=[ step1mufast, step2muComb, step3muEFSA, step4muEFCB ])]
    MuonChains += [Chain(name='HLT_mu20_ivar', Seed="L1_MU6", ChainSteps=[ step1mufast, step2muComb, step3muIso ])]

    # multi muon trigger 
    MuonChains += [Chain(name='HLT_2mu6Comb', Seed="L1_MU6", ChainSteps=[ step1mufast, step2muComb ])]
    MuonChains += [Chain(name='HLT_2mu6',     Seed="L1_MU6", ChainSteps=[ step1mufast, step2muComb, step3muEFSA, step4muEFCB ])]        

    #FS Muon trigger
    MuonChains += [Chain(name='HLT_mu6nol1', Seed="L1_MU6", ChainSteps=[stepFSmuEFSA, stepFSmuEFCB])] 

    testChains += MuonChains


##################################################################
# jet chains
##################################################################
if opt.doJetSlice == True:
    from TrigUpgradeTest.jetMenuDefs import jetMenuSequence

    jetSeq1 = jetMenuSequence()
    jetstep1=ChainStep("Step1_jet", [jetSeq1])
    
    jetChains  = [
      Chain(name='HLT_j85',  Seed="L1_J20",  ChainSteps=[jetstep1]  ),
      Chain(name='HLT_j45', Seed="L1_J20",  ChainSteps=[jetstep1]  ),
      Chain(name='HLT_j420', Seed='L1_J20', ChainSteps=[jetstep1] ),
      Chain(name='HLT_j225_gsc420_boffperf_split', Seed='L1_J20', ChainSteps=[jetstep1] ),
      Chain(name='HLT_j260_320eta490', Seed='L1_J20', ChainSteps=[jetstep1] ),

      Chain(name='HLT_3j200', Seed='L1_J20', ChainSteps=[jetstep1] ),
      Chain(name='HLT_5j70_0eta240_L14J15', Seed='L1_J20', ChainSteps=[jetstep1] ),
      Chain(name='HLT_j0_vbenfSEP30etSEP34mass35SEP50fbet', Seed='L1_J20',  ChainSteps=[jetstep1]  ),

    ]
    testChains += jetChains




##################################################################
# bjet chains
##################################################################
if opt.doBJetSlice == True:
    from TrigUpgradeTest.bjetMenuDefs import getBJetSequence

    step1 = ChainStep("Step1_bjet", [getBJetSequence('j')])
    step2 = ChainStep("Step2_bjet", [getBJetSequence('gsc')])

    bjetChains  = [                                                                                                                                                                         
          Chain(name='HLT_j35_gsc45_boffperf_split' , Seed="L1_J20",  ChainSteps=[step1,step2] ),
          Chain(name='HLT_j35_gsc45_bmv2c1070_split', Seed="L1_J20",  ChainSteps=[step1,step2] ),
          Chain(name='HLT_j35_gsc45_bmv2c1070'      , Seed="L1_J20",  ChainSteps=[step1,step2] )
        ]
    testChains += bjetChains
    
if opt.doTauSlice == True:
  from TrigUpgradeTest.tauMenuDefs import getTauSequence
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
    from TriggerMenuMT.HLTMenuConfig.MET.metMenuDefs import metCellMenuSequence

    metCellSeq = metCellMenuSequence()
    metCellStep = ChainStep("Step1_met_cell", [metCellSeq])
    metChains = [
        Chain(name="HLT_xe65_L1XE50", Seed="L1_XE50", ChainSteps=[metCellStep]),
        Chain(name="HLT_xe30_L1XE10", Seed="L1_XE10", ChainSteps=[metCellStep])
        ]

    testChains += metChains

##################################################################
# B-physics and light states chains, placeholder for now
##################################################################
if opt.doBLSSlice == True:
  pass
  

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
from AthenaCommon.AlgSequence import dumpSequence
dumpSequence(topSequence)


# this part uses the NewJO configuration, it is very hacky now


from TriggerJobOpts.TriggerConfig import collectHypos, collectFilters, collectDecisionObjects, triggerOutputStreamCfg
hypos = collectHypos(topSequence)
filters = collectFilters(topSequence)
from AthenaCommon.CFElements import findAlgorithm,findSubSequence
decObj = collectDecisionObjects( hypos, filters, findAlgorithm(topSequence, 'L1Decoder') )
print decObj

from TrigEDMConfig.TriggerEDMRun3 import TriggerHLTList
ItemList  = [ 'xAOD::TrigCompositeContainer#{}'.format(d) for d in decObj ]
ItemList += [ 'xAOD::TrigCompositeAuxContainer#{}Aux.'.format(d) for d in decObj ]
ItemList += [ k[0] for k in TriggerHLTList if 'ESD' in k[1] and "TrigComposite" not in k[0] ]
ItemList += [ k[0] for k in TriggerHLTList if 'ESD' in k[1] and "TrigComposite" in k[0] ]
ItemList += [ 'xAOD::TrigCompositeAuxContainer#{}Aux.'.format(k[0].split("#")[1]) for k in TriggerHLTList if 'ESD' in k[1] and "TrigComposite" in k[0] ]
ItemList += [ "xAOD::EventInfo#EventInfo" ]

ItemList = list(set(ItemList))



import AthenaPoolCnvSvc.WriteAthenaPool
from OutputStreamAthenaPool.OutputStreamAthenaPool import  createOutputStream
StreamESD=createOutputStream("StreamESD","myESD.pool.root",True)
StreamESD.ItemList = ItemList


HLTTop = findSubSequence(topSequence, "HLTTop")


