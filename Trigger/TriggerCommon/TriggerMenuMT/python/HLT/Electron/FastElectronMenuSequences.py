#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags 

# menu components   
from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorCentredOnClusterROITool
from TriggerMenuMT.HLT.Egamma.TrigEgammaKeys import getTrigEgammaKeys


def fastElectronSequence(ConfigFlags, variant=''):
    """ second step:  tracking....."""

    TrigEgammaKeys = getTrigEgammaKeys(variant)
    IDTrigConfig = TrigEgammaKeys.IDTrigConfig
  
    from TrigInDetConfig.InDetTrigFastTracking import makeInDetTrigFastTracking
    RoIs = "EMIDRoIs"+variant # contract with the fastCalo
    viewAlgs, viewVerify = makeInDetTrigFastTracking( config = IDTrigConfig, rois = RoIs )

    # A simple algorithm to confirm that data has been inherited from parent view
    # Required to satisfy data dependencies
    from TriggerMenuMT.HLT.CommonSequences.CaloSequences import CaloMenuDefs  
    viewVerify.DataObjects += [( 'xAOD::TrigEMClusterContainer' , 'StoreGateSvc+%s' % CaloMenuDefs.L2CaloClusters ),
                               ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % RoIs )]

    TrackParticlesName = ""
    for viewAlg in viewAlgs:
        if "InDetTrigTrackParticleCreatorAlg" in viewAlg.name():
            TrackParticlesName = viewAlg.TrackParticlesName
      

    from TrigEgammaRec.TrigEgammaFastElectronConfig import TrigEgammaFastElectron_ReFastAlgo_Clean
    theElectronFex = TrigEgammaFastElectron_ReFastAlgo_Clean("EgammaFastElectronFex_Clean_gen"+variant)

    theElectronFex.TrigEMClusterName = CaloMenuDefs.L2CaloClusters
    theElectronFex.TrackParticlesName = TrackParticlesName
    theElectronFex.ElectronsName=TrigEgammaKeys.fastElectronContainer
    theElectronFex.DummyElectronsName= "HLT_FastDummyElectrons"
 
    # EVCreator:
    l2ElectronViewsMaker = EventViewCreatorAlgorithm("IMl2Electron"+variant)
    l2ElectronViewsMaker.RoIsLink = "initialRoI" # Merge inputs based on their initial L1 ROI
    # Spawn View on SuperRoI encompassing all clusters found within the L1 RoI
    roiTool = ViewCreatorCentredOnClusterROITool()
    roiTool.AllowMultipleClusters = False # If True: SuperROI mode. If False: highest eT cluster in the L1 ROI
    roiTool.RoisWriteHandleKey = TrigEgammaKeys.fastElectronRoIContainer
    roiTool.RoIEtaWidth = IDTrigConfig.etaHalfWidth
    roiTool.RoIPhiWidth = IDTrigConfig.phiHalfWidth
    l2ElectronViewsMaker.RoITool = roiTool
    l2ElectronViewsMaker.InViewRoIs = RoIs
    l2ElectronViewsMaker.Views = "EMElectronViews"+variant
    l2ElectronViewsMaker.ViewFallThrough = True
    l2ElectronViewsMaker.RequireParentView = True

    theElectronFex.RoIs = l2ElectronViewsMaker.InViewRoIs
    electronInViewAlgs = parOR("electronInViewAlgs"+variant, viewAlgs + [ theElectronFex ])
    l2ElectronViewsMaker.ViewNodeName = "electronInViewAlgs"+variant

    electronAthSequence = seqAND("electronAthSequence"+variant, [l2ElectronViewsMaker, electronInViewAlgs ] )
    return (electronAthSequence, l2ElectronViewsMaker)


def fastElectronSequence_LRT(ConfigFlags):
    # This is SAME as fastElectronSequence but for variant "_LRT"
    return fastElectronSequence(ConfigFlags,"_LRT")



def fastElectronMenuSequence(do_idperf,is_probe_leg=False, variant=''):
    """ Creates 2nd step Electron  MENU sequence"""
    # retrieve the reco sequence+IM
    theSequence = {
            ''      : fastElectronSequence,
            '_LRT'  : fastElectronSequence_LRT
            }
    (electronAthSequence, l2ElectronViewsMaker) = RecoFragmentsPool.retrieve(theSequence[variant], ConfigFlags)

    # make the Hypo
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaFastElectronHypoAlg
    TrigEgammaKeys = getTrigEgammaKeys(variant)

    if do_idperf is True:
        theElectronHypo = TrigEgammaFastElectronHypoAlg("TrigEgammaFastElectronHypoAlg_idperf"+variant)
        theElectronHypo.Electrons = "HLT_FastDummyElectrons"
    else:
        theElectronHypo = TrigEgammaFastElectronHypoAlg("TrigEgammaFastElectronHypoAlg"+variant)
        theElectronHypo.Electrons = TrigEgammaKeys.fastElectronContainer

    theElectronHypo.RunInView=True

    from TrigEgammaHypo.TrigEgammaFastElectronHypoTool import TrigEgammaFastElectronHypoToolFromDict

    return  MenuSequence( Maker       = l2ElectronViewsMaker,                                        
                          Sequence    = electronAthSequence,
                          Hypo        = theElectronHypo,
                          HypoToolGen = TrigEgammaFastElectronHypoToolFromDict,
                          IsProbe=is_probe_leg)


def fastElectronMenuSequence_LRT(do_idperf,is_probe_leg=False):
    # This is to call fastElectronMenuSequence for the _LRT variant
    return fastElectronMenuSequence(do_idperf,is_probe_leg=is_probe_leg, variant='_LRT')
