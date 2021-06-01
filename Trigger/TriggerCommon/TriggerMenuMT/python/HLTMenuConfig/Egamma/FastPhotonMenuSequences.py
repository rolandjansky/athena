# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorCentredOnClusterROITool
import AthenaCommon.CfgMgr as CfgMgr
from TrigEDMConfig.TriggerEDMRun3 import recordable

# logger
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)


def fastPhotonMenuSequence():
    """Creates secpond step photon sequence"""
    
    from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequences import CaloMenuDefs

    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("FastPhotonViewDataVerifier")
    ViewVerify.DataObjects = [( 'xAOD::TrigEMClusterContainer' , 'StoreGateSvc+%s' % CaloMenuDefs.L2CaloClusters ),
                              ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+EMIDRoIs' )]


    from TrigEgammaRec.TrigEgammaFastPhotonConfig import TrigEgammaFastPhoton_ReFastAlgo 
    thePhotonFex = TrigEgammaFastPhoton_ReFastAlgo ()
    thePhotonFex.TrigEMClusterName = CaloMenuDefs.L2CaloClusters
    thePhotonFex.PhotonsName=recordable("HLT_FastPhotons")
    #thePhotonFex.RoIs="EMIDRoIs"


    l2PhotonViewsMaker = EventViewCreatorAlgorithm("IMl2Photon")
    l2PhotonViewsMaker.RoIsLink = "initialRoI" # Merge inputs based on their initial L1 ROI
    # Spawn View on SuperRoI encompassing all clusters found within the L1 RoI
    roiTool = ViewCreatorCentredOnClusterROITool()
    roiTool.AllowMultipleClusters = False # If True: SuperROI mode. If False: highest eT cluster in the L1 ROI
    roiTool.RoisWriteHandleKey = recordable("HLT_Roi_FastPhoton")
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


    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaFastPhotonHypoAlg
    thePhotonHypo = TrigEgammaFastPhotonHypoAlg()
    thePhotonHypo.Photons = thePhotonFex.PhotonsName
    thePhotonHypo.RunInView=True

    # this needs to be added:
    #electronDecisionsDumper = DumpDecisions("electronDecisionsDumper", Decisions = [theElectronHypo.Output] )

    photonAthSequence = seqAND("photonAthSequence",  [l2PhotonViewsMaker, photonInViewAlgs] )
    from TrigEgammaHypo.TrigEgammaFastPhotonHypoTool import TrigEgammaFastPhotonHypoToolFromDict

    return MenuSequence( Maker=l2PhotonViewsMaker,
                         Sequence=photonAthSequence,
                         Hypo=thePhotonHypo,
                         HypoToolGen=TrigEgammaFastPhotonHypoToolFromDict)

