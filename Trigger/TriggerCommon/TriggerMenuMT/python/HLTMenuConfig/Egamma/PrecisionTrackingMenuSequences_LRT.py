#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorPreviousROITool


def precisionTrackSequence_LRT(ConfigFlags):
    """ fourth step:  precision electron....."""
    InViewRoIs = "precisionTrack_LRT"
    # EVCreator:
    precisionTrackViewsMaker = EventViewCreatorAlgorithm("IMprecisionTrack_LRT")
    precisionTrackViewsMaker.RoIsLink = "initialRoI" # Merge inputs based on their initial L1 ROI
    precisionTrackViewsMaker.RoITool = ViewCreatorPreviousROITool()
    precisionTrackViewsMaker.InViewRoIs = InViewRoIs
    precisionTrackViewsMaker.Views = "precisionTrackViews_LRT"
    precisionTrackViewsMaker.ViewFallThrough = True
    precisionTrackViewsMaker.RequireParentView = True

    from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloMenuSequences_LRT import precisionCaloMenuDefs_LRT
    
    # calling precision tracking
    from TriggerMenuMT.HLTMenuConfig.Electron.PrecisionTrackSequences_LRT import precisionTracking_LRT
    precisionTrackInViewSequence, trackparticles = precisionTracking_LRT(InViewRoIs, precisionCaloMenuDefs_LRT.precisionCaloClusters)
    
    precisionTrackInViewAlgs = parOR("precisionTrackInViewAlgs_LRT", [precisionTrackInViewSequence])
    precisionTrackViewsMaker.ViewNodeName = "precisionTrackInViewAlgs_LRT"    

    # connect EVC and reco
    theSequence = seqAND("precisionTrackSequence_LRT", [precisionTrackViewsMaker, precisionTrackInViewAlgs] )
    return (theSequence,precisionTrackViewsMaker,precisionCaloMenuDefs_LRT.precisionCaloClusters,trackparticles)

def precisionTrackingMenuSequence_LRT(name):
    """ Creates precisionCalo MENU sequence """
    (sequence, precisionTrackViewsMaker, caloclusters, trackparticles) = RecoFragmentsPool.retrieve(precisionTrackSequence_LRT, ConfigFlags)

    #Hypo
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionEtcutHypoAlgMT
    from TrigEgammaHypo.TrigEgammaPrecisionEtcutHypoTool import TrigEgammaPrecisionEtcutHypoToolFromDict

    thePrecisionEtcutHypo = TrigEgammaPrecisionEtcutHypoAlgMT(name+"precisionEtcutHypo_LRT")

    return MenuSequence( Sequence    = sequence,
                         Maker       = precisionTrackViewsMaker, 
                         Hypo        = thePrecisionEtcutHypo,
                         HypoToolGen = TrigEgammaPrecisionEtcutHypoToolFromDict)

