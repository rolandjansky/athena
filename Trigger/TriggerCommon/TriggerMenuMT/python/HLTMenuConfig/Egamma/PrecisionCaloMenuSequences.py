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
      
class precisionCaloMenuDefs(object):
    """Static class to collect all string manipulation in Calo sequences """
    precisionCaloClusters = recordable("HLT_CaloEMClusters")
    precisionHICaloClusters = recordable("HLT_HICaloEMClusters")
    egEventShape = recordable('HLT_HIEventShapeEG')

    @classmethod
    def caloClusters(cls, ion):
        return cls.precisionHICaloClusters if ion is True else cls.precisionCaloClusters

def tag(ion, is_photon):
    return 'precision' + ('HI' if ion is True else '') + 'Calo' + ('Photon' if is_photon else 'Electron')

def precisionCaloSequence(ConfigFlags, ion=False, is_photon=False):
    """ Creates PrecisionCalo sequence """
    # EV creator
    InViewRoIs="PrecisionCaloRoIs"     
    precisionCaloViewsMaker = EventViewCreatorAlgorithm('IM' + tag(ion,is_photon))
    precisionCaloViewsMaker.ViewFallThrough = True
    precisionCaloViewsMaker.RoIsLink = "initialRoI" # Merge inputs based on their initial L1 ROI
    roiTool = ViewCreatorPreviousROITool()
    # Note: This step processes Decision Objects which have followed either Electron reco, Photon reco, or both.
    # For Decision Object which have followed both, there is an ambiguity about which ROI should be used in this
    # merged step. In such cases we break the ambiguity by specifying that the Electron ROI is to be used.
    roiTool.RoISGKey = "HLT_Roi_FastElectron"
    precisionCaloViewsMaker.RoITool = roiTool
    precisionCaloViewsMaker.InViewRoIs = InViewRoIs
    precisionCaloViewsMaker.Views = tag(ion,is_photon) + 'Views'
    precisionCaloViewsMaker.RequireParentView = True
    precisionCaloViewsMaker.CacheDisabled = True

    # reco sequence
    from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloRecoSequences import precisionCaloRecoSequence
    (precisionCaloInViewSequence, sequenceOut) = precisionCaloRecoSequence(None, InViewRoIs, ion, is_photon)
        
    precisionCaloViewsMaker.ViewNodeName = precisionCaloInViewSequence.name()

    theSequence = seqAND(tag(ion,is_photon) + 'Sequence', [])

    if ion is True:
        # add UE subtraction for heavy ion e/gamma triggers
        # NOTE: UE subtraction requires an average pedestal to be calculated
        # using the full event (FS info), and has to be done outside of the
        # event views in this sequence. the egammaFSRecoSequence is thus placed
        # before the precisionCaloInViewSequence.
        from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloRecoSequences import egammaFSCaloRecoSequence
        egammaFSRecoSequence = egammaFSCaloRecoSequence()

        theSequence += egammaFSRecoSequence

    # connect EVC and reco
    theSequence += [precisionCaloViewsMaker, precisionCaloInViewSequence]
    return (theSequence, precisionCaloViewsMaker, sequenceOut)

def precisionCaloMenuSequence(name, is_probe_leg=False, ion=False, is_photon=False):
    """ Creates precisionCalo MENU sequence """

    (sequence, precisionCaloViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(precisionCaloSequence, ConfigFlags, ion=ion, is_photon=is_photon)

    #Hypo
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionCaloHypoAlg
    from TrigEgammaHypo.TrigEgammaPrecisionCaloHypoTool import TrigEgammaPrecisionCaloHypoToolFromDict

    thePrecisionCaloHypo = TrigEgammaPrecisionCaloHypoAlg(name + tag(ion,is_photon) + 'Hypo')
    thePrecisionCaloHypo.CaloClusters = sequenceOut

    return MenuSequence( Sequence    = sequence,
                         Maker       = precisionCaloViewsMaker, 
                         Hypo        = thePrecisionCaloHypo,
                         HypoToolGen = TrigEgammaPrecisionCaloHypoToolFromDict,
                         IsProbe     = is_probe_leg)