#
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Include import include
from AthenaCommon.Constants import VERBOSE,DEBUG
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
import AthenaCommon.CfgMgr as CfgMgr

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from AthenaCommon.CFElements import parOR, seqOR, seqAND, stepSeq
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm




def photonMenuSequence():
    """ Creates secpond step photon sequence"""
    
    from TrigUpgradeTest.CaloMenuDefs import CaloMenuDefs
    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("photonViewDataVerifier")
    ViewVerify.DataObjects = [('xAOD::TrigEMClusterContainer','StoreGateSvc+'+ CaloMenuDefs.L2CaloClusters)]
    ViewVerify.OutputLevel = DEBUG

    from TrigEgammaHypo.TrigL2PhotonFexMTConfig import L2PhotonFex_1
    thePhotonFex= L2PhotonFex_1()
    thePhotonFex.TrigEMClusterName = CaloMenuDefs.L2CaloClusters
    thePhotonFex.PhotonsName="Photons"
    thePhotonFex.OutputLevel=VERBOSE
    #thePhotonFex.RoIs="EMIDRoIs"

    l2PhotonViewsMaker = EventViewCreatorAlgorithm("l2PhotonViewsMaker", OutputLevel=DEBUG)
    l2PhotonViewsMaker.RoIsLink = "roi"
    l2PhotonViewsMaker.InViewRoIs = "EMIDRoIs" 
    #l2PhotonViewsMaker.InViewRoIs = "EMCaloRoIs"
    l2PhotonViewsMaker.Views = "EMPhotonViews"
    l2PhotonViewsMaker.ViewFallThrough = True
    
    thePhotonFex.RoIs = l2PhotonViewsMaker.InViewRoIs


    photonInViewAlgs = parOR("photonInViewAlgs", [ViewVerify, thePhotonFex ])

    l2PhotonViewsMaker.ViewNodeName = "photonInViewAlgs"


    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2PhotonHypoAlgMT
    thePhotonHypo = TrigL2PhotonHypoAlgMT()
    thePhotonHypo.Photons = thePhotonFex.PhotonsName
    thePhotonHypo.RunInView=True
    thePhotonHypo.OutputLevel = VERBOSE

    # this needs to be added:
    #electronDecisionsDumper = DumpDecisions("electronDecisionsDumper", OutputLevel=DEBUG, Decisions = theElectronHypo.Output )    

    photonAthSequence = seqAND("photonAthSequence",  [l2PhotonViewsMaker, photonInViewAlgs] )
    from TrigEgammaHypo.TrigL2PhotonHypoTool import TrigL2PhotonHypoToolFromName


    return MenuSequence( Maker=l2PhotonViewsMaker,
                             Sequence=photonAthSequence,
                             Hypo=thePhotonHypo,
                             HypoToolGen=TrigL2PhotonHypoToolFromName)

