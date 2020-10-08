#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaCommon.Constants import DEBUG

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorInitialROITool


def precisionEtcutSequence(ConfigFlags):
    """ fourth step:  precision electron....."""
    InViewRoIs = "precisionEtcut"
    # EVCreator:
    precisionEtcutViewsMaker = EventViewCreatorAlgorithm("IMprecisionEtcut")
    precisionEtcutViewsMaker.RoIsLink = "initialRoI"
    precisionEtcutViewsMaker.RoITool = ViewCreatorInitialROITool()
    precisionEtcutViewsMaker.InViewRoIs = InViewRoIs
    precisionEtcutViewsMaker.Views = "precisionEtcutViews"
    precisionEtcutViewsMaker.ViewFallThrough = True
    precisionEtcutViewsMaker.RequireParentView = True

    from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloSequenceSetup import precisionCaloMenuDefs
    
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
    (sequence, precisionEtcutViewsMaker, caloclusters, trackparticles) = RecoFragmentsPool.retrieve(precisionEtcutSequence, ConfigFlags)

    #Hypo
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionEtcutHypoAlgMT
    from TrigEgammaHypo.TrigEgammaPrecisionEtcutHypoTool import TrigEgammaPrecisionEtcutHypoToolFromDict

    thePrecisionEtcutHypo = TrigEgammaPrecisionEtcutHypoAlgMT(name+"precisionEtcutHypo")
    thePrecisionEtcutHypo.CaloClusters = caloclusters
    thePrecisionEtcutHypo.OutputLevel = DEBUG

    return MenuSequence( Sequence    = sequence,
                         Maker       = precisionEtcutViewsMaker, 
                         Hypo        = thePrecisionEtcutHypo,
                         HypoToolGen = TrigEgammaPrecisionEtcutHypoToolFromDict)

