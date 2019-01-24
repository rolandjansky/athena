from AthenaCommon.Constants import ERROR, DEBUG
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from AthenaCommon.CFElements import seqAND


class CaloMenuDefs():
      """Static Class to collect all string manipulations in Calo sequences """
      L2CaloClusters= ""




def fastCaloMenuSequence():
    """ Creates L2 Fast Calo menu sequence"""
    # EV creator
    from TrigT2CaloCommon.CaloDef import fastCaloEVCreator
    (fastCaloViewsMaker, InViewRoIs) = fastCaloEVCreator()

    # reco sequence
    from TrigT2CaloCommon.CaloDef import fastCaloRecoSequence
    (fastCaloInViewSequence, sequenceOut) = fastCaloRecoSequence(InViewRoIs)

     # connect EVC and reco
    fastCaloSequence = seqAND("fastCaloSequence", [fastCaloViewsMaker, fastCaloInViewSequence ])
    
    # hypo    
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlgMT
    theFastCaloHypo = TrigL2CaloHypoAlgMT("L2CaloHypo", OutputLevel = DEBUG)
    theFastCaloHypo.CaloClusters = sequenceOut
    CaloMenuDefs.L2CaloClusters = sequenceOut

 
    from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromName
    return MenuSequence( Sequence    = fastCaloSequence,
                         Maker       = fastCaloViewsMaker, 
                         Hypo        = theFastCaloHypo,
                         HypoToolGen = TrigL2CaloHypoToolFromName )
