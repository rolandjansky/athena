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
    """ Creates L2 Fast Calo sequence"""
    # EV creator
    from TrigT2CaloCommon.CaloDef import fastCaloEVCreator
    (fastCaloViewsMaker, InViewRoIs) = fastCaloEVCreator()

    # reco sequence
    from TrigT2CaloCommon.CaloDef import fastCaloRecoSequence
    (fastCaloInViewSequence, sequenceOut) = fastCaloRecoSequence(InViewRoIs, doRinger=doRinger)

     # connect EVC and reco
    fastCaloSequence = seqAND("fastCaloSequence", [fastCaloViewsMaker, fastCaloInViewSequence ])
    return (fastCaloSequence, fastCaloViewsMaker, sequenceOut)


def fastCaloMenuSequence(name, doRinger=True):
    """ Creates L2 Fast Calo  MENU sequence
    The Hypo name changes depending on name, so for different implementations (Electron, Gamma,....)
    """
    (sequence, fastCaloViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(fastCaloSequence, doRinger)

    # hypo    
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlgMT
    theFastCaloHypo = TrigL2CaloHypoAlgMT(name+"L2CaloHypo")
    theFastCaloHypo.CaloClusters = sequenceOut
    CaloMenuDefs.L2CaloClusters = sequenceOut

 
    from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromDict
    return MenuSequence( Sequence    = sequence,
                         Maker       = fastCaloViewsMaker, 
                         Hypo        = theFastCaloHypo,
                         HypoToolGen = TrigL2CaloHypoToolFromDict )
