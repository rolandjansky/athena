#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorPreviousROITool
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from TrigEDMConfig.TriggerEDMRun3 import recordable
      
class precisionCaloMenuDefs_FWD(object):
      """Static class to collect all string manipulation in Calo sequences """
      precisionCaloClusters= recordable("HLT_CaloEMClusters_FWD")


def precisionCaloSequence_FWD(ConfigFlags):
    """ Creates PrecisionCalo sequence """
    # EV creator
    InViewRoIs="PrecisionCaloRoIs_FWD"     
    precisionCaloViewsMaker = EventViewCreatorAlgorithm( "IMprecisionCalo_FWD")
    precisionCaloViewsMaker.ViewFallThrough = True
    precisionCaloViewsMaker.RoIsLink = "initialRoI" # Merge inputs based on their initial L1 ROI
    roiTool = ViewCreatorPreviousROITool()
    # Note: This step processes Decision Objects which have followed either Electron reco, Photon reco, or both.
    # For Decision Object which have followed both, there is an ambiguity about which ROI should be used in this
    # merged step. In such cases we break the ambiguity by specifying that the Electron ROI is to be used.
    #roiTool.RoISGKey = "HLT_Roi_FastElectron_FWD"
    precisionCaloViewsMaker.RoITool = roiTool
    precisionCaloViewsMaker.InViewRoIs = InViewRoIs
    precisionCaloViewsMaker.Views = "precisionCaloViews_FWD"
    precisionCaloViewsMaker.RequireParentView = True

    # reco sequence
    from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloRecoSequences import precisionCaloRecoSequence_FWD
    (precisionCaloInViewSequence, sequenceOut) = precisionCaloRecoSequence_FWD(None,InViewRoIs)

    precisionCaloViewsMaker.ViewNodeName = precisionCaloInViewSequence.name()

    # connect EVC and reco
    theSequence = seqAND("precisionCaloSequence_FWD", [precisionCaloViewsMaker, precisionCaloInViewSequence] )
    return (theSequence, precisionCaloViewsMaker, sequenceOut)


def precisionCaloMenuSequence_FWD(name, is_probe_leg=False):
    """ Creates precisionCalo FWD MENU sequence """
    (sequence, precisionCaloViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(precisionCaloSequence_FWD, ConfigFlags)

    #Hypo
    from TrigEgammaForwardHypo.TrigEgammaForwardHypoConf import TrigEgammaForwardPrecisionCaloHypoAlg
    from TrigEgammaForwardHypo.TrigEgammaForwardPrecisionCaloHypoTool import TrigEgammaForwardPrecisionCaloHypoToolFromDict

    thePrecisionCaloHypo = TrigEgammaForwardPrecisionCaloHypoAlg(name+"precisionCaloHypo_FWD")
    thePrecisionCaloHypo.CaloClusters = sequenceOut

    return MenuSequence( Sequence    = sequence,
                         Maker       = precisionCaloViewsMaker, 
                         Hypo        = thePrecisionCaloHypo,
                         HypoToolGen = TrigEgammaForwardPrecisionCaloHypoToolFromDict,
                         IsProbe     = is_probe_leg )
