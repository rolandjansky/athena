#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

### Output Name ###
muFastInfo = "MuonL2SAInfo"

def generateMuonsCfg( flags ):

    acc = ComponentAccumulator()
    
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, ChainStep, Chain, RecoFragmentsPool

    # Set Hypo alg of muFast step
    from TrigMuonHypoMT.TrigMuonHypoMTConf import TrigMufastHypoAlg
    from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMufastHypoToolFromName
    muFastHypo = TrigMufastHypoAlg("TrigL2MuFastHypo")
    muFastHypo.MuonL2SAInfoFromMuFastAlg = muFastInfo

    # Set Reco alg of muFast step
    from TrigL2MuonSA.TrigL2MuonSAConfig_newJO import l2MuFastRecoCfg
    l2MuFastReco = RecoFragmentsPool.retrieve( l2MuFastRecoCfg, flags )
    acc.merge( l2MuFastReco )

    # Set muFast step
    muFastSeq = MenuSequence( Sequence    = l2MuFastReco.sequence(),
                              Maker       = l2MuFastReco.inputMaker(),
                              Hypo        = muFastHypo,
                              HypoToolGen = TrigMufastHypoToolFromName )

    muFastStep = ChainStep("Muon_muFastStep1", [muFastSeq])

    chains = [ Chain(c.split()[0], c.split()[1], [muFastStep]) for c in flags.Trigger.menu.muon ]

    return acc, chains

