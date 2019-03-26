#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Constants import VERBOSE,DEBUG,INFO,WARNING
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import seqAND
from AthenaConfiguration.AllConfigFlags import ConfigFlags
    
def metCellMenuSequence():
    # menu components   
    # retrieve the reco seuqnece
    from TrigUpgradeTest.metDefs import metCellAthSequence
    (metCellSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(metCellAthSequence,ConfigFlags)
  
    #hypo
    from TrigMissingETHypo.TrigMissingETHypoConfigMT import MissingETHypoAlgMT, TrigMETCellHypoToolFromDict, MissingETHypoToolMT

    metHypoAlg = MissingETHypoAlgMT("METHypoAlg")
    metHypoAlg.METContainerKey=sequenceOut
    metHypoAlg.OutputLevel=DEBUG

    return  MenuSequence( Sequence    = metCellSequence,
                          Maker       = InputMakerAlg,
                          Hypo        = metHypoAlg,
                          HypoToolGen = TrigMETCellHypoToolFromDict )

    
def metClusterMenuSequence():
    # menu components   
    # retrieve the reco seuqnece
    from TrigUpgradeTest.metDefs import metClusterAthSequence
    (metClusterSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(metClusterAthSequence,ConfigFlags)
  
    #hypo
    from TrigMissingETHypo.TrigMissingETHypoConfigMT import MissingETHypoAlgMT, TrigMETCellHypoToolFromDict, MissingETHypoToolMT

    metHypoAlg = MissingETHypoAlgMT("METHypoAlg")
    metHypoAlg.METContainerKey=sequenceOut
    metHypoAlg.OutputLevel=DEBUG

    return  MenuSequence( Sequence    = metClusterSequence,
                          Maker       = InputMakerAlg,
                          Hypo        = metHypoAlg,
                          HypoToolGen = TrigMETCellHypoToolFromDict )
def metJetMenuSequence():
    # menu components   
    # retrieve the reco seuqnece
    from TrigUpgradeTest.metDefs import metJetAthSequence
    (metJetSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(metJetAthSequence,ConfigFlags)
  
    #hypo
    from TrigMissingETHypo.TrigMissingETHypoConfigMT import MissingETHypoAlgMT, TrigMETJetHypoToolFromName, MissingETHypoToolMT

    metHypoAlg = MissingETHypoAlgMT("METHypoAlg")
    metHypoAlg.METContainerKey=sequenceOut
    metHypoAlg.OutputLevel=DEBUG

    return  MenuSequence( Sequence    = metJetSequence,
                          Maker       = InputMakerAlg,
                          Hypo        = metHypoAlg,
                          HypoToolGen = TrigMETJetHypoToolFromName )
