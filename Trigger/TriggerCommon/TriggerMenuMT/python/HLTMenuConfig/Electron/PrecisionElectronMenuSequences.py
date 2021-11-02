#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorPreviousROITool
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable, appendCAtoAthena
from AthenaCommon.Configurable import ConfigurableRun3Behavior

def tag(ion):
    return 'precision' + ('HI' if ion is True else '') + 'Electron'


def precisionElectronSequence(ConfigFlags, ion=False):
    """ fifth step:  precision electron....."""
    InViewRoIs = "electronPrecision"
    # EVCreator:
    precisionElectronViewsMaker = EventViewCreatorAlgorithm("IM" + tag(ion))
    precisionElectronViewsMaker.RoIsLink = "initialRoI"
    precisionElectronViewsMaker.RoITool = ViewCreatorPreviousROITool()
    precisionElectronViewsMaker.InViewRoIs = InViewRoIs
    precisionElectronViewsMaker.Views = tag(ion) + "Views" #precisionElectronViews
    precisionElectronViewsMaker.ViewFallThrough = True
    precisionElectronViewsMaker.RequireParentView = True

    # Configure the reconstruction algorithm sequence
    from TriggerMenuMT.HLTMenuConfig.Electron.PrecisionElectronRecoSequences import precisionElectronRecoSequence
    (electronPrecisionRec, sequenceOut, sequenceOut_dummy) = precisionElectronRecoSequence(InViewRoIs, ion)

    # Suffix to distinguish probe leg sequences
    electronPrecisionInViewAlgs = parOR(tag(ion) + "InViewAlgs", [electronPrecisionRec])
    precisionElectronViewsMaker.ViewNodeName = tag(ion) + "InViewAlgs"

    electronPrecisionAthSequence = seqAND(tag(ion) + "AthSequence", [precisionElectronViewsMaker, electronPrecisionInViewAlgs ] )
    return (electronPrecisionAthSequence, precisionElectronViewsMaker, sequenceOut, sequenceOut_dummy)


def precisionElectronMenuSequence(is_probe_leg=False, ion=False, do_idperf=False):
    # retrieve the reco seuqence+EVC
    (electronPrecisionAthSequence, precisionElectronViewsMaker, sequenceOut, sequenceOut_dummy) = RecoFragmentsPool.retrieve(precisionElectronSequence, ConfigFlags, ion=ion)

    # make the Hypo
    from TrigEgammaHypo.TrigEgammaPrecisionElectronHypoTool import createTrigEgammaPrecisionElectronHypoAlg

    if do_idperf:
        with ConfigurableRun3Behavior():
           hypo_tuple = createTrigEgammaPrecisionElectronHypoAlg("TrigEgamma" + tag(ion) + "HypoAlg_noGSF_idperf", sequenceOut_dummy, do_idperf)
    else:
        with ConfigurableRun3Behavior():
           hypo_tuple = createTrigEgammaPrecisionElectronHypoAlg("TrigEgamma" + tag(ion) + "HypoAlg_noGSF", sequenceOut, do_idperf)

    thePrecisionElectronHypo = conf2toConfigurable(hypo_tuple[0])
    hypo_acc = hypo_tuple[1]
    appendCAtoAthena( hypo_acc )
    
    from TrigEgammaHypo.TrigEgammaPrecisionElectronHypoTool import TrigEgammaPrecisionElectronHypoToolFromDict
    
    return  MenuSequence( Maker       = precisionElectronViewsMaker,
                          Sequence    = electronPrecisionAthSequence,
                          Hypo        = thePrecisionElectronHypo,
                          HypoToolGen = TrigEgammaPrecisionElectronHypoToolFromDict,
                          IsProbe     = is_probe_leg)



