#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from AthenaConfiguration.AllConfigFlags import ConfigFlags
      
def precisionCaloSequence(ConfigFlags):
    """ Creates PrecisionCalo sequence """
    # EV creator
    InViewRoIs="PrecisionCaloRoIs"     
    precisionCaloViewsMaker = EventViewCreatorAlgorithm( "precisionCaloViewsMaker")
    precisionCaloViewsMaker.ViewFallThrough = True
    precisionCaloViewsMaker.RoIsLink = "initialRoI"
    precisionCaloViewsMaker.InViewRoIs = InViewRoIs
    precisionCaloViewsMaker.Views = "precisionCaloViews"
    
    # reco sequence
    from TrigUpgradeTest.precisionCaloRec import precisionCaloRecoSequence
    (precisionCaloInViewSequence, sequenceOut) = precisionCaloRecoSequence(InViewRoIs)

    precisionCaloViewsMaker.ViewNodeName = precisionCaloInViewSequence.name()
    
    # connect EVC and reco
    precisionCaloSequence = seqAND("precisionCaloSequence", [precisionCaloViewsMaker, precisionCaloInViewSequence] )
    return (precisionCaloSequence, precisionCaloViewsMaker, sequenceOut)

def precisionCaloMenuSequence():
    """ Creates precisionCalo MENU sequence """
    (sequence, precisionCaloViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(precisionCaloSequence, ConfigFlags)

    #Hypo
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionCaloHypoAlgMT
    from TrigEgammaHypo.TrigEgammaPrecisionCaloHypoTool import TrigEgammaPrecisionCaloHypoToolFromDict

    thePrecisionCaloHypo = TrigEgammaPrecisionCaloHypoAlgMT("precisionCaloHypo")
    thePrecisionCaloHypo.CaloClusters = sequenceOut

    return MenuSequence( Sequence    = sequence,
                         Maker       = precisionCaloViewsMaker, 
                         Hypo        = thePrecisionCaloHypo,
                         HypoToolGen = TrigEgammaPrecisionCaloHypoToolFromDict)
