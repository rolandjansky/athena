#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool, MenuSequence
from AthenaCommon.CFElements import seqAND


class CaloMenuDefs(object):
      """Static Class to collect all string manipulations in Calo sequences """
      from TrigEDMConfig.TriggerEDMRun3 import recordable
      L2CaloClusters= recordable("HLT_L2CaloEMClusters")



def fastCaloSequence(doRinger):
    """ Creates Fast Calo sequence"""
    # EV creator
    from TrigT2CaloCommon.CaloDef import fastCaloEVCreator
    (fastCaloViewsMaker, InViewRoIs) = fastCaloEVCreator()

    # reco sequence always build the rings
    from TrigT2CaloCommon.CaloDef import fastCaloRecoSequence
    (fastCaloInViewSequence, sequenceOut) = fastCaloRecoSequence(InViewRoIs, doRinger=doRinger)

     # connect EVC and reco
    fastCaloSequence = seqAND("fastCaloSequence", [fastCaloViewsMaker, fastCaloInViewSequence ])
    return (fastCaloSequence, fastCaloViewsMaker, sequenceOut)


def fastCaloMenuSequence(name, doRinger):
    """ Creates Egamma Fast Calo  MENU sequence
    The Hypo name changes depending on name, so for different implementations (Electron, Gamma,....)
    The doRinger flag is to use or not the Ringer hypo
    """
    (sequence, fastCaloViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(fastCaloSequence, {'doRinger' : doRinger})
    # check if use Ringer and are electron because there aren't ringer for photons yet:
    # hypo
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaFastCaloHypoAlgMT
    theFastCaloHypo = TrigEgammaFastCaloHypoAlgMT(name+"EgammaFastCaloHypo")
    theFastCaloHypo.CaloClusters = sequenceOut
    CaloMenuDefs.L2CaloClusters = sequenceOut

    from TrigEgammaHypo.TrigEgammaFastCaloHypoTool import TrigEgammaFastCaloHypoToolFromDict
    return MenuSequence( Sequence    = sequence,
                         Maker       = fastCaloViewsMaker,
                         Hypo        = theFastCaloHypo,
                         HypoToolGen = TrigEgammaFastCaloHypoToolFromDict )
