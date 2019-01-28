#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

# import flags
include("TrigUpgradeTest/testHLT_MT.py")

#Currently only runs egamma and mu chains but expect to expand


##########################################
# menu
###########################################
# note : L1 Seeds must be declared in CTP file: for example
# nightly/Athena/22.0.1/InstallArea/x86_64-slc6-gcc62-opt/XML/TriggerMenuXML/LVL1config_Physics_pp_v7.xml
##########################################

doElectron = True
doPhoton = False
doMuon   = True
doJet    = False
doCombo  = True

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep

EnabledElChains = []
EnabledMuChains = []
EnabledMuComboChains = []
EnabledElComboChains = []

testChains = []



##################################################################
# egamma chains
##################################################################
if (doElectron):
    from TrigUpgradeTest.CaloMenuDefs import fastCaloMenuSequence
    from TrigUpgradeTest.electronMenuDefs import electronMenuSequence
    fastCaloStep=fastCaloMenuSequence()
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
if (doPhoton):
    from TrigUpgradeTest.CaloMenuDefs import fastCaloMenuSequence
    from TrigUpgradeTest.photonMenuDefs import photonMenuSequence

    fastCaloStep=fastCaloMenuSequence()
    photonstep= photonMenuSequence()

    photonChains = [
        Chain(name='HLT_g5_etcut', Seed="L1_EM3",  ChainSteps=[ ChainStep("Step1_g5_etcut", [fastCaloStep]),  ChainStep("Step2_g5_etcut", [photonstep])]  )
        ]

    testChains += photonChains

##################################################################
# muon chains
##################################################################
if (doMuon):
    from TrigUpgradeTest.muMenuDefs import muFastStep, muCombStep, muEFSAStep, muEFMSStep, muIsoStep
    MuonChains  = []
    muFastStep1 = muFastStep()
    muCombStep2 = muCombStep()

    # step1
    step1mufast=ChainStep("Step1_mufast", [ muFastStep1 ])
    # step2
    step2muComb=ChainStep("Step2_muComb", [ muCombStep2 ])
    # step3
    
    MuonChains += [Chain(name='HLT_mu6', Seed="L1_MU6",  ChainSteps=[step1mufast ])]
    MuonChains += [Chain(name='HLT_2mu6', Seed="L1_MU6", ChainSteps=[step1mufast ])]
    if TriggerFlags.doID==True:
        MuonChains += [Chain(name='HLT_mu6Comb', Seed="L1_MU6",  ChainSteps=[step1mufast, step2muComb ])]
        MuonChains += [Chain(name='HLT_2mu6Comb', Seed="L1_MU6", ChainSteps=[step1mufast, step2muComb ])]

    testChains += MuonChains


##################################################################
# jet chains
##################################################################
if (doJet):
    from TrigUpgradeTest.jetMenuDefs import jetSequence

    jetSeq1 = jetSequence()
    jetstep1=ChainStep("Step1_jet", [jetSeq1])
    
    jetChains  = [
        Chain(name='HLT_j85',  Seed="L1_J20",  ChainSteps=[jetstep1]  ),
        Chain(name='HLT_j100', Seed="L1_J20",  ChainSteps=[jetstep1]  )
        ]
    testChains += jetChains

##################################################################
# combined chains
##################################################################
if (doCombo):
    # combo chains
    comboChains= []

    comboStep=ChainStep("Step1_mufast_et", [fastCaloStep,muFastStep1])
    comboChains +=  [Chain(name='HLT_e3_etcut_mu6', Seed="L1_EM8I_MU10",  ChainSteps=[comboStep ])]
    testChains += comboChains


#################################
# Configure L1Decoder
#################################

# this is a temporary hack to include new test chains
EnabledChainNamesToCTP = dict([ (c.name, c.seed)  for c in testChains])
topSequence.L1DecoderTest.ChainToCTPMapping = EnabledChainNamesToCTP



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
