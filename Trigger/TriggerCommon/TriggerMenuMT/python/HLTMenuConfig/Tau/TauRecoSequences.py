#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm, ViewCreatorInitialROITool, ViewCreatorPreviousROITool
from TrigT2CaloCommon.CaloDef import HLTLCTopoRecoSequence
from TrigEDMConfig.TriggerEDMRun3 import recordable
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool
import AthenaCommon.CfgMgr as CfgMgr

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
    algo.Key_trigTauTrackOutputContainer = "HLT_tautrack_dummy"
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
    algo.Key_trigTauTrackOutputContainer = "HLT_tautrack_dummy"
    return algo

def _algoTauTrackRoiUpdater(inputRoIs, tracks):
    from TrigTauHypo.TrigTauHypoConf import TrigTauTrackRoiUpdaterMT
    algo                               = TrigTauTrackRoiUpdaterMT("TrackRoiUpdater")
    algo.RoIInputKey                   = inputRoIs
    algo.RoIOutputKey                  = "HLT_RoiForID2"
    algo.fastTracksKey                 = tracks
    return algo

def _algoTauPreselection(inputRoIs, tracks, step):
    from TrigTauRec.TrigTauRecConfigMT import TrigTauRecMerged_TauPreselection
    algo                                 = TrigTauRecMerged_TauPreselection(name= "TrigTauRecMerged_TauPreselection_"+step)
    algo.RoIInputKey                     = inputRoIs
    algo.L1RoIKey                        = "TAUCaloRoIs"
    algo.clustersKey                     = ""
    algo.Key_vertexInputContainer        = ""
    algo.Key_trigTauJetInputContainer    = "HLT_TrigTauRecMerged_CaloOnly"
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
    algo.Key_trigTauJetInputContainer    = "HLT_TrigTauRecMerged_CaloOnly"
    algo.Key_trackPartInputContainer     = tracks
    if "Id" in step:
       algo.Key_trigTauTrackInputContainer  = "HLT_tautrack_dummy"
    elif "Track" in step:
       algo.Key_trigTauTrackInputContainer  = "HLT_tautrack_Presel"
    algo.Key_trigTauJetOutputContainer   = recordable("HLT_TrigTauRecMerged_Precision")   
    algo.Key_trigTauTrackOutputContainer = recordable("HLT_tautrack_Precision")
    return algo

def _algoTauPrecisionMVA(inputRoIs, tracks, step):
    from TrigTauRec.TrigTauRecConfigMT import TrigTauRecMerged_TauPrecisionMVA
    algo                                 = TrigTauRecMerged_TauPrecisionMVA(name= "TrigTauRecMerged_TauPrecisionMVA", doMVATES=True, doTrackBDT=False, doRNN=True)
    if "EF" in step:
       algo                              = TrigTauRecMerged_TauPrecisionMVA(name= "TrigTauRecMerged_TauPrecisionMVA"+step, doMVATES=False, doTrackBDT=False, doRNN=True)
    algo.RoIInputKey                     = inputRoIs
    algo.L1RoIKey                        = "TAUCaloRoIs"
    algo.clustersKey                     = ""
    algo.Key_vertexInputContainer        = ""
    algo.Key_trigTauJetInputContainer    = "HLT_TrigTauRecMerged_CaloOnlyMVA"
    if "EF" in step:
       algo.Key_trigTauJetInputContainer    = "HLT_TrigTauRecMerged_CaloOnly"
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
    tauCaloViewsMaker.RoITool            = ViewCreatorInitialROITool()
    tauCaloViewsMaker.InViewRoIs         = InViewRoIs
    tauCaloViewsMaker.Views              = "TAUCaloViews"
    tauCaloViewsMaker.ViewNodeName       = RecoSequenceName
    (tauCaloInViewSequence, sequenceOut) = tauCaloRecoSequence( InViewRoIs, RecoSequenceName)

    tauCaloRecoVDV = CfgMgr.AthViews__ViewDataVerifier( "tauCaloRecoVDV" )
    tauCaloRecoVDV.DataObjects = [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+TAUCaloRoIs' ),
                                     ( 'CaloBCIDAverage' , 'StoreGateSvc+CaloBCIDAverage' ),
                                     ( 'ILArHVScaleCorr' , 'ConditionStore+LArHVScaleCorrRecomputed' ),
                                     ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' )]
    tauCaloInViewSequence += tauCaloRecoVDV

    # Make sure the required objects are still available at whole-event level
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence.SGInputLoader.Load += [( 'ILArHVScaleCorr' , 'ConditionStore+LArHVScaleCorrRecomputed' )]
    if not hasattr( topSequence, "CaloBCIDAvgAlg" ):
      topSequence.SGInputLoader.Load += [( 'CaloBCIDAverage' , 'StoreGateSvc+CaloBCIDAverage' )]

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
    tauCaloMVAViewsMaker.RoITool            = ViewCreatorInitialROITool()
    tauCaloMVAViewsMaker.InViewRoIs         = InViewRoIs
    tauCaloMVAViewsMaker.Views              = "TAUCaloMVAViews"
    tauCaloMVAViewsMaker.ViewNodeName       = RecoSequenceName
    (tauCaloMVAInViewSequence, sequenceOut) = tauCaloMVARecoSequence(InViewRoIs, RecoSequenceName)

    tauCaloMVARecoVDV = CfgMgr.AthViews__ViewDataVerifier( "tauCaloMVARecoVDV" )
    tauCaloMVARecoVDV.DataObjects = [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+TAUCaloRoIs' ),
                                     ( 'CaloBCIDAverage' , 'StoreGateSvc+CaloBCIDAverage' ),
                                     ( 'ILArHVScaleCorr' , 'ConditionStore+LArHVScaleCorrRecomputed' ),
                                     ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' )]
    tauCaloMVAInViewSequence += tauCaloMVARecoVDV

    # Make sure the required objects are still available at whole-event level
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence.SGInputLoader.Load += [( 'ILArHVScaleCorr' , 'ConditionStore+LArHVScaleCorrRecomputed' )]
    if not hasattr( topSequence, "CaloBCIDAvgAlg" ):
      topSequence.SGInputLoader.Load += [( 'CaloBCIDAverage' , 'StoreGateSvc+CaloBCIDAverage' )]

    tauCaloMVASequence = seqAND("tauCaloMVASequence", [tauCaloMVAViewsMaker, tauCaloMVAInViewSequence ])
    return (tauCaloMVASequence, tauCaloMVAViewsMaker, sequenceOut)

def tauIdTrackSequence( RoIs , name):

    tauIdTrackSequence = seqAND(name)

    signName = "Tau"

    if ("Iso" in name) or ("TrackTwo" in name) or ("EF" in name):
      signName = 'TauIso'

    tauViewDataVerifierName = ""
    if "FTFId" in name:
      tauViewDataVerifierName = "tauViewDataVerifierIdFTF"
    elif "FTFTrackInView" in name:
      tauViewDataVerifierName = "tauViewDataVerifierTrackFTF"
    elif "FTFTrackTwo" in name:
      tauViewDataVerifierName = "tauViewDataVerifierTrackTwoFTF"
    elif "FTFIso" in name:
      tauViewDataVerifierName = "tauViewDataVerifierIsoFTF"
    elif "EF" in name:
      tauViewDataVerifierName = "tauViewDataVerifierEF"

    from TrigInDetConfig.InDetSetup import makeInDetAlgs
    viewAlgs, ViewVerify = makeInDetAlgs( whichSignature=signName, separateTrackParticleCreator = "_" + signName, rois = RoIs, viewVerifier = tauViewDataVerifierName )

    if "FTFIso" in name:
       ViewVerify.DataObjects += [( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloOnlyMVA' )]
    else:
       ViewVerify.DataObjects += [( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloOnly')]

    ViewVerify.DataObjects += [( 'xAOD::TauTrackContainer' , 'StoreGateSvc+HLT_tautrack_dummy' ),
                               ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+' + RoIs ),
                               ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                               ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+TAUCaloRoIs' ),
                               ( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloOnly' ),
                               ( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' )]

    # Make sure the required objects are still available at whole-event level
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence.SGInputLoader.Load += [( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' )]
    from IOVDbSvc.CondDB import conddb
    if not conddb.folderRequested( "PixelClustering/PixelClusNNCalib" ):
      topSequence.SGInputLoader.Load += [( 'TTrainedNetworkCollection' , 'ConditionStore+PixelClusterNN' ),
                                         ( 'TTrainedNetworkCollection' , 'ConditionStore+PixelClusterNNWithTrack' )]
      ViewVerify.DataObjects += [( 'TTrainedNetworkCollection' , 'ConditionStore+PixelClusterNN' ),
                                 ( 'TTrainedNetworkCollection' , 'ConditionStore+PixelClusterNNWithTrack' )]

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

    if "FTFTrackInView" in name:
      tauPreselectionAlg = _algoTauPreselection(inputRoIs = RoIs, tracks = TrackParticlesName, step = "Track")
      tauIdTrackSequence += tauPreselectionAlg
    elif "TrackTwo" in name:
      tauPreselectionAlg = _algoTauPreselection(inputRoIs = RoIs, tracks = TrackParticlesName, step = "TrackTwo")
      tauIdTrackSequence += tauPreselectionAlg      

    #Precision Tracking
    PTAlgs = [] #List of precision tracking algs 
    PTTracks = [] #List of TrackCollectionKeys
    PTTrackParticles = [] #List of TrackParticleKeys
    
    from TrigInDetConfig.InDetPT import makeInDetPrecisionTracking
    #When run in a different view than FTF some data dependencies needs to be loaded through verifier
    #Pass verifier as an argument and it will automatically append necessary DataObjects@NOTE: Don't provide any verifier if loaded in the same view as FTF

    precName = ""
    if "FTFId" in name:
      precName = 'tauId'
    elif "FTFTrackInView" in name:
      precName = 'tauTrk'
    elif "FTFTrackTwo" in name:
      precName = 'tauTrkTwo'
    elif "FTFIso" in name:
      precName = 'tau'
    elif "EF" in name:
      precName = 'tauEF'

    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( precName,  verifier = False, rois = RoIs, inputFTFtracks= TrackCollection )
    PTSeq = parOR("precisionTrackingIn"+precName, PTAlgs  )

    #Get last tracks from the list as input for other alg       
    tauIdTrackSequence += PTSeq
    trackParticles = PTTrackParticles[-1]

    if "FTFId" in name:
      tauPrecisionAlg = _algoTauPrecision(inputRoIs = RoIs, tracks = trackParticles, step = "Id")
    elif "FTFTrackInView" in name:
      tauPrecisionAlg = _algoTauPrecision(inputRoIs = RoIs, tracks = trackParticles, step = "Track")
    elif "FTFTrackTwo" in name:
      tauPrecisionAlg = _algoTauPrecision(inputRoIs = RoIs, tracks = trackParticles, step = "TrackTwo")
    elif "FTFIso" in name:
      tauPrecisionAlg = _algoTauPrecisionMVA(inputRoIs = RoIs, tracks = trackParticles, step = "PrecisionMVA")
    elif "EF" in name:
      tauPrecisionAlg = _algoTauPrecisionMVA(inputRoIs = RoIs, tracks = trackParticles, step = "EF")

    tauIdTrackSequence += tauPrecisionAlg
    sequenceOut = tauPrecisionAlg.Key_trigTauJetOutputContainer

    return tauIdTrackSequence, sequenceOut

def tauCoreTrackSequence( RoIs, name ):

    tauCoreTrackSequence = seqAND(name)

    from TrigInDetConfig.InDetSetup import makeInDetAlgs
    viewAlgs, ViewVerify = makeInDetAlgs(whichSignature='TauCore',separateTrackParticleCreator="_TauCore",rois = RoIs)

    for viewAlg in viewAlgs:
       if "InDetTrigTrackParticleCreatorAlg" in viewAlg.name():
         TrackCollection = viewAlg.TrackName

    ViewVerify.DataObjects += [( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloOnlyMVA' ),
                               ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+' + RoIs ),
                               ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' )] #For some reason not picked up properly

    # Make sure the required objects are still available at whole-event level
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence.SGInputLoader.Load += [( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' )]
    from IOVDbSvc.CondDB import conddb
    if not conddb.folderRequested( "PixelClustering/PixelClusNNCalib" ):
      topSequence.SGInputLoader.Load += [( 'TTrainedNetworkCollection' , 'ConditionStore+PixelClusterNN' ),
                                         ( 'TTrainedNetworkCollection' , 'ConditionStore+PixelClusterNNWithTrack' )]
      ViewVerify.DataObjects += [( 'TTrainedNetworkCollection' , 'ConditionStore+PixelClusterNN' ),
                                 ( 'TTrainedNetworkCollection' , 'ConditionStore+PixelClusterNNWithTrack' )]

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

def tauFTFTrackTwoSequence(ConfigFlags):

    RecoSequenceName = "tauFTFTrackTwoInViewSequence"

    ftfTrackTwoViewsMaker                   = EventViewCreatorAlgorithm("IMFTFTrackTwo")
    ftfTrackTwoViewsMaker.RoIsLink          = "roi"
    ftfTrackTwoViewsMaker.RoITool           = ViewCreatorPreviousROITool()
    ftfTrackTwoViewsMaker.InViewRoIs        = "TIsoViewRoIs" # contract with the fast track core
    ftfTrackTwoViewsMaker.Views             = "TAUFTFTrackTwoViews"
    ftfTrackTwoViewsMaker.ViewFallThrough   = True
    ftfTrackTwoViewsMaker.RequireParentView = True
    ftfTrackTwoViewsMaker.ViewNodeName      = RecoSequenceName

    (tauFTFTrackTwoInViewSequence, sequenceOut) = tauIdTrackSequence( ftfTrackTwoViewsMaker.InViewRoIs, RecoSequenceName)

    tauFastTrackTwoSequence = seqAND("tauFastTrackTwoSequence", [ftfTrackTwoViewsMaker, tauFTFTrackTwoInViewSequence ])
    return (tauFastTrackTwoSequence, ftfTrackTwoViewsMaker, sequenceOut)

def tauFTFTrackSequence(ConfigFlags):

    RecoSequenceName = "tauFTFTrackInViewSequence"

    ftfTrackViewsMaker                   = EventViewCreatorAlgorithm("IMFTFTrack")
    ftfTrackViewsMaker.RoIsLink          = "roi"
    ftfTrackViewsMaker.RoITool           = ViewCreatorPreviousROITool()
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
    ftfIdViewsMaker.RoIsLink          = "roi"
    ftfIdViewsMaker.RoITool           = ViewCreatorPreviousROITool()
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
    ftfCoreViewsMaker.RoIsLink          = "roi"
    ftfCoreViewsMaker.InViewRoIs        = "TCoreViewRoIs" # contract with the fastCalo
    ftfCoreViewsMaker.RoITool           = ViewCreatorPreviousROITool()
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
    ftfIsoViewsMaker.RoIsLink          = "roi"
    ftfIsoViewsMaker.RoITool           = ViewCreatorPreviousROITool()
    ftfIsoViewsMaker.InViewRoIs        = "TIsoViewRoIs" # contract with the fast track core
    ftfIsoViewsMaker.Views             = "TAUFTFIsoViews"
    ftfIsoViewsMaker.ViewFallThrough   = True
    ftfIsoViewsMaker.RequireParentView = True
    ftfIsoViewsMaker.ViewNodeName      = RecoSequenceName

    (tauFTFIsoInViewSequence, sequenceOut) = tauIdTrackSequence( ftfIsoViewsMaker.InViewRoIs, RecoSequenceName)

    tauFastTrackIsoSequence = seqAND("tauFastTrackIsoSequence", [ftfIsoViewsMaker, tauFTFIsoInViewSequence ])
    return (tauFastTrackIsoSequence, ftfIsoViewsMaker, sequenceOut)

def tauEFSequence(ConfigFlags):

    RecoSequenceName = "tauEFInViewSequence"

    efViewsMaker                   = EventViewCreatorAlgorithm("IMTauEF")
    efViewsMaker.RoIsLink          = "roi"  
    efViewsMaker.RoITool           = ViewCreatorPreviousROITool()
    efViewsMaker.InViewRoIs        = "TIsoViewRoIs" # contract with the fast track core                                                                   
    efViewsMaker.Views             = "TAUEFViews"
    efViewsMaker.ViewFallThrough   = True
    efViewsMaker.RequireParentView = True
    efViewsMaker.ViewNodeName      = RecoSequenceName

    (tauEFInViewSequence, sequenceOut) = tauIdTrackSequence( efViewsMaker.InViewRoIs, RecoSequenceName)

    tauEFSequence = seqAND("tauEFSequence", [efViewsMaker, tauEFInViewSequence ])
    return (tauEFSequence, efViewsMaker, sequenceOut)
