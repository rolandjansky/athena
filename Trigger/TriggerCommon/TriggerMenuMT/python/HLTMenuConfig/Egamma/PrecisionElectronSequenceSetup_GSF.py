#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorPreviousROITool


def precisionElectronSequence_GSF(ConfigFlags):
    """ 
    Similar setup as ../PrecisionElectronSequenceSetup.py; tailored for GSF chains
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
    from TriggerMenuMT.HLTMenuConfig.Electron.PrecisionElectronRecoSequences_GSF import precisionElectronRecoSequence_GSF
    (electronPrecisionRec, sequenceOut) = precisionElectronRecoSequence_GSF(InViewRoIs)

    electronPrecisionInViewAlgs = parOR("electronPrecisionInViewAlgs_GSF", [electronPrecisionRec])
    precisionElectronViewsMaker.ViewNodeName = "electronPrecisionInViewAlgs_GSF"

    electronPrecisionAthSequence = seqAND("electronPrecisionAthSequence_GSF", [precisionElectronViewsMaker, electronPrecisionInViewAlgs ] )
    return (electronPrecisionAthSequence, precisionElectronViewsMaker, sequenceOut)


def precisionElectronMenuSequence_GSF():
    # retrieve the reco seuqence+EVC
    (electronPrecisionAthSequence, precisionElectronViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(precisionElectronSequence_GSF, ConfigFlags)

    # make the Hypo
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionElectronHypoAlgMT
    thePrecisionElectronHypo = TrigEgammaPrecisionElectronHypoAlgMT("TrigEgammaPrecisionElectronHypoAlgMT_GSF")
    thePrecisionElectronHypo.Electrons = sequenceOut
    thePrecisionElectronHypo.RunInView = True

    from TrigEgammaHypo.TrigEgammaPrecisionElectronHypoTool import TrigEgammaPrecisionElectronHypoToolFromDict

    return  MenuSequence( Maker       = precisionElectronViewsMaker,
                          Sequence    = electronPrecisionAthSequence,
                          Hypo        = thePrecisionElectronHypo,
                          HypoToolGen = TrigEgammaPrecisionElectronHypoToolFromDict )



