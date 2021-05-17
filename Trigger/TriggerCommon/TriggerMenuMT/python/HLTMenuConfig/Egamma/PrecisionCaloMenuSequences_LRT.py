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
      
class precisionCaloMenuDefs_LRT(object):
      """Static class to collect all string manipulation in Calo sequences """
      precisionCaloClusters= recordable("HLT_CaloEMClusters_LRT")

def precisionCaloSequence_LRT(ConfigFlags):
    """ Creates PrecisionCalo sequence """
    # EV creator
    InViewRoIs="PrecisionCaloRoIs_LRT"     
    precisionCaloViewsMaker = EventViewCreatorAlgorithm( "IMprecisionCalo_LRT")
    precisionCaloViewsMaker.ViewFallThrough = True
    precisionCaloViewsMaker.RoIsLink = "initialRoI" # Merge inputs based on their initial L1 ROI
    roiTool = ViewCreatorPreviousROITool()
    # Note: This step processes Decision Objects which have followed either Electron reco, Photon reco, or both.
    # For Decision Object which have followed both, there is an ambiguity about which ROI should be used in this
    # merged step. In such cases we break the ambiguity by specifying that the Electron ROI is to be used.
    roiTool.RoISGKey = "HLT_Roi_FastElectron_LRT"
    precisionCaloViewsMaker.RoITool = roiTool
    precisionCaloViewsMaker.InViewRoIs = InViewRoIs
    precisionCaloViewsMaker.Views = "precisionCaloViews_LRT"
    precisionCaloViewsMaker.RequireParentView = True

    # reco sequence
    from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloRecoSequences import precisionCaloRecoSequence_LRT
    (precisionCaloInViewSequence, sequenceOut) = precisionCaloRecoSequence_LRT(None,InViewRoIs)

    precisionCaloViewsMaker.ViewNodeName = precisionCaloInViewSequence.name()

    # connect EVC and reco
    theSequence = seqAND("precisionCaloSequence_LRT", [precisionCaloViewsMaker, precisionCaloInViewSequence] )
    return (theSequence, precisionCaloViewsMaker, sequenceOut)

def precisionCaloMenuSequence_LRT(name,is_probe_leg=False):
    """ Creates precisionCalo MENU sequence """
    (sequence, precisionCaloViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(precisionCaloSequence_LRT, ConfigFlags)

    #Hypo
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionCaloHypoAlg
    from TrigEgammaHypo.TrigEgammaPrecisionCaloHypoTool import TrigEgammaPrecisionCaloHypoToolFromDict

    thePrecisionCaloHypo = TrigEgammaPrecisionCaloHypoAlg(name+"precisionCaloHypo_LRT")
    thePrecisionCaloHypo.CaloClusters = sequenceOut

    return MenuSequence( Sequence    = sequence,
                         Maker       = precisionCaloViewsMaker, 
                         Hypo        = thePrecisionCaloHypo,
                         HypoToolGen = TrigEgammaPrecisionCaloHypoToolFromDict,
                         IsProbe     = is_probe_leg)
