#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR, seqAND, seqOR
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from TrigT2CaloCommon.CaloDef import HLTLCTopoRecoSequence
from TrigEDMConfig.TriggerEDMRun3 import recordable
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool


def _algoTauRoiUpdater(inputRoIs, clusters):
    from TrigTauHypo.TrigTauHypoConf import TrigTauCaloRoiUpdaterMT
    algo = TrigTauCaloRoiUpdaterMT("TauCaloRoiUpdater")
    algo.RoIInputKey  = inputRoIs
    algo.RoIOutputKey = "RoiForTau"
    algo.CaloClustersKey = clusters
    return algo

def _algoTauCaloOnly(inputRoIs, clusters):
    from TrigTauRec.TrigTauRecConfigMT import TrigTauRecMerged_TauCaloOnly
    roiUpdateAlgo = _algoTauRoiUpdater(inputRoIs, clusters)
    algo = TrigTauRecMerged_TauCaloOnly()
    algo.RoIInputKey         = inputRoIs
    algo.clustersKey         = clusters
    algo.L1RoIKey            = roiUpdateAlgo.RoIInputKey
    algo.TrigTauRecOutputKey = recordable("HLT_TrigTauRecMerged")
    return algo

def _algoTauCaloOnlyMVA(inputRoIs, clusters):
    from TrigTauRec.TrigTauRecConfigMT import TrigTauRecMerged_TauCaloOnlyMVA
    roiUpdateAlgo = _algoTauRoiUpdater(inputRoIs, clusters)
    algo = TrigTauRecMerged_TauCaloOnlyMVA()
    algo.RoIInputKey         = inputRoIs
    algo.L1RoIKey            = roiUpdateAlgo.RoIInputKey
    algo.clustersKey         = clusters
    algo.TrigTauRecOutputKey = recordable("HLT_TrigTauRecMerged")
    return algo

def tauCaloRecoSequence(InViewRoIs, SeqName):
    # lc sequence
    (lcTopoInViewSequence, lcCaloSequenceOut) = RecoFragmentsPool.retrieve(HLTLCTopoRecoSequence, InViewRoIs)
    tauCaloRoiUpdaterAlg = _algoTauRoiUpdater(inputRoIs = InViewRoIs, clusters = lcCaloSequenceOut)
    tauCaloOnlyAlg       = _algoTauCaloOnly(inputRoIs   = InViewRoIs, clusters = lcCaloSequenceOut)
    RecoSequence = parOR( SeqName, [lcTopoInViewSequence,tauCaloRoiUpdaterAlg,tauCaloOnlyAlg] )
    return (RecoSequence, tauCaloOnlyAlg.TrigTauRecOutputKey)

def tauCaloMVARecoSequence(InViewRoIs, SeqName):
    # lc sequence
    (lcTopoInViewSequence, lcCaloSequenceOut) = RecoFragmentsPool.retrieve(HLTLCTopoRecoSequence, InViewRoIs)
    tauCaloRoiUpdaterAlg = _algoTauRoiUpdater(inputRoIs = InViewRoIs, clusters = lcCaloSequenceOut)
    tauCaloOnlyMVAAlg	 = _algoTauCaloOnlyMVA(inputRoIs   = InViewRoIs, clusters = lcCaloSequenceOut)
    RecoSequence = parOR( SeqName, [lcTopoInViewSequence,tauCaloRoiUpdaterAlg,tauCaloOnlyMVAAlg] )
    return (RecoSequence, tauCaloOnlyMVAAlg.TrigTauRecOutputKey)

def tauCaloSequence(ConfigFlags):
    """ Creates L2 Fast Calo sequence for Taus"""
    # EV creator
    InViewRoIs="TAUCaloRoIs"
    RecoSequenceName="tauCaloInViewSequence"

    tauCaloViewsMaker = EventViewCreatorAlgorithm( "IMtauCalo")
    tauCaloViewsMaker.ViewFallThrough = True
    tauCaloViewsMaker.RoIsLink = "initialRoI"
    tauCaloViewsMaker.InViewRoIs = InViewRoIs
    tauCaloViewsMaker.Views = "TAUCaloViews"
    tauCaloViewsMaker.ViewNodeName = RecoSequenceName
    (tauCaloInViewSequence, sequenceOut) = tauCaloRecoSequence( InViewRoIs, RecoSequenceName)

    tauCaloSequence = seqAND("tauCaloSequence", [tauCaloViewsMaker, tauCaloInViewSequence ])
    return (tauCaloSequence, tauCaloViewsMaker, sequenceOut)    

def tauCaloMVASequence(ConfigFlags):
    """ Creates L2 Fast Calo sequence for Taus"""
    # EV creator
    InViewRoIs="TAUCaloRoIs"
    RecoSequenceName="tauCaloMVAInViewSequence"

    tauCaloMVAViewsMaker = EventViewCreatorAlgorithm( "IMtauCaloMVA")
    tauCaloMVAViewsMaker.ViewFallThrough = True
    tauCaloMVAViewsMaker.RoIsLink = "initialRoI"
    tauCaloMVAViewsMaker.InViewRoIs = InViewRoIs
    tauCaloMVAViewsMaker.Views = "TAUCaloMVAViews"
    tauCaloMVAViewsMaker.ViewNodeName = RecoSequenceName
    (tauCaloMVAInViewSequence, sequenceOut) = tauCaloMVARecoSequence(InViewRoIs, RecoSequenceName)

    tauCaloMVASequence = seqAND("tauCaloMVASequence", [tauCaloMVAViewsMaker, tauCaloMVAInViewSequence ])
    return (tauCaloMVASequence, tauCaloMVAViewsMaker, sequenceOut)

def tauFTFCoreSequence( RoIs, name ):
    import AthenaCommon.CfgMgr as CfgMgr

    tauFTFCoreSequence = parOR(name)

    from TriggerMenuMT.HLTMenuConfig.CommonSequences.InDetSetup import makeInDetAlgs
    (viewAlgs, eventAlgs) = makeInDetAlgs(whichSignature='TauCore',separateTrackParticleCreator="_TauCore")

    for viewAlg in viewAlgs:
       if "InDetTrigTrackParticleCreatorAlg" in viewAlg.name():
         TrackCollection = viewAlg.TrackName

    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("tauViewDataVerifierCoreFTF")
    ViewVerify.DataObjects = [('xAOD::TauJetContainer','StoreGateSvc+HLT_TrigTauRecMerged')]
    viewAlgs.append(ViewVerify)

    tauTrackRoiUpdaterAlg = _algoTauTrackRoiUpdater(inputRoIs = RoIs, tracks = TrackCollection)

    viewAlgs.append(tauTrackRoiUpdaterAlg)

    global TrackParticlesName
    global theFTF_name

    for viewAlg in viewAlgs:
       tauFTFCoreSequence += viewAlg
       #print viewAlg.name()
       if "RoIs" in viewAlg.properties():
         viewAlg.RoIs = RoIs
       if "roiCollectionName" in viewAlg.properties():
         viewAlg.roiCollectionName = RoIs
       if "TrackRoiUpdater" in viewAlg.name():
         viewAlg.RoIInputKey = RoIs
       if "InDetTrigTrackParticleCreatorAlg" in viewAlg.name():
         TrackParticlesName = viewAlg.TrackParticlesName
       if "TrigFastTrackFinder" in  viewAlg.name():
         theFTF_name = viewAlg.getName()

    sequenceOut = TrackParticlesName

    return tauFTFCoreSequence, sequenceOut

def tauFTFIsoSequence( RoIs , name):
    import AthenaCommon.CfgMgr as CfgMgr

    tauFTFIsoSequence = parOR("FTFIsoViewNode")

    from TriggerMenuMT.HLTMenuConfig.CommonSequences.InDetSetup import makeInDetAlgs
    (viewAlgs, eventAlgs) = makeInDetAlgs(whichSignature='TauIso',separateTrackParticleCreator="_TauIso")

    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("tauViewDataVerifierCoreFTF")
    ViewVerify.DataObjects = [('xAOD::TauJetContainer','StoreGateSvc+HLT_TrigTauRecMerged')]
    viewAlgs.append(ViewVerify)

    global TrackParticlesNameIso
    global theFTFIso_name

    for viewAlg in viewAlgs:
       tauFTFIsoSequence += viewAlg
       #print viewAlg.name()
       if "RoIs" in viewAlg.properties():
         viewAlg.RoIs = RoIs
       if "roiCollectionName" in viewAlg.properties():
         viewAlg.roiCollectionName = RoIs
       if "TrackRoiUpdater" in viewAlg.name():
         viewAlg.RoIInputKey = RoIs
       if "InDetTrigTrackParticleCreatorAlg" in viewAlg.name():
         TrackParticlesNameIso = viewAlg.TrackParticlesName
       if "TrigFastTrackFinder" in  viewAlg.name():
         theFTFIso_name = viewAlg.getName()

    sequenceOut = TrackParticlesNameIso

    return (tauFTFIsoSequence, sequenceOut)

def tauFTFSequence(ConfigFlags):

    RecoSequenceName = "tauFTFCoreInViewSequence"

    ftfCoreViewsMaker                   = EventViewCreatorAlgorithm("IMFTFCore")
    ftfCoreViewsMaker.RoIsLink          = "roi" # -||-
    ftfCoreViewsMaker.InViewRoIs        = "FTFCoreViewRoIs" # contract with the fastCalo
    ftfCoreViewsMaker.Views             = "TAUFTFCoreViews"
    ftfCoreViewsMaker.ViewFallThrough   = True
    ftfCoreViewsMaker.RequireParentView = True
    ftfCoreViewsMaker.ViewNodeName      = RecoSequenceName

    #tauFTFCoreRecoSequence, sequenceOutCore = tauFTFCoreSequence( ftfCoreViewsMaker.InViewRoIs, "RoIs")

    (tauFTFCoreInViewSequence, sequenceOut) = tauFTFCoreSequence( ftfCoreViewsMaker.InViewRoIs, RecoSequenceName)

    tauFastTrackCoreSequence = seqAND("tauFastTrackCoreSequence", [ftfCoreViewsMaker, tauFTFCoreInViewSequence ])
    return (tauFastTrackCoreSequence, ftfCoreViewsMaker, sequenceOut)



####    ftfIsoViewsMaker                   = EventViewCreatorAlgorithm("IMFTFIso")
####    ftfIsoViewsMaker.RoIsLink          = "roi" # -||-
####    ftfIsoViewsMaker.InViewRoIs        = "FTFIsoViewRoIs" # contract with the fastCalo
####    ftfIsoViewsMaker.Views             = "TAUFTFIsoViews"
####    ftfIsoViewsMaker.ViewFallThrough   = True
####    ftfIsoViewsMaker.RequireParentView = True
####
####    tauFTFIsoRecoSequence  = tauFTFIsoSequence( ftfIsoViewsMaker.InViewRoIs, "RoIs")
####
####    tauTrackRecoSequence, sequenceOutIso = seqOR("tauTrackRecoSequence", [tauFTFIsoRecoSequence, tauFTFCoreRecoSequence])
####
####    tauSequence = seqAND("tauTrackRecoSequence", [ftfIsoViewsMaker, tauTrackRecoSequence])
####
####    return (tauSequence, ftfIsoViewsMaker, sequenceOutIso)
####
####    (tauCaloInViewSequence, sequenceOut) = tauCaloRecoSequence( InViewRoIs, RecoSequenceName)
####
####    tauCaloSequence = seqAND("tauCaloSequence", [tauCaloViewsMaker, tauCaloInViewSequence ])
####    return (tauCaloSequence, tauCaloViewsMaker, sequenceOut)    """
####
