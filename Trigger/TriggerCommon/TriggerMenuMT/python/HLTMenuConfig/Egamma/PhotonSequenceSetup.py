# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorInitialROITool
import AthenaCommon.CfgMgr as CfgMgr
from TrigEDMConfig.TriggerEDMRun3 import recordable

# logger
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Egamma.PhotonSequenceSetup' )


def fastPhotonMenuSequence():
    """Creates secpond step photon sequence"""
    
    from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import CaloMenuDefs
    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("FastPhotonViewDataVerifier")
    ViewVerify.DataObjects = [( 'xAOD::TrigEMClusterContainer' , 'StoreGateSvc+' + CaloMenuDefs.L2CaloClusters ),
                              ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+EMIDRoIs' )]

    from TrigEgammaHypo.TrigEgammaFastPhotonFexMTConfig import EgammaFastPhotonFex_1
    thePhotonFex= EgammaFastPhotonFex_1()
    thePhotonFex.TrigEMClusterName = CaloMenuDefs.L2CaloClusters
    thePhotonFex.PhotonsName=recordable("HLT_FastPhotons")
    #thePhotonFex.RoIs="EMIDRoIs"

    l2PhotonViewsMaker = EventViewCreatorAlgorithm("IMl2Photon")
    l2PhotonViewsMaker.RoIsLink = "initialRoI"
    l2PhotonViewsMaker.InViewRoIs = "EMIDRoIs" 
    #l2PhotonViewsMaker.InViewRoIs = "EMCaloRoIs"
    l2PhotonViewsMaker.RoITool = ViewCreatorInitialROITool()
    l2PhotonViewsMaker.Views = "EMPhotonViews"
    l2PhotonViewsMaker.ViewFallThrough = True
    l2PhotonViewsMaker.RequireParentView = True
    
    thePhotonFex.RoIs = l2PhotonViewsMaker.InViewRoIs


    photonInViewAlgs = parOR("photonInViewAlgs", [ViewVerify, thePhotonFex ])

    l2PhotonViewsMaker.ViewNodeName = "photonInViewAlgs"


    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaFastPhotonHypoAlgMT
    thePhotonHypo = TrigEgammaFastPhotonHypoAlgMT()
    thePhotonHypo.Photons = thePhotonFex.PhotonsName
    thePhotonHypo.RunInView=True

    # this needs to be added:
    #electronDecisionsDumper = DumpDecisions("electronDecisionsDumper", Decisions = theElectronHypo.Output )

    photonAthSequence = seqAND("photonAthSequence",  [l2PhotonViewsMaker, photonInViewAlgs] )
    from TrigEgammaHypo.TrigEgammaFastPhotonHypoTool import TrigEgammaFastPhotonHypoToolFromDict


    return MenuSequence( Maker=l2PhotonViewsMaker,
                         Sequence=photonAthSequence,
                         Hypo=thePhotonHypo,
                         HypoToolGen=TrigEgammaFastPhotonHypoToolFromDict)


def precisionPhotonSequence(ConfigFlags):
    """ This function creates the PrecisionPhoton sequence"""

    # Prepare first the EventView
    InViewRoIs="PrecisionPhotonRoIs"                                          
    precisionPhotonViewsMaker = EventViewCreatorAlgorithm( "IMprecisionPhoton") 
    precisionPhotonViewsMaker.ViewFallThrough = True                          
    precisionPhotonViewsMaker.RequireParentView = True
    precisionPhotonViewsMaker.RoIsLink = "initialRoI"            # ROI link used to merge inputs
    precisionPhotonViewsMaker.RoITool = ViewCreatorInitialROITool() # Tool used to supply ROIs for EventViews
    precisionPhotonViewsMaker.InViewRoIs = InViewRoIs            # names to use for the collection of which the RoIs are picked up
    precisionPhotonViewsMaker.Views = "precisionPhotonViews"     # Output container which has the view objects

    # Configure the reconstruction algorithm sequence
    from TriggerMenuMT.HLTMenuConfig.Photon.PhotonRecoSequences import precisionPhotonRecoSequence
    (precisionPhotonInViewSequence, sequenceOut) = precisionPhotonRecoSequence(InViewRoIs)

    precisionPhotonViewsMaker.ViewNodeName = precisionPhotonInViewSequence.name()

    theSequence = seqAND("precisionPhotonSequence", [precisionPhotonViewsMaker,precisionPhotonInViewSequence])
    return (theSequence, precisionPhotonViewsMaker, sequenceOut)



def precisionPhotonMenuSequence(name):

    # This will be executed after pricisionCalo, so we need to pickup indeed the topoClusters by precisionCalo and add them here as requirements

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    """Creates precisionPhoton  sequence"""
    (sequence, precisionPhotonViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(precisionPhotonSequence,ConfigFlags)

    # Hypo 

    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionPhotonHypoAlgMT
    from TrigEgammaHypo.TrigEgammaPrecisionPhotonHypoTool import TrigEgammaPrecisionPhotonHypoToolFromDict

    thePrecisionPhotonHypo = TrigEgammaPrecisionPhotonHypoAlgMT(name+"precisionPhotonHypo")
    thePrecisionPhotonHypo.Photons = sequenceOut

    return MenuSequence( Sequence    = sequence,
                         Maker       = precisionPhotonViewsMaker, 
                         Hypo        = thePrecisionPhotonHypo,
                         HypoToolGen = TrigEgammaPrecisionPhotonHypoToolFromDict)

