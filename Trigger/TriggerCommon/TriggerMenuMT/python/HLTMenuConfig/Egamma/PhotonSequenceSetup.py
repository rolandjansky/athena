#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import AthenaCommon.CfgMgr as CfgMgr

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm




def fastPhotonMenuSequence():
    """Creates secpond step photon sequence"""
    
    from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import CaloMenuDefs
    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("FastPhotonViewDataVerifier")
    ViewVerify.DataObjects = [('xAOD::TrigEMClusterContainer','StoreGateSvc+'+ CaloMenuDefs.L2CaloClusters)]

    from TrigEgammaHypo.TrigL2PhotonFexMTConfig import L2PhotonFex_1
    thePhotonFex= L2PhotonFex_1()
    thePhotonFex.TrigEMClusterName = CaloMenuDefs.L2CaloClusters
    thePhotonFex.PhotonsName="TrigL2Photons"
    #thePhotonFex.RoIs="EMIDRoIs"

    l2PhotonViewsMaker = EventViewCreatorAlgorithm("IMl2Photon")
    l2PhotonViewsMaker.RoIsLink = "roi"
    l2PhotonViewsMaker.InViewRoIs = "EMIDRoIs" 
    #l2PhotonViewsMaker.InViewRoIs = "EMCaloRoIs"
    l2PhotonViewsMaker.Views = "EMPhotonViews"
    l2PhotonViewsMaker.ViewFallThrough = True
    l2PhotonViewsMaker.RequireParentView = True
    
    thePhotonFex.RoIs = l2PhotonViewsMaker.InViewRoIs


    photonInViewAlgs = parOR("photonInViewAlgs", [ViewVerify, thePhotonFex ])

    l2PhotonViewsMaker.ViewNodeName = "photonInViewAlgs"


    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2PhotonHypoAlgMT
    thePhotonHypo = TrigL2PhotonHypoAlgMT()
    thePhotonHypo.Photons = thePhotonFex.PhotonsName
    thePhotonHypo.RunInView=True

    # this needs to be added:
    #electronDecisionsDumper = DumpDecisions("electronDecisionsDumper", Decisions = theElectronHypo.Output )

    photonAthSequence = seqAND("photonAthSequence",  [l2PhotonViewsMaker, photonInViewAlgs] )
    from TrigEgammaHypo.TrigL2PhotonHypoTool import TrigL2PhotonHypoToolFromDict


    return MenuSequence( Maker=l2PhotonViewsMaker,
                         Sequence=photonAthSequence,
                         Hypo=thePhotonHypo,
                         HypoToolGen=TrigL2PhotonHypoToolFromDict)


def precisionPhotonSequence(ConfigFlags):
    """ This function creates the PrecisionPhoton sequence"""

    InViewRoIs="PrecisionPhotonRoIs"                                          
    precisionPhotonViewsMaker = EventViewCreatorAlgorithm( "IMprecisionPhoton") 
    precisionPhotonViewsMaker.ViewFallThrough = True                          
    precisionPhotonViewsMaker.RoIsLink = "PrecisionCaloRoIs"  # <------ Is it here the output of the previous step?
    precisionPhotonViewsMaker.InViewRoIs = InViewRoIs                         
    precisionPhotonViewsMaker.Views = "precisionPhotonViews"                    

    # Configure the reconstruction algorithm sequence
    from TriggerMenuMT.HLTMenuConfig.Photon.PhotonRecoSequences import precisionPhotonRecoSequence
    (precisionPhotonInViewSequence, sequenceOut) = precisionPhotonRecoSequence(InViewRoIs)


    theSequence = seqAND("precisionPhotonSequence", [precisionPhotonViewsMaker,precisionPhotonInViewSequence])
    return (theSequence, precisionPhotonViewsMaker, sequenceOut)



def precisionPhotonMenuSequence(name):
    # First the data verifiers:
    # Here we define the data dependencies. What input needs to be available for the Fexs (i.e. TopoClusters from precisionCalo) in order to run
    from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloSequenceSetup import precisionCaloMenuDefs
    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("PrecisionPhotonPhotonViewDataVerifier")
    ViewVerify.DataObjects = [('xAOD::CaloClusterContainer','StoreGateSvc+'+ precisionCaloMenuDefs.precisionCaloClusters)]

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

