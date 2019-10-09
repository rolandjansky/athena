#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from TrigT2CaloCommon.CaloDef import HLTLCTopoRecoSequence
from TrigEDMConfig.TriggerEDMRun3 import recordable
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool


def _algoTauRoiUpdater(inputRoIs, clusters):
    from TrigTauHypo.TrigTauHypoConf import TrigTauCaloRoiUpdaterMT
    algo = TrigTauCaloRoiUpdaterMT("TauCaloRoiUpdater")
    algo.RoIInputKey  = inputRoIs
    algo.RoIOutputKey = recordable("HLT_RoiForTau")
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

def _algoTauTrackRoiUpdater(inputRoIs, tracks):
    from TrigTauHypo.TrigTauHypoConf import TrigTauTrackRoiUpdaterMT
    algo = TrigTauTrackRoiUpdaterMT("TrackRoiUpdater")
    algo.RoIInputKey   = inputRoIs
    algo.RoIOutputKey  = recordable("HLT_RoiForID2")
    algo.fastTracksKey = tracks
    return algo

def _algoTauPrecisionMVA(inputRoIs, tracks):
    from TrigTauRec.TrigTauRecConfigMT import TrigTauRecMerged_TauPrecisionMVA
    algo = TrigTauRecMerged_TauPrecisionMVA(doMVATES=True, doTrackBDT=False, doRNN=True)
    algo.RoIInputKey = inputRoIs
    algo.L1RoIKey    = "TAUCaloRoIs"
    algo.TrigTauRecOutputKey = recordable("HLT_TrigTauRecMerged_MVA")
    algo.TrigTauJet = "HLT_TrigTauRecMerged"
    algo.Key_trackPartInputContainer = tracks
    algo.UseCaloClusters = False
    #algo.clustersKey = "caloclusters"
    algo.Key_vertexInputContainer = "VxPrimaryCandidate"
    algo.TrigTauTrkOutputKey = recordable("HLT_tautrack_MVA")
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

def tauCoreTrackSequence( RoIs, name ):
    import AthenaCommon.CfgMgr as CfgMgr

    tauCoreTrackSequence = parOR(name)

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
       tauCoreTrackSequence += viewAlg
       #print viewAlg.name()
       if "RoIs" in viewAlg.properties():
         viewAlg.RoIs = RoIs
       if "roiCollectionName" in viewAlg.properties():
         viewAlg.roiCollectionName = RoIs
       if "TrackRoiUpdater" in viewAlg.name():
         viewAlg.RoIInputKey = RoIs
       if "TrigFastTrackFinder" in  viewAlg.name():
         theFTF_name = viewAlg.getName()

    sequenceOut = TrackCollection

    return tauCoreTrackSequence, sequenceOut

def tauIsoTrackSequence( RoIs , name):
    import AthenaCommon.CfgMgr as CfgMgr

    tauIsoTrackSequence = parOR(name)

    from TriggerMenuMT.HLTMenuConfig.CommonSequences.InDetSetup import makeInDetAlgs
    (viewAlgs, eventAlgs) = makeInDetAlgs(whichSignature='TauIso',separateTrackParticleCreator="_TauIso")

    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("tauViewDataVerifierIsoFTF")
    ViewVerify.DataObjects = [('xAOD::TauJetContainer','StoreGateSvc+HLT_TrigTauRecMerged')]
    viewAlgs.append(ViewVerify)

    global theFTFIso_name

    for viewAlg in viewAlgs:
       tauIsoTrackSequence += viewAlg
       #print viewAlg.name()
       if "RoIs" in viewAlg.properties():
         viewAlg.RoIs = RoIs
       if "roiCollectionName" in viewAlg.properties():
         viewAlg.roiCollectionName = RoIs
       if "TrackRoiUpdater" in viewAlg.name():
         viewAlg.RoIInputKey = RoIs
       #if "InDetTrigTrackParticleCreatorAlg" in viewAlg.name():
       #  TrackCollection = viewAlg.TrackParticlesName
       if "TrigFastTrackFinder" in  viewAlg.name():
         TrackCollection = viewAlg.TracksName


    #Precision Tracking
    PTAlgs = [] #List of precision tracking algs
    PTTracks = [] #List of TrackCollectionKeys
    PTTrackParticles = [] #List of TrackParticleKeys

    from TrigUpgradeTest.InDetPT import makeInDetPrecisionTracking
    #When run in a different view than FTF some data dependencies needs to be loaded through verifier
    #Pass verifier as an argument and it will automatically append necessary DataObjects
    #@NOTE: Don't provide any verifier if loaded in the same view as FTF

    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( "taus",  ViewVerify, inputFTFtracks= TrackCollection )
    PTSeq = seqAND("precisionTrackingInTaus", PTAlgs  )
    #Get last tracks from the list as input for other alg

    tauIsoTrackSequence += PTSeq

    #Default from FTF
    #trackParticles = "xAODTracks"
    #TODO: change according to what needs to be done here
    #Last key in the list is for the TrackParticles after all PT stages (so far only one :) )
    trackParticles = PTTrackParticles[-1]

    tauPrecisionMVAAlg = _algoTauPrecisionMVA(inputRoIs = RoIs, tracks = trackParticles)

    tauIsoTrackSequence += tauPrecisionMVAAlg

    sequenceOut = tauPrecisionMVAAlg.TrigTauRecOutputKey

    print "Here_too: ", sequenceOut

    return tauIsoTrackSequence, sequenceOut

def tauFTFCoreSequence(ConfigFlags):

    RecoSequenceName = "tauFTFCoreInViewSequence"

    ftfCoreViewsMaker                   = EventViewCreatorAlgorithm("IMFTFCore")
    ftfCoreViewsMaker.RoIsLink          = "roi" # -||-
    ftfCoreViewsMaker.InViewRoIs        = "TCoreViewRoIs" # contract with the fastCalo
    ftfCoreViewsMaker.Views             = "TAUFTFCoreViews"
    ftfCoreViewsMaker.ViewFallThrough   = True
    ftfCoreViewsMaker.RequireParentView = True
    ftfCoreViewsMaker.ViewNodeName      = RecoSequenceName

    (tauFTFCoreInViewSequence, sequenceOut) = tauCoreTrackSequence( ftfCoreViewsMaker.InViewRoIs, RecoSequenceName)

    tauFastTrackCoreSequence = seqAND("tauFastTrackCoreSequence", [ftfCoreViewsMaker, tauFTFCoreInViewSequence ])
    return (tauFastTrackCoreSequence, ftfCoreViewsMaker, sequenceOut)

def tauFTFIsoSequence(ConfigFlags):

    RecoSequenceName = "tauFTFIsoInViewSequence"

    ftfIsoViewsMaker                   = EventViewCreatorAlgorithm("IMFTFIso")
    ftfIsoViewsMaker.RoIsLink          = "roi" # -||-                                                                            
    ftfIsoViewsMaker.InViewRoIs        = "TIsoViewRoIs" # contract with the fast track core
    ftfIsoViewsMaker.Views             = "TAUFTFIsoViews"
    ftfIsoViewsMaker.ViewFallThrough   = True
    ftfIsoViewsMaker.RequireParentView = True
    ftfIsoViewsMaker.ViewNodeName      = RecoSequenceName

    (tauFTFIsoInViewSequence, sequenceOut) = tauIsoTrackSequence( ftfIsoViewsMaker.InViewRoIs, RecoSequenceName)

    tauFastTrackIsoSequence = seqAND("tauFastTrackIsoSequence", [ftfIsoViewsMaker, tauFTFIsoInViewSequence ])
    return (tauFastTrackIsoSequence, ftfIsoViewsMaker, sequenceOut)
