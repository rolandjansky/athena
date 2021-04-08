#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorPreviousROITool


def precisionElectronSequence(ConfigFlags):
    """ fifth step:  precision electron....."""
    InViewRoIs = "precisionElectron"
    # EVCreator:
    precisionElectronViewsMaker = EventViewCreatorAlgorithm("IMprecisionElectron")
    precisionElectronViewsMaker.RoIsLink = "initialRoI"
    precisionElectronViewsMaker.RoITool = ViewCreatorPreviousROITool()
    precisionElectronViewsMaker.InViewRoIs = InViewRoIs
    precisionElectronViewsMaker.Views = "precisionElectronViews" #precisionElectronViews
    precisionElectronViewsMaker.ViewFallThrough = True
    precisionElectronViewsMaker.RequireParentView = True

    # Configure the reconstruction algorithm sequence
    from TriggerMenuMT.HLTMenuConfig.Electron.PrecisionElectronRecoSequences import precisionElectronRecoSequence
    (electronPrecisionRec, sequenceOut) = precisionElectronRecoSequence(InViewRoIs)

    electronPrecisionInViewAlgs = parOR("electronPrecisionInViewAlgs", [electronPrecisionRec])
    precisionElectronViewsMaker.ViewNodeName = "electronPrecisionInViewAlgs"

    electronPrecisionAthSequence = seqAND("electronPrecisionAthSequence", [precisionElectronViewsMaker, electronPrecisionInViewAlgs ] )
    return (electronPrecisionAthSequence, precisionElectronViewsMaker, sequenceOut)


def precisionElectronMenuSequence():
    # retrieve the reco seuqence+EVC
    (electronPrecisionAthSequence, precisionElectronViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(precisionElectronSequence, ConfigFlags)

    # make the Hypo
    from TriggerMenuMT.HLTMenuConfig.Egamma.EgammaDefs import TrigElectronSelectors
    SelectorTool_vloose, SelectorTool_loose, SelectorTool_medium, SelectorTool_tight = TrigElectronSelectors()
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionElectronHypoAlgMT
    thePrecisionElectronHypo = TrigEgammaPrecisionElectronHypoAlgMT("TrigEgammaPrecisionElectronHypoAlgMT_noGSF")
    thePrecisionElectronHypo.Electrons = sequenceOut
    thePrecisionElectronHypo.RunInView = True
    thePrecisionElectronHypo.ElectronLHSelector_vLoose = SelectorTool_vloose
    thePrecisionElectronHypo.ElectronLHSelector_Loose = SelectorTool_loose
    thePrecisionElectronHypo.ElectronLHSelector_Medium = SelectorTool_medium
    thePrecisionElectronHypo.ElectronLHSelector_Tight = SelectorTool_tight

    from TrigEgammaHypo.TrigEgammaPrecisionElectronHypoTool import TrigEgammaPrecisionElectronHypoToolFromDict

    return  MenuSequence( Maker       = precisionElectronViewsMaker,
                          Sequence    = electronPrecisionAthSequence,
                          Hypo        = thePrecisionElectronHypo,
                          HypoToolGen = TrigEgammaPrecisionElectronHypoToolFromDict )



