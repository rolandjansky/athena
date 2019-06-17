# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Electron.ElectronRecoSequences import l2CaloRecoCfg, l2CaloHypoCfg
from TriggerMenuMT.HLTMenuConfig.Photon.PhotonRecoSequences import l2PhotonRecoCfg, l2PhotonHypoCfg
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, \
    ChainStep, Chain, getChainStepName, createStepView
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromDict
from TrigEgammaHypo.TrigL2PhotonHypoTool import TrigL2PhotonHypoToolFromDict
from TrigEDMConfig.TriggerEDMRun3 import recordable

def generateChains(flags, chainDict):

    firstStepName = getChainStepName('Photon', 1)
    stepReco, stepView = createStepView(firstStepName)

    accCalo = ComponentAccumulator()
    accCalo.addSequence(stepView)

    l2CaloReco = l2CaloRecoCfg(flags)
    accCalo.merge(l2CaloReco, sequenceName=stepReco.getName())

    l2CaloHypo = l2CaloHypoCfg( flags,
                                name = 'L2PhotonCaloHypo',
                                CaloClusters = recordable('HLT_L2CaloEMClusters') )

    l2CaloHypo.HypoTools = [ TrigL2CaloHypoToolFromDict(chainDict) ]

    accCalo.addEventAlgo(l2CaloHypo, sequenceName=stepView.getName())

    fastCaloSequence = MenuSequence( Sequence = l2CaloReco.sequence(),
                                     Maker = l2CaloReco.inputMaker(),
                                     Hypo = l2CaloHypo,
                                     HypoToolGen = None,
                                     CA = accCalo )

    fastCaloStep = ChainStep(firstStepName, [fastCaloSequence])


    secondStepName = getChainStepName('Photon', 2)
    stepReco, stepView = createStepView(secondStepName)

    accPhoton = ComponentAccumulator()
    accPhoton.addSequence(stepView)

    l2PhotonReco = l2PhotonRecoCfg(flags)
    accPhoton.merge(l2PhotonReco, sequenceName=stepReco.getName())

    l2PhotonHypo = l2PhotonHypoCfg( flags,
                                    Photons = 'HLT_L2Photons',
                                    RunInView = True )

    l2PhotonHypo.HypoTools = [ TrigL2PhotonHypoToolFromDict(chainDict) ]

    accPhoton.addEventAlgo(l2PhotonHypo, sequenceName=stepView.getName())

    l2PhotonSequence = MenuSequence( Sequence = l2PhotonReco.sequence(),
                                     Maker = l2PhotonReco.inputMaker(),
                                     Hypo = l2PhotonHypo,
                                     HypoToolGen = None,
                                     CA = accPhoton )

    l2PhotonStep = ChainStep(secondStepName, [l2PhotonSequence])

    import pprint
    pprint.pprint(chainDict)

    chain = Chain(chainDict['chainName'], chainDict['L1item'], [fastCaloStep, l2PhotonStep])
    return chain
