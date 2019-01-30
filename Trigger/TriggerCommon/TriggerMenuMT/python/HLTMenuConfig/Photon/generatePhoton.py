# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from TrigUpgradeTest.ElectronMenuConfig import l2CaloRecoCfg, l2CaloHypoCfg
from TrigUpgradeTest.PhotonMenuConfig import l2PhotonRecoCfg, l2PhotonHypoCfg
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, \
    ChainStep, Chain, RecoFragmentsPool, getChainStepName
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromDict
from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlgMT
from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2PhotonHypoAlgMT
from TrigEgammaHypo.TrigL2PhotonHypoTool import TrigL2PhotonHypoToolFromDict


def generateChains(flags, chainDict):
    accCalo = ComponentAccumulator()


    l2CaloHypo = l2CaloHypoCfg( flags,
                                name = 'L2PhotonCaloHypo',
                                CaloClusters = 'L2CaloEMClusters' )

    l2CaloHypo.HypoTools = [ TrigL2CaloHypoToolFromDict(chainDict) ]

    l2CaloReco = RecoFragmentsPool.retrieve(l2CaloRecoCfg, flags)
    accCalo.merge(l2CaloReco)

    fastCaloSequence = MenuSequence( Sequence = l2CaloReco.sequence(),
                                     Maker = l2CaloReco.inputMaker(),
                                     Hypo = l2CaloHypo,
                                     HypoToolGen = None,
                                     CA = accCalo )

    # del accCalo

    fastCaloStep = ChainStep(getChainStepName('Photon', 1), [fastCaloSequence])

    accPhoton = ComponentAccumulator()

    l2PhotonHypo = l2PhotonHypoCfg( flags,
                                    Photons = 'L2Photons',
                                    RunInView = True )

    l2PhotonHypo.HypoTools = [ TrigL2PhotonHypoToolFromDict(chainDict) ]

    l2PhotonReco = RecoFragmentsPool.retrieve(l2PhotonRecoCfg, flags)
    accPhoton.merge(l2PhotonReco)


    l2PhotonSequence = MenuSequence( Sequence = l2PhotonReco.sequence(),
                                     Maker = l2PhotonReco.inputMaker(),
                                     Hypo = l2PhotonHypo,
                                     HypoToolGen = None,
                                     CA = accPhoton )

    # del accPhoton

    l2PhotonStep = ChainStep(getChainStepName('Photon', 2), [l2PhotonSequence])

    import pprint
    pprint.pprint(chainDict)

    chain = Chain(chainDict['chainName'], chainDict['L1item'], [fastCaloStep, l2PhotonStep])
    return chain
