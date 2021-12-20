# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration



# menu components   
from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaKeys import getTrigEgammaKeys
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorCentredOnClusterROITool
import AthenaCommon.CfgMgr as CfgMgr

# logger
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)


def fastPhotonSequence(flags):
    """Creates secpond step photon sequence"""
    
    TrigEgammaKeys = getTrigEgammaKeys()

    from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequences import CaloMenuDefs

    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("FastPhotonViewDataVerifier")
    ViewVerify.DataObjects = [( 'xAOD::TrigEMClusterContainer' , 'StoreGateSvc+%s' % CaloMenuDefs.L2CaloClusters ),
                              ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+EMIDRoIs' )]


    from TrigEgammaRec.TrigEgammaFastPhotonConfig import TrigEgammaFastPhoton_ReFastAlgo 
    thePhotonFex = TrigEgammaFastPhoton_ReFastAlgo ()
    thePhotonFex.TrigEMClusterName = CaloMenuDefs.L2CaloClusters # From commom staff
    thePhotonFex.PhotonsName= TrigEgammaKeys.fastPhotonContainer
    #thePhotonFex.RoIs="EMIDRoIs"


    l2PhotonViewsMaker = EventViewCreatorAlgorithm("IMl2Photon")
    l2PhotonViewsMaker.RoIsLink = "initialRoI" # Merge inputs based on their initial L1 ROI
    # Spawn View on SuperRoI encompassing all clusters found within the L1 RoI
    roiTool = ViewCreatorCentredOnClusterROITool()
    roiTool.AllowMultipleClusters = False # If True: SuperROI mode. If False: highest eT cluster in the L1 ROI
    roiTool.RoisWriteHandleKey = TrigEgammaKeys.fastPhotonRoIContainer
    # not running the tracking here, so do not need to set this size 
    # from the ID Trigger configuration, however, if we want overlap 
    # of the Rois then we would need to use the electron instance size
    # for consistency
    roiTool.RoIEtaWidth = 0.05
    roiTool.RoIPhiWidth = 0.10
    l2PhotonViewsMaker.RoITool = roiTool
    l2PhotonViewsMaker.InViewRoIs = "EMIDRoIs"
    #l2PhotonViewsMaker.InViewRoIs = "EMCaloRoIs"
    l2PhotonViewsMaker.Views = "EMPhotonViews"
    l2PhotonViewsMaker.ViewFallThrough = True
    l2PhotonViewsMaker.RequireParentView = True

    thePhotonFex.RoIs = l2PhotonViewsMaker.InViewRoIs

    photonInViewAlgs = parOR("photonInViewAlgs", [ViewVerify, thePhotonFex ])

    l2PhotonViewsMaker.ViewNodeName = "photonInViewAlgs"


    
    # this needs to be added:
    #electronDecisionsDumper = DumpDecisions("electronDecisionsDumper", Decisions = [theElectronHypo.Output] )

    photonAthSequence = seqAND("photonAthSequence",  [l2PhotonViewsMaker, photonInViewAlgs] )
    
    return (photonAthSequence, l2PhotonViewsMaker)
    


def fastPhotonMenuSequence(flags=None):
    """Creates secpond step photon sequence"""
    
    TrigEgammaKeys = getTrigEgammaKeys()

    # retrieve the reco sequence+IM
    (photonAthSequence, l2PhotonViewsMaker) = RecoFragmentsPool.retrieve(fastPhotonSequence, flags)

    # make the hypo
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaFastPhotonHypoAlg
    thePhotonHypo = TrigEgammaFastPhotonHypoAlg()
    thePhotonHypo.Photons = TrigEgammaKeys.fastPhotonContainer
    thePhotonHypo.RunInView=True

    from TrigEgammaHypo.TrigEgammaFastPhotonHypoTool import TrigEgammaFastPhotonHypoToolFromDict

    return MenuSequence( Maker=l2PhotonViewsMaker,
                         Sequence=photonAthSequence,
                         Hypo=thePhotonHypo,
                         HypoToolGen=TrigEgammaFastPhotonHypoToolFromDict
                         )

