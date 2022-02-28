#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from TriggerMenuMT.HLT.Config.MenuComponents import RecoFragmentsPool, MenuSequence
from AthenaCommon.CFElements import seqAND

class CaloMenuDefs_FWD(object):
      """Static Class to collect all string manipulations in Calo sequences """
      from TrigEDMConfig.TriggerEDMRun3 import recordable
      L2CaloClusters= recordable("HLT_FastCaloEMClusters_FWD")


#
# central or forward fast calo sequence 
#
def fastCaloSequence_FWD(flags, name="fastCaloFWDSequence"):

    from TrigT2CaloCommon.CaloDef import fastCaloEVFWDCreator 
    from TrigT2CaloCommon.CaloDef import fastCaloRecoFWDSequence 
    
    (fastCaloViewsMaker, InViewRoIs) = fastCaloEVFWDCreator()
    # reco sequence always build the rings
    (fastCaloInViewSequence, sequenceOut) = fastCaloRecoFWDSequence(InViewRoIs, doRinger=True)
     # connect EVC and reco
    fastCaloSequence = seqAND(name, [fastCaloViewsMaker, fastCaloInViewSequence ])
    return (fastCaloSequence, fastCaloViewsMaker, sequenceOut)



#
# Create e/g fast calo menu sequence for central or forward region.
#
def fastCaloMenuSequence_FWD(flags, name, doRinger=True, is_probe_leg=False):
    """ Creates Egamma Fast Calo FWD MENU sequence (Reco and Hypo)
    The Hypo name changes depending on name, so for different implementations (Electron, Gamma,....)
    """

    (sequence, fastCaloViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(fastCaloSequence_FWD, flags=flags)
    
    # hypo
    from TrigEgammaForwardHypo.TrigEgammaForwardHypoConf import TrigEgammaForwardFastCaloHypoAlgMT
    from TrigEgammaForwardHypo.TrigEgammaForwardFastCaloHypoTool import TrigEgammaForwardFastCaloHypoToolFromDict
    theFastCaloHypo = TrigEgammaForwardFastCaloHypoAlgMT(name+"EgammaFastCaloFWDHypo")
    theFastCaloHypo.CaloClusters = sequenceOut

    from TrigEgammaForwardHypo.TrigEgammaForwardFastCaloHypoTool import createTrigEgammaForwardFastCaloHypoAlgMT
   
    theFastCaloHypo = createTrigEgammaForwardFastCaloHypoAlgMT(name+"EgammaFastCaloFWDHypo", sequenceOut)
    CaloMenuDefs_FWD.L2CaloClusters = sequenceOut

    return MenuSequence( Sequence    = sequence,
                         Maker       = fastCaloViewsMaker,
                         Hypo        = theFastCaloHypo,
                         HypoToolGen = TrigEgammaForwardFastCaloHypoToolFromDict,
                         IsProbe     = is_probe_leg )



