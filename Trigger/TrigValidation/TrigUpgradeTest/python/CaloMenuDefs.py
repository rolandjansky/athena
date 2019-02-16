from AthenaCommon.Constants import ERROR, DEBUG
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool, MenuSequence
from AthenaCommon.CFElements import seqAND
from AthenaConfiguration.AllConfigFlags import ConfigFlags


class CaloMenuDefs():
      """Static Class to collect all string manipulations in Calo sequences """
      L2CaloClusters= ""



def fastCaloSequence(ConfigFlags):
    """ Creates L2 Fast Calo sequence"""
    # EV creator
    from TrigT2CaloCommon.CaloDef import fastCaloEVCreator
    (fastCaloViewsMaker, InViewRoIs) = fastCaloEVCreator()

    # reco sequence
    from TrigT2CaloCommon.CaloDef import fastCaloRecoSequence
    (fastCaloInViewSequence, sequenceOut) = fastCaloRecoSequence(InViewRoIs)

     # connect EVC and reco
    fastCaloSequence = seqAND("fastCaloSequence", [fastCaloViewsMaker, fastCaloInViewSequence ])
    return (fastCaloSequence, fastCaloViewsMaker, sequenceOut)


def fastCaloMenuSequence(name):
    """ Creates L2 Fast Calo  MENU sequence
    The Hypo name changes depending on name, so for different implementations (Electron, Gamma,....)
    """
    (sequence, fastCaloViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(fastCaloSequence, ConfigFlags)

    # hypo    
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlgMT
    theFastCaloHypo = TrigL2CaloHypoAlgMT(name+"L2CaloHypo", OutputLevel = DEBUG)
    theFastCaloHypo.CaloClusters = sequenceOut
    CaloMenuDefs.L2CaloClusters = sequenceOut

 
    from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromName
    return MenuSequence( Sequence    = sequence,
                         Maker       = fastCaloViewsMaker, 
                         Hypo        = theFastCaloHypo,
                         HypoToolGen = TrigL2CaloHypoToolFromName )
