#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm


def precisionElectronSequence(ConfigFlags):
    """ fourth step:  precision tracking....."""    
    InViewRoIs = "precisionElectron"
    # EVCreator:
    precisionTrackViewsMaker = EventViewCreatorAlgorithm("IMprecisionTrack")
    precisionTrackViewsMaker.RoIsLink = "initialRoI" # -||-
    precisionTrackViewsMaker.InViewRoIs = InViewRoIs # contract with the precisionCalo
    precisionTrackViewsMaker.Views = "precisionElectronViews" #precisionTrackViews
    precisionTrackViewsMaker.ViewFallThrough = True
    precisionTrackViewsMaker.RequireParentView = True

    # Configure the reconstruction algorithm sequence
    from TriggerMenuMT.HLTMenuConfig.Electron.PrecisionElectronRecoSequences import precisionElectronRecoSequence
    (electronPrecisionRec, electronPrecisionTrack, sequenceOut) = precisionElectronRecoSequence(InViewRoIs)

    electronPrecisionInViewAlgs = parOR("electronPrecisionInViewAlgs", [electronPrecisionTrack, electronPrecisionRec])
    precisionTrackViewsMaker.ViewNodeName = "electronPrecisionInViewAlgs"

    electronPrecisionAthSequence = seqAND("electronPrecisionAthSequence", [precisionTrackViewsMaker, electronPrecisionInViewAlgs ] )
    return (electronPrecisionAthSequence, precisionTrackViewsMaker, sequenceOut)


def precisionElectronMenuSequence():
    # retrieve the reco seuqence+EVC
    (electronPrecisionAthSequence, precisionTrackViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(precisionElectronSequence, ConfigFlags)

    # make the Hypo
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionElectronHypoAlgMT
    thePrecisionElectronHypo = TrigEgammaPrecisionElectronHypoAlgMT("TrigEgammaPrecisionElectronHypoAlgMT")
    thePrecisionElectronHypo.Electrons = sequenceOut
    thePrecisionElectronHypo.RunInView = True

    # from TrigEgammaHypo.TrigL2ElectronHypoTool import TrigL2ElectronHypoToolFromDict
    from TrigEgammaHypo.TrigEgammaPrecisionElectronHypoTool import TrigEgammaPrecisionElectronHypoToolFromDict

    return  MenuSequence( Maker       = precisionTrackViewsMaker,                                        
                          Sequence    = electronPrecisionAthSequence,
                          Hypo        = thePrecisionElectronHypo,
                          HypoToolGen = TrigEgammaPrecisionElectronHypoToolFromDict )

