#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorPreviousROITool


def precisionElectronSequence_LRT(ConfigFlags):
    """ fifth step:  precision electron....."""
    InViewRoIs = "precisionElectron_LRT"
    # EVCreator:
    precisionElectronViewsMaker = EventViewCreatorAlgorithm("IMprecisionElectron_LRT")
    precisionElectronViewsMaker.RoIsLink = "initialRoI"
    precisionElectronViewsMaker.RoITool = ViewCreatorPreviousROITool()
    precisionElectronViewsMaker.InViewRoIs = InViewRoIs
    precisionElectronViewsMaker.Views = "precisionElectronViews_LRT" #precisionElectronViews
    precisionElectronViewsMaker.ViewFallThrough = True
    precisionElectronViewsMaker.RequireParentView = True

    # Configure the reconstruction algorithm sequence

    from TriggerMenuMT.HLTMenuConfig.Electron.PrecisionElectronRecoSequences_LRT import precisionElectronRecoSequence_LRT
    (electronPrecisionRec, sequenceOut, sequenceOut_dummy) = precisionElectronRecoSequence_LRT(InViewRoIs)

    electronPrecisionInViewAlgs = parOR("electronPrecisionInViewAlgs_LRT", [electronPrecisionRec])
    precisionElectronViewsMaker.ViewNodeName = "electronPrecisionInViewAlgs_LRT"

    electronPrecisionAthSequence = seqAND("electronPrecisionAthSequence_LRT", [precisionElectronViewsMaker, electronPrecisionInViewAlgs ] )
    return (electronPrecisionAthSequence, precisionElectronViewsMaker, sequenceOut, sequenceOut_dummy)


def precisionElectronMenuSequence_LRT(is_probe_leg=False, do_idperf=False):
    # retrieve the reco seuqence+EVC
    (electronPrecisionAthSequence, precisionElectronViewsMaker, sequenceOut, sequenceOut_dummy) = RecoFragmentsPool.retrieve(precisionElectronSequence_LRT, ConfigFlags)

    # make the Hypo
    from TrigEgammaHypo.TrigEgammaPrecisionElectronHypoTool import createTrigEgammaPrecisionElectronHypoAlg
    if do_idperf:
        thePrecisionElectronHypo = createTrigEgammaPrecisionElectronHypoAlg("TrigEgammaPrecisionElectronHypoAlg_LRT_idperf", sequenceOut_dummy, do_idperf)
    else:
        thePrecisionElectronHypo = createTrigEgammaPrecisionElectronHypoAlg("TrigEgammaPrecisionElectronHypoAlg_LRT", sequenceOut, do_idperf)
    
    from TrigEgammaHypo.TrigEgammaPrecisionElectronHypoTool import TrigEgammaPrecisionElectronHypoToolFromDict
    
    return  MenuSequence( Maker       = precisionElectronViewsMaker,
                          Sequence    = electronPrecisionAthSequence,
                          Hypo        = thePrecisionElectronHypo,
                          HypoToolGen = TrigEgammaPrecisionElectronHypoToolFromDict,
                          IsProbe     = is_probe_leg )


