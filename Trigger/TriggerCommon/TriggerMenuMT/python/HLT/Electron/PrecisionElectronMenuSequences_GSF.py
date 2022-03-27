#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorPreviousROITool
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable, appendCAtoAthena
from AthenaCommon.Configurable import ConfigurableRun3Behavior

def precisionElectronSequence_GSF(ConfigFlags):
    """ 
    Similar setup as ../PrecisionElectronMenuSequences.py; tailored for GSF chains
    """
    InViewRoIs = "precisionElectron"
    # EVCreator:
    precisionElectronViewsMaker = EventViewCreatorAlgorithm("IMprecisionElectron_GSF")
    precisionElectronViewsMaker.RoIsLink = "initialRoI"
    precisionElectronViewsMaker.RoITool = ViewCreatorPreviousROITool()
    precisionElectronViewsMaker.InViewRoIs = InViewRoIs
    precisionElectronViewsMaker.Views = "precisionElectronViews_GSF" #precisionElectronViews_GSF
    precisionElectronViewsMaker.ViewFallThrough = True
    precisionElectronViewsMaker.RequireParentView = True

    # Configure the reconstruction algorithm sequence
    from TriggerMenuMT.HLT.Electron.PrecisionElectronRecoSequences_GSF import precisionElectronRecoSequence_GSF
    (electronPrecisionRec, sequenceOut, sequenceOut_dummy) = precisionElectronRecoSequence_GSF(InViewRoIs)

    electronPrecisionInViewAlgs = parOR("electronPrecisionInViewAlgs_GSF", [electronPrecisionRec])
    precisionElectronViewsMaker.ViewNodeName = "electronPrecisionInViewAlgs_GSF"

    electronPrecisionAthSequence = seqAND("electronPrecisionAthSequence_GSF", [precisionElectronViewsMaker, electronPrecisionInViewAlgs ] )
    return (electronPrecisionAthSequence, precisionElectronViewsMaker, sequenceOut, sequenceOut_dummy)


def precisionElectronMenuSequence_GSF(is_probe_leg=False):
    # retrieve the reco seuqence+EVC
    (electronPrecisionAthSequence, precisionElectronViewsMaker, sequenceOut, sequenceOut_dummy) = RecoFragmentsPool.retrieve(precisionElectronSequence_GSF, ConfigFlags)

    # make the Hypo
    from TrigEgammaHypo.TrigEgammaPrecisionElectronHypoTool import createTrigEgammaPrecisionElectronHypoAlg
    with ConfigurableRun3Behavior():
        hypo_tuple = createTrigEgammaPrecisionElectronHypoAlg("TrigEgammaPrecisionElectronHypoAlg_GSF", sequenceOut)

    thePrecisionElectronHypo = conf2toConfigurable(hypo_tuple[0])
    hypo_acc = hypo_tuple[1]
    appendCAtoAthena( hypo_acc )

    from TrigEgammaHypo.TrigEgammaPrecisionElectronHypoTool import TrigEgammaPrecisionElectronHypoToolFromDict

    return  MenuSequence( Maker       = precisionElectronViewsMaker,
                          Sequence    = electronPrecisionAthSequence,
                          Hypo        = thePrecisionElectronHypo,
                          HypoToolGen = TrigEgammaPrecisionElectronHypoToolFromDict,
                          IsProbe     = is_probe_leg )
