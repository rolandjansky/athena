# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, ChainStep, Chain, getChainStepName, createStepView
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from TrigL2MuonSA.TrigL2MuonSAConfig_newJO import l2MuFastRecoCfg, l2MuFastHypoCfg
from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMufastHypoToolFromDict


def fakeHypoAlgCfg(flags, name="FakeHypoForMuon"):
    from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoAlg
    return HLTTest__TestHypoAlg( name, Input="" )

def generateChains( flags, chainDict ):
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

    l2muFastHypo.HypoTools = [ TrigMufastHypoToolFromDict(chainDict) ]

    acc.addEventAlgo(l2muFastHypo, sequenceName=stepView.getName())

    l2muFastSequence = MenuSequence( Sequence = l2muFastReco.sequence(),
                                     Maker = l2muFastReco.inputMaker(),
                                     Hypo = l2muFastHypo,
                                     HypoToolGen = None,
                                     CA = acc )

    l2muFastStep = ChainStep( stepName, [l2muFastSequence] )

    ### Set muon step2 ###
    # Please set up L2muComb step here

    #EF MS only
    stepEFMSName = getChainStepName('EFMSMuon', 2)
    stepEFMSReco, stepEFMSView = createStepView(stepEFMSName)

    accMS = ComponentAccumulator()
    accMS.addSequence(stepEFMSView)

    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import InViewReco
    recoMS = InViewReco("EFMuMSReco")

    from MuonConfig.MuonSegmentFindingConfig import MooSegmentFinderAlgCfg
    segCfg = MooSegmentFinderAlgCfg(flags,name="TrigMooSegmentFinder", TgcPrepDataContainer="TGC_Measurements")
    recoMS.mergeReco(segCfg)

    accMS.merge(recoMS, sequenceName=stepEFMSReco.getName())

    # TODO remove once full step is in place
    from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoTool
    fakeHypoAlg = fakeHypoAlgCfg(flags, name='FakeHypoForMuon')
    def makeFakeHypoTool(name, cfg):
        return HLTTest__TestHypoTool(name)
    fakeHypoAlg.HypoTools = [ makeFakeHypoTool(chainDict['chainName'], None) ]
    accMS.addEventAlgo(fakeHypoAlg, sequenceName=stepEFMSView.getName())

    efmuMSSequence = MenuSequence( Sequence = recoMS.sequence(),
                                     Maker = recoMS.inputMaker(),
                                     Hypo = fakeHypoAlg, 
                                     HypoToolGen = None,
                                     CA = accMS )

    efmuMSStep = ChainStep( stepEFMSName, [efmuMSSequence] )

    l1Thresholds=[]
    for part in chainDict['chainParts']:
        l1Thresholds.append(part['L1threshold'])
    import pprint
    pprint.pprint(chainDict)
    chain = Chain( name=chainDict['chainName'], L1Thresholds=l1Thresholds, ChainSteps=[ l2muFastStep, efmuMSStep ] )
    return chain

