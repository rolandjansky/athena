# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from TrigUpgradeTest.ElectronMenuConfig import l2CaloRecoCfg, l2CaloHypoCfg, l2CaloAlgCfg
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, \
    ChainStep, Chain, RecoFragmentsPool, getChainStepName

from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromName
from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlgMT
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

# TODO remove once full tracking is in place
def fakeHypoAlgCfg(flags, name="FakeHypoForElectron"):
    from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoAlg
    return HLTTest__TestHypoAlg( name, Input="" )


def generateChains( flags,  chainDict ):
    import pprint
    pprint.pprint( chainDict )

    acc = ComponentAccumulator()

    l2CaloHypo = TrigL2CaloHypoAlgMT('L2ElectronCaloHypo')
    l2CaloHypo.CaloClusters = 'L2CaloEMClusters'

    l2CaloHypoTool = TrigL2CaloHypoToolFromName(chainDict['chainName'], chainDict['chainName'])
    l2CaloHypo.HypoTools = [l2CaloHypoTool]

    # l2CaloHypo = RecoFragmentsPool.retrieve(l2CaloHypoCfg,
    #                                         flags,
    #                                         name = 'L2ElectronCaloHypo',
    #                                         CaloClusters = 'L2CaloEMClusters' )


    # from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import InViewReco
    # l2CaloReco = InViewReco("FastCaloEMReco")
    # algAcc, alg = l2CaloAlgCfg(flags, roisKey=l2CaloReco.name + 'RoIs')
    # l2CaloReco.addRecoAlg(alg)
    # l2CaloReco.merge(algAcc)

    l2CaloReco = RecoFragmentsPool.retrieve(l2CaloRecoCfg, flags)
    # l2CaloReco.addHypoAlg(l2CaloHypo)
    acc.merge(l2CaloReco)

    # acc.addEventAlgo(l2CaloHypo)

    fastCaloSequence = MenuSequence( Sequence    = l2CaloReco.sequence(),
                                     Maker       = l2CaloReco.inputMaker(),
                                     Hypo        = l2CaloHypo,
                                     HypoToolGen = None )

    fastCaloStep = ChainStep(getChainStepName('Electron', 1), [fastCaloSequence])


           
    # # # fast ID
    from TrigUpgradeTest.InDetConfig import indetInViewRecoCfg
    fastInDetReco = RecoFragmentsPool.retrieve( indetInViewRecoCfg,
                                                flags,
                                                viewMakerName="ElectronInDet" )
    acc.merge( fastInDetReco )
    # TODO once tracking fully works remove fake hypos

    from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoTool

    # TODO remove once full tracking is in place    
    fakeHypoAlg = RecoFragmentsPool.retrieve( fakeHypoAlgCfg, flags, name="FakeHypoForElectron" )

    
    def makeFakeHypoTool(name, cfg):
        return HLTTest__TestHypoTool(name)
    
    fastInDetSequence = MenuSequence( Sequence    = fastInDetReco.sequence(),
                                      Maker       = fastInDetReco.inputMaker(),
                                      Hypo        = fakeHypoAlg,
                                      HypoToolGen = makeFakeHypoTool )
    
    fastInDetStep = ChainStep( getChainStepName( "Electron", 2 ), [fastInDetSequence] )

    
    # # # EF calo

    # # # EF ID
    
    # # # offline egamma

    chain = Chain( chainDict['chainName'], chainDict['L1item'], [fastCaloStep, fastInDetStep] )
    return acc, chain
