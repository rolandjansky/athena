# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, ChainStep, Chain, RecoFragmentsPool, getChainStepName
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from TrigUpgradeTest.MuonMenuConfig import l2MuFastRecoCfg, l2MuFastHypoCfg
from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMufastHypoToolFromName


def generateChains( flags, chainDict ):

    acc = ComponentAccumulator()

    ### Set muon step1 ###
    l2muFastHypo = RecoFragmentsPool.retrieve( l2MuFastHypoCfg,
                                               flags,
                                               name = "TrigL2MuFastHypo",
                                               muFastInfo = "MuonL2SAInfo" )

    l2muFastReco = RecoFragmentsPool.retrieve( l2MuFastRecoCfg, flags )
    acc.merge( l2muFastReco )

    l2muFastSequence = MenuSequence( Sequence = l2muFastReco.sequence(),
                                     Maker = l2muFastReco.inputMaker(),
                                     Hypo = l2muFastHypo, 
                                     HypoToolGen = TrigMufastHypoToolFromName )

    l2muFastStep = ChainStep( getChainStepName('Muon', 1), [l2muFastSequence] )

    ### Set muon step2 ###
    # Please set up L2muComb step here 
   

    import pprint
    pprint.pprint(chainDict)

    chain = Chain( chainDict['chainName'], chainDict['L1item'], [ l2muFastStep ] )

    return acc, chain
