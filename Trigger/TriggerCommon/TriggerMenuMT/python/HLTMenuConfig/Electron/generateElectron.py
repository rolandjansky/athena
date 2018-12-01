# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from TrigUpgradeTest.ElectronMenuConfig import l2CaloRecoCfg, l2CaloHypoCfg
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, ChainStep, Chain, RecoFragmentsPool

from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromName
from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlgMT
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def generateChains( flags,  chainDict ):
    import pprint
    pprint.pprint( chainDict )

    acc = ComponentAccumulator()

    l2CaloHypo = RecoFragmentsPool.retrieve(l2CaloHypoCfg,
                                            flags,
                                            name = 'L2ElectronCaloHypo',
                                            CaloClusters ='L2CaloEMClusters')

    l2CaloReco = RecoFragmentsPool.retrieve(l2CaloRecoCfg, flags)
    acc.merge(l2CaloReco)

    fastCaloSequence = MenuSequence( Sequence    = l2CaloReco.sequence(),
                                     Maker       = l2CaloReco.inputMaker(),
                                     Hypo        = l2CaloHypo,
                                     HypoToolGen = TrigL2CaloHypoToolFromName )

    fastCaloStep = ChainStep("Electron_step1", [fastCaloSequence])

           
    # # # fast ID 
    
    # # # EF calo

    # # # EF ID
    
    # # # offline egamma

    chain = Chain(chainDict['chainName'], chainDict['L1item'], [fastCaloStep] )
    return acc, chain
