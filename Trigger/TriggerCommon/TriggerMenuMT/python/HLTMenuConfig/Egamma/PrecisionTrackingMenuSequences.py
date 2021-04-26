#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorPreviousROITool


def precisionTrackingSequence(ConfigFlags):
    """ fourth step:  precision electron....."""
    InViewRoIs = "precisionEtcut"
    # EVCreator:
    precisionEtcutViewsMaker = EventViewCreatorAlgorithm("IMprecisionEtcut")
    precisionEtcutViewsMaker.RoIsLink = "initialRoI" # Merge inputs based on their initial L1 ROI
    precisionEtcutViewsMaker.RoITool = ViewCreatorPreviousROITool()
    precisionEtcutViewsMaker.InViewRoIs = InViewRoIs
    precisionEtcutViewsMaker.Views = "precisionEtcutViews"
    precisionEtcutViewsMaker.ViewFallThrough = True
    precisionEtcutViewsMaker.RequireParentView = True

    from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloMenuSequences import precisionCaloMenuDefs
    
    # calling precision tracking
    from TriggerMenuMT.HLTMenuConfig.Electron.PrecisionTrackSequences import precisionTracking
    precisionTrackInViewSequence, trackparticles = precisionTracking(InViewRoIs, precisionCaloMenuDefs.precisionCaloClusters)
    
    precisionEtcutInViewAlgs = parOR("precisionEtcutInViewAlgs", [precisionTrackInViewSequence])
    precisionEtcutViewsMaker.ViewNodeName = "precisionEtcutInViewAlgs"    

    # connect EVC and reco
    theSequence = seqAND("precisionEtcutSequence", [precisionEtcutViewsMaker, precisionEtcutInViewAlgs] )
    return (theSequence,precisionEtcutViewsMaker,precisionCaloMenuDefs.precisionCaloClusters,trackparticles)


def precisionTrackingMenuSequence(name):
    """ Creates precisionCalo MENU sequence """
    (sequence, precisionTrackingViewsMaker, caloclusters, trackparticles) = RecoFragmentsPool.retrieve(precisionTrackingSequence, ConfigFlags)

    #Hypo
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionTrackingHypoAlgMT
    from TrigEgammaHypo.TrigEgammaPrecisionTrackingHypoTool import TrigEgammaPrecisionTrackingHypoToolFromDict

    thePrecisionTrackingHypo = TrigEgammaPrecisionTrackingHypoAlgMT(name+"precisionEtcutHypo")

    return MenuSequence( Sequence    = sequence,
                         Maker       = precisionTrackingViewsMaker, 
                         Hypo        = thePrecisionTrackingHypo,
                         HypoToolGen = TrigEgammaPrecisionTrackingHypoToolFromDict)

