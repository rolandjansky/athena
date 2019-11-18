#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaConfiguration.AllConfigFlags import ConfigFlags

from TrigMissingETHypo.TrigMissingETHypoConfigMT import TrigMETCellHypoToolFromDict
from TrigMissingETHypo.TrigMissingETHypoConf import TrigMissingETHypoAlgMT

    
def metCellMenuSequence():
    # menu components   
    # retrieve the reco seuqnece
    from TriggerMenuMT.HLTMenuConfig.MET.METRecoSequences import metCellAthSequence
    (metCellSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(metCellAthSequence,ConfigFlags)
  
    #hypo
    metHypoAlg = TrigMissingETHypoAlgMT("METHypoAlg_cell")
    metHypoAlg.METContainerKey=sequenceOut

    return  MenuSequence( Sequence    = metCellSequence,
                          Maker       = InputMakerAlg,
                          Hypo        = metHypoAlg,
                          HypoToolGen = TrigMETCellHypoToolFromDict )

    
def metClusterMenuSequence():
    # menu components   
    # retrieve the reco seuqnece
    from TriggerMenuMT.HLTMenuConfig.MET.METRecoSequences import metClusterAthSequence
    (metClusterSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(metClusterAthSequence,ConfigFlags)
  
    #hypo
    metHypoAlg = TrigMissingETHypoAlgMT("METHypoAlg_tc")
    metHypoAlg.METContainerKey=sequenceOut

    return  MenuSequence( Sequence    = metClusterSequence,
                          Maker       = InputMakerAlg,
                          Hypo        = metHypoAlg,
                          HypoToolGen = TrigMETCellHypoToolFromDict )


def metClusterPufitMenuSequence():
    # menu components   
    # retrieve the reco seuqnece
    from TriggerMenuMT.HLTMenuConfig.MET.METRecoSequences import metClusterPufitAthSequence
    (metClusterPufitSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(metClusterPufitAthSequence,ConfigFlags)
  
    #hypo
    metHypoAlg = TrigMissingETHypoAlgMT("METHypoAlg_tcpufit")
    metHypoAlg.METContainerKey=sequenceOut

    return  MenuSequence( Sequence    = metClusterPufitSequence,
                          Maker       = InputMakerAlg,
                          Hypo        = metHypoAlg,
                          HypoToolGen = TrigMETCellHypoToolFromDict )



def metJetMenuSequence():

    # menu components   
    # retrieve the reco seuqnece
    from TriggerMenuMT.HLTMenuConfig.MET.METRecoSequences import metJetAthSequence
    (metJetSequence, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(metJetAthSequence,ConfigFlags)

    #hypo
    metHypoAlg = TrigMissingETHypoAlgMT("METHypoAlg_mht")
    metHypoAlg.METContainerKey=sequenceOut

    return  MenuSequence( Sequence    = metJetSequence,
                          Maker       = InputMakerAlg,
                          Hypo        = metHypoAlg,
                          HypoToolGen = TrigMETCellHypoToolFromDict )

def metTrkMHTMenuSequence():
    from TriggerMenuMT.HLTMenuConfig.MET.METRecoSequences import metTrkMHTAthSequence
    reco_seq, input_alg, seq_out = RecoFragmentsPool.retrieve(
            metTrkMHTAthSequence, ConfigFlags)

    # The hypo
    hypo = TrigMissingETHypoAlgMT("METHypoAlg_trkmht")
    hypo.METContainerKey = seq_out

    # NB - the function is called 'TrigMETCellHypoToolFromDict' but it isn't
    # actually specific to cell at all... something to change in the future
    return MenuSequence( Sequence    = reco_seq,
                         Maker       = input_alg,
                         Hypo        = hypo,
                         HypoToolGen = TrigMETCellHypoToolFromDict )
