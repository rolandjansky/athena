#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaConfiguration.AllConfigFlags import ConfigFlags
    
def metCellMenuSequence():
    # menu components   
    # retrieve the reco seuqnece
    from TriggerMenuMT.HLTMenuConfig.MET.METSequences import metCellAthSequence
    (metCellSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(metCellAthSequence,ConfigFlags)
  
    #hypo
    from TrigMissingETHypo.TrigMissingETHypoConfigMT import MissingETHypoAlgMT, TrigMETCellHypoToolFromDict

    metHypoAlg = MissingETHypoAlgMT("METHypoAlg_cell")
    metHypoAlg.METContainerKey=sequenceOut

    return  MenuSequence( Sequence    = metCellSequence,
                          Maker       = InputMakerAlg,
                          Hypo        = metHypoAlg,
                          HypoToolGen = TrigMETCellHypoToolFromDict )

    
def metClusterMenuSequence():
    # menu components   
    # retrieve the reco seuqnece
    from TriggerMenuMT.HLTMenuConfig.MET.METSequences import metClusterAthSequence
    (metClusterSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(metClusterAthSequence,ConfigFlags)
  
    #hypo
    from TrigMissingETHypo.TrigMissingETHypoConfigMT import MissingETHypoAlgMT, TrigMETCellHypoToolFromDict

    metHypoAlg = MissingETHypoAlgMT("METHypoAlg_tc")
    metHypoAlg.METContainerKey=sequenceOut

    return  MenuSequence( Sequence    = metClusterSequence,
                          Maker       = InputMakerAlg,
                          Hypo        = metHypoAlg,
                          HypoToolGen = TrigMETCellHypoToolFromDict )


def metClusterPufitMenuSequence():
    # menu components   
    # retrieve the reco seuqnece
    from TriggerMenuMT.HLTMenuConfig.MET.METSequences import metClusterPufitAthSequence
    (metClusterPufitSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(metClusterPufitAthSequence,ConfigFlags)
  
    #hypo
    from TrigMissingETHypo.TrigMissingETHypoConfigMT import MissingETHypoAlgMT, TrigMETCellHypoToolFromDict

    metHypoAlg = MissingETHypoAlgMT("METHypoAlg_tcpufit")
    metHypoAlg.METContainerKey=sequenceOut

    return  MenuSequence( Sequence    = metClusterPufitSequence,
                          Maker       = InputMakerAlg,
                          Hypo        = metHypoAlg,
                          HypoToolGen = TrigMETCellHypoToolFromDict )



#def metJetMenuSequence():

    # menu components   
    # retrieve the reco seuqnece
#    from TriggerMenuMT.HLTMenuConfig.MET.METSequences import metJetAthSequence
#    (metJetSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(metJetAthSequence,ConfigFlags)
  
    #hypo
#    from TrigMissingETHypo.TrigMissingETHypoConfigMT import MissingETHypoAlgMT, TrigMETCellHypoToolFromDict

#    metHypoAlg = MissingETHypoAlgMT("METHypoAlg_mht")
#    metHypoAlg.METContainerKey=sequenceOut

#    return  MenuSequence( Sequence    = metJetSequence,
#                          Maker       = InputMakerAlg,
#                          Hypo        = metHypoAlg,
#                          HypoToolGen = TrigMETCellHypoToolFromDict )
