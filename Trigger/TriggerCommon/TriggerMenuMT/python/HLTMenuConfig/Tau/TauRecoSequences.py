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
    algo                               = TrigTauCaloRoiUpdaterMT("TauCaloRoiUpdater")
    algo.RoIInputKey                   = inputRoIs
    algo.RoIOutputKey                  = "HLT_RoiForTau"
    algo.CaloClustersKey               = clusters
    return algo

def _algoTauCaloOnly(inputRoIs, clusters):
    from TrigTauRec.TrigTauRecConfigMT import TrigTauRecMerged_TauCaloOnly
    roiUpdateAlgo                      = _algoTauRoiUpdater(inputRoIs, clusters)
    algo                               = TrigTauRecMerged_TauCaloOnly()
    algo.RoIInputKey                   = inputRoIs
    algo.clustersKey                   = clusters
    algo.L1RoIKey                      = roiUpdateAlgo.RoIInputKey
    algo.Key_vertexInputContainer      = ""
    algo.Key_trackPartInputContainer   = ""
    algo.Key_trigTauJetInputContainer  = ""
    algo.Key_trigTauTrackInputContainer  = ""
    algo.Key_trigTauJetOutputContainer = recordable("HLT_TrigTauRecMerged_CaloOnly")
    algo.Key_trigTauTrackOutputContainer = recordable("HLT_tautrack_dummy")
    return algo

def _algoTauCaloOnlyMVA(inputRoIs, clusters):
    from TrigTauRec.TrigTauRecConfigMT import TrigTauRecMerged_TauCaloOnlyMVA
    roiUpdateAlgo                      = _algoTauRoiUpdater(inputRoIs, clusters)
    algo                               = TrigTauRecMerged_TauCaloOnlyMVA()
    algo.RoIInputKey                   = inputRoIs
    algo.L1RoIKey                      = roiUpdateAlgo.RoIInputKey
    algo.clustersKey                   = clusters
    algo.Key_vertexInputContainer      = ""
    algo.Key_trackPartInputContainer   = ""
    algo.Key_trigTauJetInputContainer  = ""
    algo.Key_trigTauTrackInputContainer  = ""
    algo.Key_trigTauJetOutputContainer = recordable("HLT_TrigTauRecMerged_CaloOnlyMVA")
    algo.Key_trigTauTrackOutputContainer = recordable("HLT_tautrack_dummy")
    return algo

def _algoTauTrackRoiUpdater(inputRoIs, tracks):
    from TrigTauHypo.TrigTauHypoConf import TrigTauTrackRoiUpdaterMT
    algo                               = TrigTauTrackRoiUpdaterMT("TrackRoiUpdater")
    algo.RoIInputKey                   = inputRoIs
    algo.RoIOutputKey                  = "HLT_RoiForID2"
    algo.fastTracksKey                 = tracks
    return algo

def _algoTauPreselection(inputRoIs, tracks):
    from TrigTauRec.TrigTauRecConfigMT import TrigTauRecMerged_TauPreselection
    algo                                 = TrigTauRecMerged_TauPreselection()
    algo.RoIInputKey                     = inputRoIs
    algo.L1RoIKey                        = "TAUCaloRoIs"
    algo.clustersKey                     = ""
    algo.Key_vertexInputContainer        = ""
    algo.Key_trigTauJetInputContainer    = "HLT_TrigTauRecMerged_CaloOnlyMVA"
    algo.Key_trackPartInputContainer     = tracks
    algo.Key_trigTauTrackInputContainer  = "HLT_tautrack_dummy"
    algo.Key_trigTauJetOutputContainer   = recordable("HLT_TrigTauRecMerged_Presel")
    algo.Key_trigTauTrackOutputContainer = recordable("HLT_tautrack_Presel")
    return algo

def _algoTauPrecision(inputRoIs, tracks, step):
    from TrigTauRec.TrigTauRecConfigMT import TrigTauRecMerged_TauPrecision
    algo                                 = TrigTauRecMerged_TauPrecision(name= "TrigTauRecMerged_TauPrecision_"+step)
    algo.RoIInputKey                     = inputRoIs
    algo.L1RoIKey                        = "TAUCaloRoIs"
    algo.clustersKey                     = ""
    algo.Key_vertexInputContainer        = ""
    algo.Key_trigTauJetInputContainer    = "HLT_TrigTauRecMerged_CaloOnlyMVA"
    algo.Key_trackPartInputContainer     = tracks
    if "Id" in step:
       algo.Key_trigTauTrackInputContainer  = "HLT_tautrack_dummy"
    elif "Track" in step:
       algo.Key_trigTauTrackInputContainer  = "HLT_tautrack_Presel"
    algo.Key_trigTauJetOutputContainer   = recordable("HLT_TrigTauRecMerged_Precision")   
    algo.Key_trigTauTrackOutputContainer = recordable("HLT_tautrack_Precision")
    return algo

def _algoTauPrecisionMVA(inputRoIs, tracks):
    from TrigTauRec.TrigTauRecConfigMT import TrigTauRecMerged_TauPrecisionMVA
    algo                                 = TrigTauRecMerged_TauPrecisionMVA(doMVATES=True, doTrackBDT=False, doRNN=True)
    algo.RoIInputKey                     = inputRoIs
    algo.L1RoIKey                        = "TAUCaloRoIs"
    algo.clustersKey                     = ""
    algo.Key_vertexInputContainer        = ""
    algo.Key_trigTauJetInputContainer    = "HLT_TrigTauRecMerged_CaloOnlyMVA"
    algo.Key_trackPartInputContainer     = tracks
    algo.Key_trigTauTrackInputContainer  = "HLT_tautrack_dummy"
    algo.Key_trigTauJetOutputContainer   = recordable("HLT_TrigTauRecMerged_MVA")
    algo.Key_trigTauTrackOutputContainer = recordable("HLT_tautrack_MVA")
    return algo

def tauCaloRecoSequence(InViewRoIs, SeqName):
    global TauCaloJetContainer
    # lc sequence
    (lcTopoInViewSequence, lcCaloSequenceOut) = RecoFragmentsPool.retrieve(HLTLCTopoRecoSequence, InViewRoIs)
    tauCaloRoiUpdaterAlg                      = _algoTauRoiUpdater(inputRoIs = InViewRoIs, clusters = lcCaloSequenceOut)
    tauCaloOnlyAlg                            = _algoTauCaloOnly(inputRoIs   = InViewRoIs, clusters = lcCaloSequenceOut)
    RecoSequence                              = parOR( SeqName, [lcTopoInViewSequence,tauCaloRoiUpdaterAlg,tauCaloOnlyAlg] )
    return (RecoSequence, tauCaloOnlyAlg.Key_trigTauJetOutputContainer)

def tauCaloMVARecoSequence(InViewRoIs, SeqName):
    global TauCaloJetContainer
    # lc sequence
    (lcTopoInViewSequence, lcCaloSequenceOut) = RecoFragmentsPool.retrieve(HLTLCTopoRecoSequence, InViewRoIs)
    tauCaloRoiUpdaterAlg                      = _algoTauRoiUpdater(inputRoIs = InViewRoIs, clusters = lcCaloSequenceOut)
    tauCaloOnlyMVAAlg	                      = _algoTauCaloOnlyMVA(inputRoIs   = InViewRoIs, clusters = lcCaloSequenceOut)
    RecoSequence                              = parOR( SeqName, [lcTopoInViewSequence,tauCaloRoiUpdaterAlg,tauCaloOnlyMVAAlg] )
    return (RecoSequence, tauCaloOnlyMVAAlg.Key_trigTauJetOutputContainer)

def tauCaloSequence(ConfigFlags):
    """ Creates L2 Fast Calo sequence for Taus"""
    # EV creator
    InViewRoIs                           = "TAUCaloRoIs"
    RecoSequenceName                     = "tauCaloInViewSequence"

    tauCaloViewsMaker                    = EventViewCreatorAlgorithm( "IMtauCalo")
    tauCaloViewsMaker.ViewFallThrough    = True
    tauCaloViewsMaker.RoIsLink           = "initialRoI"
    tauCaloViewsMaker.InViewRoIs         = InViewRoIs
    tauCaloViewsMaker.Views              = "TAUCaloViews"
    tauCaloViewsMaker.ViewNodeName       = RecoSequenceName
    (tauCaloInViewSequence, sequenceOut) = tauCaloRecoSequence( InViewRoIs, RecoSequenceName)

    tauCaloSequence = seqAND("tauCaloSequence", [tauCaloViewsMaker, tauCaloInViewSequence ])
    return (tauCaloSequence, tauCaloViewsMaker, sequenceOut)    

def tauCaloMVASequence(ConfigFlags):
    """ Creates L2 Fast Calo sequence for Taus"""
    # EV creator
    InViewRoIs                              = "TAUCaloRoIs"
    RecoSequenceName                        = "tauCaloMVAInViewSequence"

    tauCaloMVAViewsMaker                    = EventViewCreatorAlgorithm( "IMtauCaloMVA")
    tauCaloMVAViewsMaker.ViewFallThrough    = True
    tauCaloMVAViewsMaker.RoIsLink           = "initialRoI"
    tauCaloMVAViewsMaker.InViewRoIs         = InViewRoIs
    tauCaloMVAViewsMaker.Views              = "TAUCaloMVAViews"
    tauCaloMVAViewsMaker.ViewNodeName       = RecoSequenceName
    (tauCaloMVAInViewSequence, sequenceOut) = tauCaloMVARecoSequence(InViewRoIs, RecoSequenceName)

    tauCaloMVASequence = seqAND("tauCaloMVASequence", [tauCaloMVAViewsMaker, tauCaloMVAInViewSequence ])
    return (tauCaloMVASequence, tauCaloMVAViewsMaker, sequenceOut)

def tauIdTrackSequence( RoIs , name):
    import AthenaCommon.CfgMgr as CfgMgr

    tauIdTrackSequence = parOR(name)

    signName = "Tau"

    if "FTFIso" in name:
      signName = 'TauIso'

    from TrigInDetConfig.InDetSetup import makeInDetAlgs
    viewAlgs = makeInDetAlgs(whichSignature=signName,separateTrackParticleCreator="_"+signName,rois = RoIs)

    tauViewDataVerifierName = ""
    if "FTFId" in name:
      tauViewDataVerifierName = "tauViewDataVerifierIdFTF"
    elif "FTFTrack" in name:
      tauViewDataVerifierName = "tauViewDataVerifierTrackFTF"
    elif "FTFIso" in name:
      tauViewDataVerifierName = "tauViewDataVerifierIsoFTF"

    ViewVerify = CfgMgr.AthViews__ViewDataVerifier(tauViewDataVerifierName)
    ViewVerify.DataObjects = [('xAOD::TauJetContainer','StoreGateSvc+HLT_TrigTauRecMerged_CaloOnlyMVA'),('xAOD::TauTrackContainer','StoreGateSvc+HLT_tautrack_dummy')]
    viewAlgs.append(ViewVerify)

    for viewAlg in viewAlgs:
       tauIdTrackSequence += viewAlg
       if "RoIs" in viewAlg.properties():
         viewAlg.RoIs = RoIs
       if "roiCollectionName" in viewAlg.properties():
         viewAlg.roiCollectionName = RoIs
       if "TrackRoiUpdater" in viewAlg.name():
         viewAlg.RoIInputKey = RoIs
       if "TrigFastTrackFinder" in  viewAlg.name():
         TrackCollection = viewAlg.TracksName
       if "InDetTrigTrackParticleCreatorAlg" in viewAlg.name():          
         TrackParticlesName = viewAlg.TrackParticlesName

    if "FTFTrack" in name:
      tauPreselectionAlg = _algoTauPreselection(inputRoIs = RoIs, tracks = TrackParticlesName)
      tauIdTrackSequence += tauPreselectionAlg

    #Precision Tracking
    PTAlgs = [] #List of precision tracking algs 
    PTTracks = [] #List of TrackCollectionKeys
    PTTrackParticles = [] #List of TrackParticleKeys
    
    from TrigUpgradeTest.InDetPT import makeInDetPrecisionTracking
    #When run in a different view than FTF some data dependencies needs to be loaded through verifier
    #Pass verifier as an argument and it will automatically append necessary DataObjects@NOTE: Don't provide any verifier if loaded in the same view as FTF

    precName = ""
    if "FTFId" in name:
      precName = 'tauId'
    elif "FTFTrack" in name:
      precName = 'tauTrk'
    elif "FTFIso" in name:
      precName = 'tau'

    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( precName,  False, inputFTFtracks= TrackCollection )
    PTSeq = seqAND("precisionTrackingIn"+precName, PTAlgs  )

    #Get last tracks from the list as input for other alg       
    tauIdTrackSequence += PTSeq
    trackParticles = PTTrackParticles[-1]

    if "FTFId" in name:
      tauPrecisionAlg = _algoTauPrecision(inputRoIs = RoIs, tracks = trackParticles, step = "Id")
    elif "FTFTrack" in name:
      tauPrecisionAlg = _algoTauPrecision(inputRoIs = RoIs, tracks = trackParticles, step = "Track")
    elif "FTFIso" in name:
      tauPrecisionAlg = _algoTauPrecisionMVA(inputRoIs = RoIs, tracks = trackParticles)

    tauIdTrackSequence += tauPrecisionAlg
    sequenceOut = tauPrecisionAlg.Key_trigTauJetOutputContainer

    return tauIdTrackSequence, sequenceOut

def tauCoreTrackSequence( RoIs, name ):
    import AthenaCommon.CfgMgr as CfgMgr

    tauCoreTrackSequence = seqAND(name)

    from TrigInDetConfig.InDetSetup import makeInDetAlgs
    viewAlgs = makeInDetAlgs(whichSignature='TauCore',separateTrackParticleCreator="_TauCore",rois = RoIs)

    for viewAlg in viewAlgs:
       if "InDetTrigTrackParticleCreatorAlg" in viewAlg.name():
         TrackCollection = viewAlg.TrackName

    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("tauViewDataVerifierCoreFTF")
    ViewVerify.DataObjects = [('xAOD::TauJetContainer','StoreGateSvc+HLT_TrigTauRecMerged_CaloOnlyMVA')]
    viewAlgs.append(ViewVerify)

    tauTrackRoiUpdaterAlg = _algoTauTrackRoiUpdater(inputRoIs = RoIs, tracks = TrackCollection)

    viewAlgs.append(tauTrackRoiUpdaterAlg)

    for viewAlg in viewAlgs:
       tauCoreTrackSequence += viewAlg
       if "RoIs" in viewAlg.properties():
         viewAlg.RoIs = RoIs
       if "roiCollectionName" in viewAlg.properties():
         viewAlg.roiCollectionName = RoIs
       if "TrackRoiUpdater" in viewAlg.name():
         viewAlg.RoIInputKey = RoIs

    sequenceOut = TrackCollection

    return tauCoreTrackSequence, sequenceOut

def tauFTFTrackSequence(ConfigFlags):

    RecoSequenceName = "tauFTFTrackInViewSequence"

    ftfTrackViewsMaker                   = EventViewCreatorAlgorithm("IMFTFTrack")
    ftfTrackViewsMaker.RoIsLink          = "roi" # -||-
    ftfTrackViewsMaker.InViewRoIs        = "TIdViewRoIs" # contract with the fast track core
    ftfTrackViewsMaker.Views             = "TAUFTFTrackViews"
    ftfTrackViewsMaker.ViewFallThrough   = True
    ftfTrackViewsMaker.RequireParentView = True
    ftfTrackViewsMaker.ViewNodeName      = RecoSequenceName

    (tauFTFTrackInViewSequence, sequenceOut) = tauIdTrackSequence( ftfTrackViewsMaker.InViewRoIs, RecoSequenceName)

    tauFastTrackSequence = seqAND("tauFastTrackSequence", [ftfTrackViewsMaker, tauFTFTrackInViewSequence ])
    return (tauFastTrackSequence, ftfTrackViewsMaker, sequenceOut)

def tauFTFIdSequence(ConfigFlags):

    RecoSequenceName = "tauFTFIdInViewSequence"

    ftfIdViewsMaker                   = EventViewCreatorAlgorithm("IMFTFId")
    ftfIdViewsMaker.RoIsLink          = "roi" # -||-                                                                                          
    ftfIdViewsMaker.InViewRoIs        = "TIdViewRoIs" # contract with the fast track core                                                    
    ftfIdViewsMaker.Views             = "TAUFTFIdViews"
    ftfIdViewsMaker.ViewFallThrough   = True
    ftfIdViewsMaker.RequireParentView = True
    ftfIdViewsMaker.ViewNodeName      = RecoSequenceName

    (tauFTFIdInViewSequence, sequenceOut) = tauIdTrackSequence( ftfIdViewsMaker.InViewRoIs, RecoSequenceName)

    tauFastTrackIdSequence = seqAND("tauFastTrackIdSequence", [ftfIdViewsMaker, tauFTFIdInViewSequence ])
    return (tauFastTrackIdSequence, ftfIdViewsMaker, sequenceOut)

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

    (tauFTFIsoInViewSequence, sequenceOut) = tauIdTrackSequence( ftfIsoViewsMaker.InViewRoIs, RecoSequenceName)

    tauFastTrackIsoSequence = seqAND("tauFastTrackIsoSequence", [ftfIsoViewsMaker, tauFTFIsoInViewSequence ])
    return (tauFastTrackIsoSequence, ftfIsoViewsMaker, sequenceOut)
