#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorPreviousROITool
from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloMenuSequences import precisionCaloMenuDefs

def tag(ion):
    return 'precision' + ('HI' if ion is True else '') + 'Tracking'


def precisionTrackingSequence(ConfigFlags, ion=False):
    """ fourth step:  precision electron....."""
    InViewRoIs = "precisionTracking"
    # EVCreator:
    precisionTrackingViewsMaker = EventViewCreatorAlgorithm("IM" + tag(ion))
    precisionTrackingViewsMaker.RoIsLink = "initialRoI" # Merge inputs based on their initial L1 ROI
    precisionTrackingViewsMaker.RoITool = ViewCreatorPreviousROITool()
    precisionTrackingViewsMaker.InViewRoIs = InViewRoIs
    precisionTrackingViewsMaker.Views = tag(ion) + "Views"
    precisionTrackingViewsMaker.ViewFallThrough = True
    precisionTrackingViewsMaker.RequireParentView = True
    
    # calling precision tracking
    from TriggerMenuMT.HLTMenuConfig.Electron.PrecisionTrackingSequences import precisionTracking
    precisionTrackInViewSequence, trackparticles = precisionTracking(InViewRoIs, ion)

    precisionTrackingInViewAlgs = parOR(tag(ion) + "InViewAlgs", [precisionTrackInViewSequence])
    precisionTrackingViewsMaker.ViewNodeName = tag(ion) + "InViewAlgs"

    # connect EVC and reco
    theSequence = seqAND(tag(ion) + "Sequence", [precisionTrackingViewsMaker, precisionTrackingInViewAlgs] )
    return (theSequence,precisionTrackingViewsMaker,precisionCaloMenuDefs.caloClusters(ion),trackparticles)


def precisionTrackingMenuSequence(name, is_probe_leg=False, ion=False):
    """ Creates precisionCalo MENU sequence """
    (sequence, precisionTrackingViewsMaker, caloclusters, trackparticles) = RecoFragmentsPool.retrieve(precisionTrackingSequence, ConfigFlags, ion=ion)

    #Hypo
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionTrackingHypoAlg

    from TrigEgammaHypo.TrigEgammaPrecisionTrackingHypoTool import TrigEgammaPrecisionTrackingHypoToolFromDict

    thePrecisionTrackingHypo = TrigEgammaPrecisionTrackingHypoAlg(name + tag(ion) + "Hypo")
    thePrecisionTrackingHypo.CaloClusters = precisionCaloMenuDefs.caloClusters(ion)

    return MenuSequence( Sequence    = sequence,
                         Maker       = precisionTrackingViewsMaker, 
                         Hypo        = thePrecisionTrackingHypo,
                         HypoToolGen = TrigEgammaPrecisionTrackingHypoToolFromDict,
                         IsProbe     = is_probe_leg)


