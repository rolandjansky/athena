#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

import AthenaCommon.CfgMgr as CfgMgr
from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from TrigUpgradeTest.tauDefs import tauCaloSequence


# ====================================================================================================  
#    Get MenuSequences
# ==================================================================================================== 

def getTauSequence( step ):
    if step == "calo":
        return tauCaloMenuSequence("Tau")
    if step == "track_core":
        return tauCoreTrackSequence()
    return None

# ===============================================================================================
#      Fist step - tau calo
# ===============================================================================================

def tauCaloMenuSequence(name):
    (sequence, tauCaloViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauCaloSequence,ConfigFlags)

    # hypo
    from TrigTauHypo.TrigTauHypoConf import TrigTauCaloHypoAlgMT
    theTauCaloHypo = TrigTauCaloHypoAlgMT(name+"L2CaloHypo")
    theTauCaloHypo.taujets     = sequenceOut

    from TrigTauHypo.TrigL2TauHypoTool import TrigL2TauHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = tauCaloViewsMaker,
                          Hypo        = theTauCaloHypo,
                          HypoToolGen = TrigL2TauHypoToolFromDict )

# ===============================================================================================
#      Second step - fast tracking
# ===============================================================================================

def tauCoreTrackSequence():

    from TriggerMenuMT.HLTMenuConfig.CommonSequences.InDetSetup import makeInDetAlgs
    (viewAlgs, eventAlgs) = makeInDetAlgs("TauCore")

    from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_TauCore

    theFTFCore = TrigFastTrackFinder_TauCore()
    theFTFCore.isRoI_Seeded = True
    viewAlgs.append(theFTFCore)


    # A simple algorithm to confirm that data has been inherited from parent view
    # Required to satisfy data dependencies
    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("tauViewDataVerifier")
    ViewVerify.DataObjects = [('xAOD::TauJetContainer','StoreGateSvc+taujets')]
    viewAlgs.append(ViewVerify)


    from TrigTauHypo.TrigTauHypoConf import TrigTauTrackRoiUpdaterMT
    TrackRoiUpdater = TrigTauTrackRoiUpdaterMT("TrackRoiUpdater")
    TrackRoiUpdater.RoIInputKey  = "TAUCaloRoIs"
    TrackRoiUpdater.RoIOutputKey = "RoiForID2"
    TrackRoiUpdater.fastTracksKey = "TrigFastTrackFinder_Tracks"

    l2TauViewsMaker = EventViewCreatorAlgorithm("l2TauViewsMaker")
    l2TauViewsMaker.RoIsLink = "roi" # -||-
    l2TauViewsMaker.InViewRoIs = "TCoreRoIs" # contract with the fastCalo
    l2TauViewsMaker.Views = "TAUIDViews"
    l2TauViewsMaker.ViewFallThrough = True


    for viewAlg in viewAlgs:
       if "RoIs" in viewAlg.properties():
         viewAlg.RoIs = l2TauViewsMaker.InViewRoIs
       if "roiCollectionName" in viewAlg.properties():
         viewAlg.roiCollectionName = l2TauViewsMaker.InViewRoIs
    TrackRoiUpdater.RoIInputKey = l2TauViewsMaker.InViewRoIs

    tauInViewAlgs = parOR("tauInViewAlgs", viewAlgs + [ TrackRoiUpdater ])

    l2TauViewsMaker.ViewNodeName = "tauInViewAlgs"

    tauCoreTrkAthSequence = seqAND("tauCoreTrkAthSequence", eventAlgs + [l2TauViewsMaker, tauInViewAlgs ] )

    from TrigTauHypo.TrigTauHypoConf import  TrigTrackPreSelHypoAlgMT
    fastTrkHypo = TrigTrackPreSelHypoAlgMT("TrackPreSelHypoAlg")
    fastTrkHypo.trackcollection = TrackRoiUpdater.fastTracksKey

    from TrigTauHypo.TrigTrackPreSelHypoTool import TrigTauTrackHypoToolFromDict

    return  MenuSequence( Sequence    = tauCoreTrkAthSequence,
                          Maker       = l2TauViewsMaker,
                          Hypo        = fastTrkHypo,
                          HypoToolGen = TrigTauTrackHypoToolFromDict )
