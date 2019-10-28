#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

import AthenaCommon.CfgMgr as CfgMgr
from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from TrigEDMConfig.TriggerEDMRun3 import recordable
from TriggerMenuMT.HLTMenuConfig.Tau.TauRecoSequences import tauCaloSequence, tauCaloMVASequence, tauFTFCoreSequence, tauFTFIsoSequence

# ====================================================================================================  
#    Get MenuSequences
# ==================================================================================================== 

def getTauSequence( step ):
    if step == "calo":
        return tauCaloMenuSequence("Tau")
    if step == "calo_mva":
        return tauCaloMVAMenuSequence("Tau")
    if step == "track_core":
        return tauCoreTrackSequence()
    if step == "precision":
        return tauPrecisionSequence()
    if step == "track_twostep_core":
        return tauTwoStepTrackSeqCore()
    if step == "track_twostep_iso":
        return tauTwoStepTrackSeqIso()
    return None

# ===============================================================================================
#      Calo step
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
#      Calo MVA step
# ===============================================================================================

def tauCaloMVAMenuSequence(name):
    (sequence, tauCaloMVAViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauCaloMVASequence,ConfigFlags)

    # hypo
    from TrigTauHypo.TrigTauHypoConf import TrigTauCaloHypoAlgMT
    theTauCaloMVAHypo = TrigTauCaloHypoAlgMT(name+"L2CaloMVAHypo")
    theTauCaloMVAHypo.taujets     = sequenceOut

    from TrigTauHypo.TrigL2TauHypoTool import TrigL2TauHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = tauCaloMVAViewsMaker,
                          Hypo        = theTauCaloMVAHypo,
                          HypoToolGen = TrigL2TauHypoToolFromDict )

# ===============================================================================================
#      Fast tracking step
# ===============================================================================================

def tauCoreTrackSequence():

    from TriggerMenuMT.HLTMenuConfig.CommonSequences.InDetSetup import makeInDetAlgs
    RoIs = "TCoreViewRoIs"
    viewAlgsTP = makeInDetAlgs(whichSignature='TauCore',separateTrackParticleCreator="_TauCore", rois = RoIs)

    # A simple algorithm to confirm that data has been inherited from parent view
    # Required to satisfy data dependencies
    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("tauViewDataVerifier")
    ViewVerify.DataObjects = [('xAOD::TauJetContainer','StoreGateSvc+HLT_TrigTauRecMerged')]
    viewAlgsTP.append(ViewVerify)


    from TrigTauHypo.TrigTauHypoConf import TrigTauTrackRoiUpdaterMT
    TrackRoiUpdater = TrigTauTrackRoiUpdaterMT("TrackRoiUpdater")
    #TrackRoiUpdater.RoIInputKey  = "TAUCaloRoIs"
    TrackRoiUpdater.RoIOutputKey = recordable("HLT_RoiForID")


    fastTrackViewsMaker = EventViewCreatorAlgorithm("IMTauFastTrack")
    fastTrackViewsMaker.RoIsLink = "roi" # -||-
    fastTrackViewsMaker.InViewRoIs = RoIs
    fastTrackViewsMaker.Views = "TAUIDViews"
    fastTrackViewsMaker.ViewFallThrough = True
    fastTrackViewsMaker.RequireParentView = True


    for viewAlg in viewAlgsTP:
       if "InDetTrigTrackParticleCreatorAlg" in viewAlg.name():
         TrackCollection = viewAlg.TrackName

    TrackRoiUpdater.RoIInputKey = fastTrackViewsMaker.InViewRoIs
    TrackRoiUpdater.fastTracksKey = TrackCollection

    tauInViewAlgs = parOR("tauInViewAlgs", viewAlgsTP + [ TrackRoiUpdater ])

    fastTrackViewsMaker.ViewNodeName = tauInViewAlgs.name()

    tauCoreTrkAthSequence = seqAND("tauCoreTrkAthSequence", [fastTrackViewsMaker, tauInViewAlgs ] )

    from TrigTauHypo.TrigTauHypoConf import  TrigTrackPreSelHypoAlgMT
    fastTrkHypo = TrigTrackPreSelHypoAlgMT("TrackPreSelHypoAlg")
    fastTrkHypo.trackcollection = TrackRoiUpdater.fastTracksKey

    from TrigTauHypo.TrigTrackPreSelHypoTool import TrigTauTrackHypoToolFromDict

    return  MenuSequence( Sequence    = tauCoreTrkAthSequence,
                          Maker       = fastTrackViewsMaker,
                          Hypo        = fastTrkHypo,
                          HypoToolGen = TrigTauTrackHypoToolFromDict )

# ===============================================================================================
#      Fast, precision tracking and ID step (altogether) / Precision tracking not included yet
# ===============================================================================================

def tauPrecisionSequence():

    from TriggerMenuMT.HLTMenuConfig.CommonSequences.InDetSetup import makeInDetAlgs
    RoIs = "TCoreViewRoIs" # contract with the fastCalo
    viewAlgsPT = makeInDetAlgs(whichSignature='Tau',separateTrackParticleCreator="_Tau", rois = RoIs)

    TrackParticlesName = ""
    for viewAlg in viewAlgsPT:
        if "InDetTrigTrackParticleCreatorAlg" in viewAlg.name():
            TrackParticlesName = viewAlg.TrackParticlesName
            TrackCollection = viewAlg.TrackName

    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("tauViewDataVerifier")
    ViewVerify.DataObjects = [('xAOD::TauJetContainer','StoreGateSvc+HLT_TrigTauRecMerged')]
    viewAlgsPT.append(ViewVerify)


    from TrigTauHypo.TrigTauHypoConf import TrigTauTrackRoiUpdaterMT
    precisionTRU = TrigTauTrackRoiUpdaterMT("precisionTRU")
    #TrackRoiUpdater.RoIInputKey  = "TAUCaloRoIs"
    precisionTRU.RoIOutputKey = recordable("HLT_RoiForID1")
    precisionTRU.fastTracksKey = TrackCollection
    #"TrigFastTrackFinder_Tracks"

    from TrigTauRec.TrigTauRecConfigMT import TrigTauRecMerged_TauPrecisionMVA
    trigTauMVA = TrigTauRecMerged_TauPrecisionMVA(doMVATES=True, doTrackBDT=False, doRNN=True)
    trigTauMVA.RoIInputKey = precisionTRU.RoIOutputKey
    trigTauMVA.L1RoIKey    = "TAUCaloRoIs"
    trigTauMVA.TrigTauRecOutputKey = recordable("HLT_TrigTauRecMerged_MVA")
    trigTauMVA.TrigTauJet = "HLT_TrigTauRecMerged"
    trigTauMVA.Key_trackPartInputContainer = TrackParticlesName
    trigTauMVA.UseCaloClusters = False
    #trigTauMVA.clustersKey = "caloclusters"
    trigTauMVA.Key_vertexInputContainer = "VxPrimaryCandidate"
    trigTauMVA.TrigTauTrkOutputKey = recordable("HLT_tautrack_MVA")

    precisionViewsMaker = EventViewCreatorAlgorithm("IMPrecisionTau")
    precisionViewsMaker.RoIsLink = "roi" # -||-
    precisionViewsMaker.InViewRoIs = "TPrecViewRoIs" # contract with the fastCalo
    precisionViewsMaker.Views = "TAUID2Views"
    precisionViewsMaker.ViewFallThrough = True
    precisionViewsMaker.RequireParentView = True

    precisionTRU.RoIInputKey = precisionViewsMaker.InViewRoIs

    tauPInViewAlgs = parOR("tauPInViewAlgs", viewAlgsPT + [ precisionTRU, trigTauMVA ])

    precisionViewsMaker.ViewNodeName = tauPInViewAlgs.name()

    tauPrecisionAthSequence = seqAND("tauPrecisionAthSequence", [precisionViewsMaker, tauPInViewAlgs ] )


    from TrigTauHypo.TrigTauHypoConf import  TrigEFTauMVHypoAlgMT
    precisionHypo = TrigEFTauMVHypoAlgMT("EFTauMVHypoAlg")
    precisionHypo.taujetcontainer = trigTauMVA.TrigTauRecOutputKey

    from TrigTauHypo.TrigEFTauMVHypoTool import TrigEFTauMVHypoToolFromDict

    return  MenuSequence( Sequence    = tauPrecisionAthSequence,
                          Maker       = precisionViewsMaker,
                          Hypo        = precisionHypo,
                          HypoToolGen = TrigEFTauMVHypoToolFromDict )


# ===============================================================================================
#      Fast, precision tracking and ID step (altogether) / Precision tracking not included yet
# ===============================================================================================

def tauTwoStepTrackSeqCore():

    (sequence, ftfCoreViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauFTFCoreSequence,ConfigFlags)    

    from TrigTauHypo.TrigTauHypoConf import  TrigTrackPreSelHypoAlgMT
    fastTrkHypo = TrigTrackPreSelHypoAlgMT("TrackPreSelHypoAlg_RejectEmpty")
    fastTrkHypo.trackcollection = sequenceOut

    from TrigTauHypo.TrigTrackPreSelHypoTool import TrigTauTrackHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = ftfCoreViewsMaker,
                          Hypo        = fastTrkHypo,
                          HypoToolGen = TrigTauTrackHypoToolFromDict )


def tauTwoStepTrackSeqIso():

    (sequence, ftfIsoViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(tauFTFIsoSequence,ConfigFlags )

    from TrigTauHypo.TrigTauHypoConf import  TrigEFTauMVHypoAlgMT
    precisionHypo = TrigEFTauMVHypoAlgMT("EFTauMVHypoAlgFinal")
    precisionHypo.taujetcontainer = sequenceOut

    from TrigTauHypo.TrigEFTauMVHypoTool import TrigEFTauMVHypoToolFromDict

    return  MenuSequence( Sequence    = sequence,
                          Maker       = ftfIsoViewsMaker,
                          Hypo        = precisionHypo,
                          HypoToolGen = TrigEFTauMVHypoToolFromDict )


