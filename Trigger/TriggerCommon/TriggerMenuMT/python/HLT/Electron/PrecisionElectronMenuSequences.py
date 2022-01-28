#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLT.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorPreviousROITool
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable, appendCAtoAthena
from AthenaCommon.Configurable import ConfigurableRun3Behavior

def tag(ion):
    return 'precision' + ('HI' if ion is True else '') + 'Electron'


def precisionElectronSequence(ConfigFlags, ion=False, variant=''):
    """ fifth step:  precision electron....."""
    InViewRoIs = "electronPrecision"
    # EVCreator:
    precisionElectronViewsMaker = EventViewCreatorAlgorithm("IM" + tag(ion) + variant)
    precisionElectronViewsMaker.RoIsLink = "initialRoI"
    precisionElectronViewsMaker.RoITool = ViewCreatorPreviousROITool()
    precisionElectronViewsMaker.InViewRoIs = InViewRoIs
    precisionElectronViewsMaker.Views = tag(ion) + "Views" + variant #precisionElectronViews
    precisionElectronViewsMaker.ViewFallThrough = True
    precisionElectronViewsMaker.RequireParentView = True

    # Configure the reconstruction algorithm sequence
    from TriggerMenuMT.HLT.Electron.PrecisionElectronRecoSequences import precisionElectronRecoSequence
    (electronPrecisionRec, sequenceOut, sequenceOut_dummy) = precisionElectronRecoSequence(InViewRoIs, ion, variant)

    # Suffix to distinguish probe leg sequences
    electronPrecisionInViewAlgs = parOR(tag(ion) + "InViewAlgs" + variant, [electronPrecisionRec])
    precisionElectronViewsMaker.ViewNodeName = tag(ion) + "InViewAlgs" + variant

    electronPrecisionAthSequence = seqAND(tag(ion) + "AthSequence" + variant, [precisionElectronViewsMaker, electronPrecisionInViewAlgs ] )
    return (electronPrecisionAthSequence, precisionElectronViewsMaker, sequenceOut, sequenceOut_dummy)


def precisionElectronMenuSequence(is_probe_leg=False, ion=False,  variant=''):
    # retrieve the reco seuqence+EVC
    (electronPrecisionAthSequence, precisionElectronViewsMaker, sequenceOut, sequenceOut_dummy) = RecoFragmentsPool.retrieve(precisionElectronSequence, ConfigFlags, ion=ion, variant=variant)

    # make the Hypo
    from TrigEgammaHypo.TrigEgammaPrecisionElectronHypoTool import createTrigEgammaPrecisionElectronHypoAlg
    with ConfigurableRun3Behavior():
       hypo_tuple = createTrigEgammaPrecisionElectronHypoAlg("TrigEgamma" + tag(ion) + "HypoAlg_noGSF"+ variant, sequenceOut)
    thePrecisionElectronHypo = conf2toConfigurable(hypo_tuple[0])
    hypo_acc = hypo_tuple[1]
    appendCAtoAthena( hypo_acc )
    
    from TrigEgammaHypo.TrigEgammaPrecisionElectronHypoTool import TrigEgammaPrecisionElectronHypoToolFromDict
    
    return  MenuSequence( Maker       = precisionElectronViewsMaker,
                          Sequence    = electronPrecisionAthSequence,
                          Hypo        = thePrecisionElectronHypo,
                          HypoToolGen = TrigEgammaPrecisionElectronHypoToolFromDict,
                          IsProbe     = is_probe_leg)

def precisionElectronMenuSequence_LRT(is_probe_leg=False ):
    return precisionElectronMenuSequence(is_probe_leg=is_probe_leg, ion=False, variant='_LRT')
