#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorPreviousROITool


def tag(ion):
    return 'precision' + ('HI' if ion is True else '') + 'Etcut'


def precisionTrackingSequence(ConfigFlags, ion=False):
    """ fourth step:  precision electron....."""
    InViewRoIs = "precisionEtcut"
    # EVCreator:
    precisionEtcutViewsMaker = EventViewCreatorAlgorithm("IM" + tag(ion))
    precisionEtcutViewsMaker.RoIsLink = "initialRoI" # Merge inputs based on their initial L1 ROI
    precisionEtcutViewsMaker.RoITool = ViewCreatorPreviousROITool()
    precisionEtcutViewsMaker.InViewRoIs = InViewRoIs
    precisionEtcutViewsMaker.Views = tag(ion) + "Views"
    precisionEtcutViewsMaker.ViewFallThrough = True
    precisionEtcutViewsMaker.RequireParentView = True

    from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloMenuSequences import precisionCaloMenuDefs
    
    # calling precision tracking
    from TriggerMenuMT.HLTMenuConfig.Electron.PrecisionTrackingSequences import precisionTracking
    precisionTrackInViewSequence, trackparticles = precisionTracking(InViewRoIs, precisionCaloMenuDefs.caloClusters(ion), ion)

    precisionEtcutInViewAlgs = parOR(tag(ion) + "InViewAlgs", [precisionTrackInViewSequence])
    precisionEtcutViewsMaker.ViewNodeName = tag(ion) + "InViewAlgs"

    # connect EVC and reco
    theSequence = seqAND(tag(ion) + "Sequence", [precisionEtcutViewsMaker, precisionEtcutInViewAlgs] )
    return (theSequence,precisionEtcutViewsMaker,precisionCaloMenuDefs.caloClusters(ion),trackparticles)


def precisionTrackingMenuSequence(name, is_probe_leg=False, ion=False):
    """ Creates precisionCalo MENU sequence """
    (sequence, precisionTrackingViewsMaker, caloclusters, trackparticles) = RecoFragmentsPool.retrieve(precisionTrackingSequence, ConfigFlags, ion=ion)

    #Hypo
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionTrackingHypoAlg
    from TrigEgammaHypo.TrigEgammaPrecisionTrackingHypoTool import TrigEgammaPrecisionTrackingHypoToolFromDict

    thePrecisionTrackingHypo = TrigEgammaPrecisionTrackingHypoAlg(name + tag(ion) + "Hypo")

    return MenuSequence( Sequence    = sequence,
                         Maker       = precisionTrackingViewsMaker, 
                         Hypo        = thePrecisionTrackingHypo,
                         HypoToolGen = TrigEgammaPrecisionTrackingHypoToolFromDict,
                         IsProbe     = is_probe_leg)


