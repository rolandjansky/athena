#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorPreviousROITool


def precisionTracks_GSFRefittedSequence(ConfigFlags):
    """ fifth step:  GSF refitting of precision track....."""

    from TriggerMenuMT.HLT.Egamma.TrigEgammaKeys import getTrigEgammaKeys
    TrigEgammaKeys = getTrigEgammaKeys('_GSF')
    caloClusters = TrigEgammaKeys.precisionElectronCaloClusterContainer
 

    InViewRoIs = "precisionTracks_GSFRefitted"
    # EVCreator:
    precisionTracks_GSFRefittedViewsMaker = EventViewCreatorAlgorithm("IMPrecisionTracking_GSFRefitted")
    precisionTracks_GSFRefittedViewsMaker.mergeUsingFeature=True
    precisionTracks_GSFRefittedViewsMaker.RoITool = ViewCreatorPreviousROITool()
    precisionTracks_GSFRefittedViewsMaker.InViewRoIs = InViewRoIs
    precisionTracks_GSFRefittedViewsMaker.Views = "precisionTracks_GSFRefittedViews"
    precisionTracks_GSFRefittedViewsMaker.ViewFallThrough = True
    precisionTracks_GSFRefittedViewsMaker.RequireParentView = True
    
    # calling precision tracking
    from TriggerMenuMT.HLT.Electron.PrecisionTracks_GSFRefittedSequence import precisionTracks_GSFRefitted
    precisionTracks_GSFRefittedInViewSequence, trackParticles = precisionTracks_GSFRefitted(InViewRoIs)

    precisionTracks_GSFRefittedInViewAlgs = parOR("precisionTracking_GSFRefittedInViewAlgs", [precisionTracks_GSFRefittedInViewSequence])
    precisionTracks_GSFRefittedViewsMaker.ViewNodeName = "precisionTracking_GSFRefittedInViewAlgs"

    # connect EVC and reco
    theSequence = seqAND("precisionTracks_GSFRefittedSequence", [precisionTracks_GSFRefittedViewsMaker, precisionTracks_GSFRefittedInViewAlgs] )
    return (theSequence,precisionTracks_GSFRefittedViewsMaker,caloClusters,trackParticles)



def precisionTracks_GSFRefittedMenuSequence(name, is_probe_leg=False):
    """ Creates precisionCalo MENU sequence """
    (sequence, precisionTracks_GSFRefittedViewsMaker,caloClusters,trackParticles) = RecoFragmentsPool.retrieve(precisionTracks_GSFRefittedSequence, ConfigFlags)

    #Hypo
    from TrigStreamerHypo.TrigStreamerHypoConf import TrigStreamerHypoAlg, TrigStreamerHypoTool
    thePrecisionTrack_GSFRefittedHypo = TrigStreamerHypoAlg("precisionTracks_GSFRefittedHypo")
    thePrecisionTrack_GSFRefittedHypo.FeatureIsROI = False
    def acceptAllHypoToolGen(chainDict):
        return TrigStreamerHypoTool(chainDict["chainName"], Pass = True)

    return MenuSequence( Sequence    = sequence,
                         Maker       = precisionTracks_GSFRefittedViewsMaker, 
                         Hypo        = thePrecisionTrack_GSFRefittedHypo,
                         HypoToolGen = acceptAllHypoToolGen, # Note: TrigEgammaPrecisionTrackingHypoAlg does not use HypoTools
                         IsProbe     = is_probe_leg)


