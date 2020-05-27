# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import CAMenuSequence, ChainStep, Chain, getChainStepName, createStepView
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from TrigL2MuonSA.TrigL2MuonSAConfig_newJO import l2MuFastRecoCfg, l2MuFastHypoCfg
from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMufastHypoToolFromDict

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainDict

from AthenaConfiguration.ComponentFactory import CompFactory

def fakeHypoAlgCfg(flags, name="FakeHypoForMuon"):
    HLTTest__TestHypoAlg=CompFactory.HLTTest.TestHypoAlg
    return HLTTest__TestHypoAlg( name, Input="" )

def generateChains( flags, chainDict ):
    chainDict = splitChainDict(chainDict)[0]
    
    stepName = getChainStepName('Muon', 1)
    stepReco, stepView = createStepView(stepName)

    acc = ComponentAccumulator()
    acc.addSequence(stepView)

    l2muFastReco = l2MuFastRecoCfg(flags)
    acc.merge( l2muFastReco, sequenceName=stepReco.getName() )

    ### Set muon step1 ###
    l2muFastHypo = l2MuFastHypoCfg( flags,
                                    name = 'TrigL2MuFastHypo',
                                    muFastInfo = 'MuonL2SAInfo' )

    acc.addEventAlgo(l2muFastHypo, sequenceName=stepView.getName())

    l2muFastSequence = CAMenuSequence( Sequence = l2muFastReco.sequence(),
                                     Maker = l2muFastReco.inputMaker(),
                                     Hypo = l2muFastHypo,
                                     HypoToolGen = TrigMufastHypoToolFromDict,
                                     CA = acc )

    l2muFastSequence.createHypoTools(chainDict)

    l2muFastStep = ChainStep( stepName, [l2muFastSequence] )

    ### Set muon step2 ###
    # Please set up L2muComb step here

    #EF MS only
    stepEFMSName = getChainStepName('EFMSMuon', 2)
    stepEFMSReco, stepEFMSView = createStepView(stepEFMSName)

    #Clone and replace offline flags so we can set muon trigger specific values
    muonflags = flags.cloneAndReplace('Muon', 'Trigger.Offline.Muon')
    muonflags.Muon.useTGCPriorNextBC=True
    muonflags.Muon.enableErrorTuning=False
    muonflags.Muon.MuonTrigger=True
    muonflags.Muon.SAMuonTrigger=True
    muonflags.lock()

    accMS = ComponentAccumulator()
    accMS.addSequence(stepEFMSView)

    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import InViewReco
    recoMS = InViewReco("EFMuMSReco")

    from MuonConfig.MuonSegmentFindingConfig import MooSegmentFinderAlgCfg
    segCfg = MooSegmentFinderAlgCfg(muonflags,name="TrigMooSegmentFinder",UseTGCNextBC=False, UseTGCPriorBC=False)
    recoMS.mergeReco(segCfg)

    from MuonConfig.MuonTrackBuildingConfig import MuonTrackBuildingCfg
    trkCfg = MuonTrackBuildingCfg(muonflags, name="TrigMuPatTrackBuilder")
    recoMS.mergeReco(trkCfg)

    #The MuonCandidateAlg is not quite fully working yet
    #from MuonCombinedConfig.MuonCombinedReconstructionConfig import MuonCombinedMuonCandidateAlgCfg
    #candCfg = MuonCombinedMuonCandidateAlgCfg(muonflags, name = "TrigMuonCandidateAlg")
    #recoMS.mergeReco(candCfg)

    accMS.merge(recoMS, sequenceName=stepEFMSReco.getName())

    # TODO remove once full step is in place
    HLTTest__TestHypoTool=CompFactory.HLTTest.TestHypoTool
    fakeHypoAlg = fakeHypoAlgCfg(muonflags, name='FakeHypoForMuon')
    def makeFakeHypoTool(chainDict, cfg=None):
        return HLTTest__TestHypoTool(chainDict['chainName'])

    accMS.addEventAlgo(fakeHypoAlg, sequenceName=stepEFMSView.getName())

    efmuMSSequence = CAMenuSequence( Sequence = recoMS.sequence(),
                                     Maker = recoMS.inputMaker(),
                                     Hypo = fakeHypoAlg, 
                                     HypoToolGen = makeFakeHypoTool,
                                     CA = accMS )

    efmuMSSequence.createHypoTools(chainDict)

    efmuMSStep = ChainStep( stepEFMSName, [efmuMSSequence] )

    l1Thresholds=[]
    for part in chainDict['chainParts']:
        l1Thresholds.append(part['L1threshold'])

    # needed for debugging
    #import pprint
    #pprint.pprint(chainDict)
    chain = Chain( name=chainDict['chainName'], L1Thresholds=l1Thresholds, ChainSteps=[ l2muFastStep, efmuMSStep ] )
    return chain

