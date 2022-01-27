#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLT.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorPreviousROITool

def tag(ion):
    return 'precision' + ('HI' if ion is True else '') + 'Tracking'


def precisionTrackingSequence(ConfigFlags, ion=False, variant=''):
    """ fourth step:  precision electron....."""

    from TriggerMenuMT.HLT.Egamma.TrigEgammaKeys import getTrigEgammaKeys
    TrigEgammaKeys = getTrigEgammaKeys(variant, ion=ion)
    caloClusters = TrigEgammaKeys.precisionElectronCaloClusterContainer
 

    InViewRoIs = "precisionTracking" + variant
    # EVCreator:
    precisionTrackingViewsMaker = EventViewCreatorAlgorithm("IM" + tag(ion) + variant)
    precisionTrackingViewsMaker.RoIsLink = "initialRoI" # Merge inputs based on their initial L1 ROI
    precisionTrackingViewsMaker.RoITool = ViewCreatorPreviousROITool()
    precisionTrackingViewsMaker.InViewRoIs = InViewRoIs
    precisionTrackingViewsMaker.Views = tag(ion) + "Views" + variant
    precisionTrackingViewsMaker.ViewFallThrough = True
    precisionTrackingViewsMaker.RequireParentView = True
    
    # calling precision tracking
    from TriggerMenuMT.HLT.Electron.PrecisionTrackingRecoSequences import precisionTracking
    precisionTrackInViewSequence, trackParticles = precisionTracking(InViewRoIs, ion, variant)

    precisionTrackingInViewAlgs = parOR(tag(ion) + "InViewAlgs" + variant, [precisionTrackInViewSequence])
    precisionTrackingViewsMaker.ViewNodeName = tag(ion) + "InViewAlgs" + variant

    # connect EVC and reco
    theSequence = seqAND(tag(ion) + "Sequence" + variant, [precisionTrackingViewsMaker, precisionTrackingInViewAlgs] )
    return (theSequence,precisionTrackingViewsMaker,caloClusters,trackParticles)



def precisionTrackingMenuSequence(name, is_probe_leg=False, ion=False, variant=''):
    """ Creates precisionCalo MENU sequence """
    (sequence, precisionTrackingViewsMaker, caloClusters, trackParticles) = RecoFragmentsPool.retrieve(precisionTrackingSequence, ConfigFlags, ion=ion, variant=variant)

    #Hypo
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionTrackingHypoAlg
    from TrigEgammaHypo.TrigEgammaPrecisionTrackingHypoTool import TrigEgammaPrecisionTrackingHypoToolFromDict
    thePrecisionTrackingHypo = TrigEgammaPrecisionTrackingHypoAlg(name + tag(ion) + "Hypo" + variant)
    thePrecisionTrackingHypo.CaloClusters = caloClusters

    return MenuSequence( Sequence    = sequence,
                         Maker       = precisionTrackingViewsMaker, 
                         Hypo        = thePrecisionTrackingHypo,
                         HypoToolGen = TrigEgammaPrecisionTrackingHypoToolFromDict,
                         IsProbe     = is_probe_leg)


def precisionTrackingMenuSequence_LRT(name, is_probe_leg=False):
    return precisionTrackingMenuSequence(name, is_probe_leg=is_probe_leg, ion=False, variant='_LRT')


