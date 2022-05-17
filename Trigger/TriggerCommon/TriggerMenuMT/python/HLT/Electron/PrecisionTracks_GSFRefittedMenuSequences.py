#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorPreviousROITool

def tag(ion):
    return 'precision' + ('HI' if ion is True else '') + 'Tracking_GSFRefitted'

def precisionTracks_GSFRefittedSequence(ConfigFlags, ion=False, variant=''):
    """ fifth step:  GSF refitting of precision track....."""

    from TriggerMenuMT.HLT.Egamma.TrigEgammaKeys import getTrigEgammaKeys
    TrigEgammaKeys = getTrigEgammaKeys(variant, ion=ion)
    caloClusters = TrigEgammaKeys.precisionElectronCaloClusterContainer
 

    InViewRoIs = "precisionTracks_GSFRefitted"+variant
    # EVCreator:
    precisionTracks_GSFRefittedViewsMaker = EventViewCreatorAlgorithm("IM" + tag(ion) + variant)
    precisionTracks_GSFRefittedViewsMaker.mergeUsingFeature=True
    precisionTracks_GSFRefittedViewsMaker.RoITool = ViewCreatorPreviousROITool()
    precisionTracks_GSFRefittedViewsMaker.InViewRoIs = InViewRoIs
    precisionTracks_GSFRefittedViewsMaker.Views = tag(ion) + "Views" + variant
    precisionTracks_GSFRefittedViewsMaker.ViewFallThrough = True
    precisionTracks_GSFRefittedViewsMaker.RequireParentView = True
    
    # calling precision tracking
    from TriggerMenuMT.HLT.Electron.PrecisionTracks_GSFRefittedSequence import precisionTracks_GSFRefitted
    precisionTracks_GSFRefittedInViewSequence, trackParticles = precisionTracks_GSFRefitted(InViewRoIs, ion, variant)

    precisionTracks_GSFRefittedInViewAlgs = parOR(tag(ion) + "InViewAlgs" + variant, [precisionTracks_GSFRefittedInViewSequence])
    precisionTracks_GSFRefittedViewsMaker.ViewNodeName = tag(ion) + "InViewAlgs" + variant

    # connect EVC and reco
    theSequence = seqAND(tag(ion) + "Sequence" + variant, [precisionTracks_GSFRefittedViewsMaker, precisionTracks_GSFRefittedInViewAlgs] )
    return (theSequence,precisionTracks_GSFRefittedViewsMaker,caloClusters,trackParticles)

def precisionTracks_GSFRefittedMenuSequence(name, is_probe_leg=False, ion=False, variant='_GSF'):
    """ Creates precisionCalo MENU sequence """
    (sequence, precisionTracks_GSFRefittedViewsMaker,caloClusters,trackParticles) = RecoFragmentsPool.retrieve(precisionTracks_GSFRefittedSequence, ConfigFlags, ion=ion, variant=variant)

    #Hypo
    from TrigStreamerHypo.TrigStreamerHypoConf import TrigStreamerHypoAlg, TrigStreamerHypoTool
    thePrecisionTrack_GSFRefittedHypo = TrigStreamerHypoAlg(name + tag(ion) + "Hypo" + variant)
    thePrecisionTrack_GSFRefittedHypo.FeatureIsROI = False
    def acceptAllHypoToolGen(chainDict):
        return TrigStreamerHypoTool(chainDict["chainName"], Pass = True)

    return MenuSequence( Sequence    = sequence,
                         Maker       = precisionTracks_GSFRefittedViewsMaker, 
                         Hypo        = thePrecisionTrack_GSFRefittedHypo,
                         HypoToolGen = acceptAllHypoToolGen, # Note: TrigEgammaPrecisionTrackingHypoAlg does not use HypoTools
                         IsProbe     = is_probe_leg)


def precisionTracks_GSFRefittedMenuSequence_LRT(name, is_probe_leg=False):
    return precisionTracks_GSFRefittedMenuSequence(name, is_probe_leg=is_probe_leg, ion=False, variant='_LRTGSF')

