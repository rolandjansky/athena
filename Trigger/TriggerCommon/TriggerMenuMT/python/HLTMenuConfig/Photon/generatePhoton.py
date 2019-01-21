# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from TrigUpgradeTest.ElectronMenuConfig import l2CaloRecoCfg, l2CaloHypoCfg
from TrigUpgradeTest.PhotonMenuConfig import l2PhotonRecoCfg, l2PhotonHypoCfg
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, \
    ChainStep, Chain, RecoFragmentsPool, getChainStepName
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromName
from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlgMT
from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2PhotonHypoAlgMT
from TrigEgammaHypo.TrigL2PhotonHypoTool import TrigL2PhotonHypoToolFromName


def generateChains(flags, chainDict):
    acc = ComponentAccumulator()

    # l2CaloHypo = RecoFragmentsPool.retrieve(l2CaloHypoCfg,
    #                                         flags,
    #                                         name='L2PhotonCaloHypo',
    #                                         CaloClusters='L2CaloEMClusters')

    l2CaloHypo = TrigL2CaloHypoAlgMT('L2PhotonCaloHypo')
    l2CaloHypo.CaloClusters = 'L2CaloEMClusters'

    l2CaloHypoTool = TrigL2CaloHypoToolFromName(chainDict['chainName'], chainDict['chainName'])
    l2CaloHypo.HypoTools = [l2CaloHypoTool]

    l2CaloReco = RecoFragmentsPool.retrieve(l2CaloRecoCfg, flags)
    # l2CaloReco.addHypoAlg(l2CaloHypo)
    acc.merge(l2CaloReco)

    fastCaloSequence = MenuSequence( Sequence    = l2CaloReco.sequence(),
                                     Maker       = l2CaloReco.inputMaker(),
                                     Hypo        = l2CaloHypo,
                                     HypoToolGen = None )

    fastCaloStep = ChainStep(getChainStepName('Photon', 1), [fastCaloSequence])

    # l2PhotonHypo = RecoFragmentsPool.retrieve(l2PhotonHypoCfg,
    #                                           flags,
    #                                           Photons='L2Photons',
    #                                           RunInView=True)

    l2PhotonHypo = TrigL2PhotonHypoAlgMT()
    l2PhotonHypo.Photons = 'L2Photons'
    l2PhotonHypo.RunInView = True

    from AthenaCommon.Constants import VERBOSE
    l2PhotonHypo.OutputLevel = VERBOSE

    l2PhotonHypoTool = TrigL2PhotonHypoToolFromName(chainDict['chainName'], chainDict['chainName'])
    l2PhotonHypo.HypoTools = [l2PhotonHypoTool]

    l2PhotonReco = RecoFragmentsPool.retrieve(l2PhotonRecoCfg, flags)
    # l2PhotonReco.addHypoAlg(l2PhotonHypo)
    acc.merge(l2PhotonReco)


    l2PhotonSequence = MenuSequence( Sequence    = l2PhotonReco.sequence(),
                                     Maker       = l2PhotonReco.inputMaker(),
                                     Hypo        = l2PhotonHypo,
                                     HypoToolGen = None )

    l2PhotonStep = ChainStep(getChainStepName('Photon', 2), [l2PhotonSequence])

    import pprint
    pprint.pprint(chainDict)

    chain = Chain(chainDict['chainName'], chainDict['L1item'], [fastCaloStep, l2PhotonStep])
    return acc, chain
