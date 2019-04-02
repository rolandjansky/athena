#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
from AthenaCommon.Constants import VERBOSE,DEBUG

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from AthenaCommon.CFElements import seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
      
def precisionCaloMenuSequence():
    InViewRoIs="PrecisionCaloRoIs"     
    precisionCaloViewsMaker = EventViewCreatorAlgorithm( "precisionCaloViewsMaker", OutputLevel=DEBUG)
    precisionCaloViewsMaker.ViewFallThrough = True
    precisionCaloViewsMaker.RoIsLink = "initialRoI"
    precisionCaloViewsMaker.InViewRoIs = InViewRoIs
    precisionCaloViewsMaker.Views = "precisionCaloViews"

    from TrigUpgradeTest.precisionCaloRec import precisionCaloRecoSequence
    (precisionSequence, sequenceOut) = precisionCaloRecoSequence(InViewRoIs)

    precisionCaloViewsMaker.ViewNodeName = precisionSequence.name()

#Hypo
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionCaloHypoAlgMT
    from TrigEgammaHypo.TrigEgammaPrecisionCaloHypoTool import TrigEgammaPrecisionCaloHypoToolFromDict

    thePrecisionCaloHypo = TrigEgammaPrecisionCaloHypoAlgMT("precisionCaloHypo")
    thePrecisionCaloHypo.CaloClusters = sequenceOut
    thePrecisionCaloHypo.OutputLevel = VERBOSE

    precisionAthSequence = seqAND("precisionAthSequence", [precisionCaloViewsMaker, precisionSequence] )
    return MenuSequence( Sequence    = precisionAthSequence,
                         Maker       = precisionCaloViewsMaker, 
                         Hypo        = thePrecisionCaloHypo,
                         HypoToolGen = TrigEgammaPrecisionCaloHypoToolFromDict)
