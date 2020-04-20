# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Electron.ElectronRecoSequences import l2CaloRecoCfg, l2CaloHypoCfg
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import CAMenuSequence, \
    ChainStep, Chain, getChainStepName, createStepView

from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromDict
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TrigEDMConfig.TriggerEDMRun3 import recordable
from AthenaConfiguration.ComponentFactory import CompFactory

# TODO remove once full tracking is in place
def fakeHypoAlgCfg(flags, name="FakeHypoForElectron"):    
    return CompFactory.getComp("HLTTest::TestHypoAlg")( name, Input="" )


def generateChains( flags,  chainDict ):
    import pprint
    pprint.pprint( chainDict )

    firstStepName = getChainStepName('Electron', 1)
    stepReco, stepView = createStepView(firstStepName)

    accCalo = ComponentAccumulator()
    accCalo.addSequence(stepView)

    l2CaloReco = l2CaloRecoCfg(flags)
    accCalo.merge(l2CaloReco, sequenceName=stepReco.getName())

    l2CaloHypo =  l2CaloHypoCfg( flags, name = 'L2ElectronCaloHypo',
                                 CaloClusters = recordable('HLT_L2CaloEMClusters'))

    accCalo.addEventAlgo(l2CaloHypo, sequenceName=stepView.getName())

    fastCaloSequence = CAMenuSequence( Sequence    = l2CaloReco.sequence(),
                                     Maker       = l2CaloReco.inputMaker(),
                                     Hypo        = l2CaloHypo,
                                     HypoToolGen = TrigL2CaloHypoToolFromDict, 
                                     CA = accCalo)

    fastCaloSequence.createHypoTools(chainDict)

    accCalo.printConfig()

    fastCaloStep = ChainStep(firstStepName, [fastCaloSequence])


    secondStepName = getChainStepName('Electron', 2)
    stepReco, stepView = createStepView(secondStepName)

    accTrk = ComponentAccumulator()
    accTrk.addSequence(stepView)

    # # # fast ID
    from TrigInDetConfig.TrigInDetConfig import indetInViewRecoCfg
    fastInDetReco = indetInViewRecoCfg(flags, viewMakerName='ElectronInDet', signature='Electron')
    accTrk.merge( fastInDetReco, sequenceName=stepReco.getName() )
    # TODO once tracking fully works remove fake hypos


    # TODO remove once full tracking is in place
    fakeHypoAlg = fakeHypoAlgCfg(flags, name='FakeHypoForElectron')


    def makeFakeHypoTool(chainDict, cfg = None):
        return CompFactory.getComp("HLTTest::TestHypoTool")(chainDict['chainName'])

    accTrk.addEventAlgo(fakeHypoAlg, sequenceName=stepView.getName())

    fastInDetSequence = CAMenuSequence( Sequence    = fastInDetReco.sequence(),
                                      Maker       = fastInDetReco.inputMaker(),
                                      Hypo        = fakeHypoAlg,
                                      HypoToolGen = makeFakeHypoTool,
                                      CA = accTrk)

    fastInDetSequence.createHypoTools(chainDict)

    fastInDetStep = ChainStep( secondStepName, [fastInDetSequence] )

    l1Thresholds=[]
    for part in chainDict['chainParts']:
        l1Thresholds.append(part['L1threshold'])
    
    # # # EF calo

    # # # EF ID
    
    # # # offline egamma

    chain = Chain( chainDict['chainName'], L1Thresholds=l1Thresholds, ChainSteps=[fastCaloStep, fastInDetStep] )
    
    return chain
