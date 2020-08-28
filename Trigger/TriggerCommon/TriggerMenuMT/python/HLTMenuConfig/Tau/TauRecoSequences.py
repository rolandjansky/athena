#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR, seqAND
from AthenaCommon.GlobalFlags import globalflags
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorInitialROITool, ViewCreatorFetchFromViewROITool 
from TrigT2CaloCommon.CaloDef import HLTLCTopoRecoSequence
from TrigEDMConfig.TriggerEDMRun3 import recordable
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool
import AthenaCommon.CfgMgr as CfgMgr

def _algoTauRoiUpdater(inputRoIs, clusters):
    from TrigTauHypo.TrigTauHypoConf import TrigTauCaloRoiUpdaterMT
    algo                               = TrigTauCaloRoiUpdaterMT("TauCaloRoiUpdater")
    algo.RoIInputKey                   = inputRoIs
    algo.RoIOutputKey                  = "UpdatedCaloRoI"
    algo.CaloClustersKey               = clusters
    return algo

def _algoTauCaloOnly(L1RoIs, inputRoIs, clusters):
    from TrigTauRec.TrigTauRecConfigMT import TrigTauRecMerged_TauCaloOnly
    algo                               = TrigTauRecMerged_TauCaloOnly()
    algo.RoIInputKey                   = inputRoIs
    algo.clustersKey                   = clusters
    algo.L1RoIKey                      = L1RoIs
    algo.Key_vertexInputContainer      = ""
    algo.Key_trackPartInputContainer   = ""
    algo.Key_trigTauJetInputContainer  = ""
    algo.Key_trigTauTrackInputContainer  = ""
    algo.Key_trigTauJetOutputContainer = recordable("HLT_TrigTauRecMerged_CaloOnly")
    algo.Key_trigTauTrackOutputContainer = "HLT_tautrack_dummy"
    return algo

def _algoTauCaloOnlyMVA(L1RoIs, inputRoIs, clusters):
    from TrigTauRec.TrigTauRecConfigMT import TrigTauRecMerged_TauCaloOnlyMVA
    algo                               = TrigTauRecMerged_TauCaloOnlyMVA()
    algo.RoIInputKey                   = inputRoIs
    algo.L1RoIKey                      = L1RoIs
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
    algo.RoIOutputKey                  = "UpdatedTrackRoI"
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
    algo.Key_trackPartInputContainer     = tracks
    if "Id" in step:
       algo.Key_trigTauTrackInputContainer  = "HLT_tautrack_dummy"
       algo.Key_trigTauJetInputContainer    = "HLT_TrigTauRecMerged_CaloOnly"
    elif "Track" in step:
       algo.Key_trigTauTrackInputContainer  = "HLT_tautrack_Presel"
       algo.Key_trigTauJetInputContainer    = "HLT_TrigTauRecMerged_Presel"
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
       algo.Key_trigTauJetInputContainer = "HLT_TrigTauRecMerged_CaloOnly"
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
    updatedRoIs                               = tauCaloRoiUpdaterAlg.RoIOutputKey
    tauCaloOnlyAlg                            = _algoTauCaloOnly(L1RoIs = InViewRoIs,inputRoIs   = updatedRoIs, clusters = lcCaloSequenceOut)
    RecoSequence                              = parOR( SeqName, [lcTopoInViewSequence,tauCaloRoiUpdaterAlg,tauCaloOnlyAlg] )
    return (RecoSequence, tauCaloOnlyAlg.Key_trigTauJetOutputContainer)

def tauCaloMVARecoSequence(InViewRoIs, SeqName):
    global TauCaloJetContainer
    # lc sequence
    (lcTopoInViewSequence, lcCaloSequenceOut) = RecoFragmentsPool.retrieve(HLTLCTopoRecoSequence, InViewRoIs)
    tauCaloRoiUpdaterAlg                      = _algoTauRoiUpdater(inputRoIs = InViewRoIs, clusters = lcCaloSequenceOut)
    updatedRoIs                               = tauCaloRoiUpdaterAlg.RoIOutputKey
    tauCaloOnlyMVAAlg	                      = _algoTauCaloOnlyMVA(L1RoIs = InViewRoIs,inputRoIs = updatedRoIs, clusters = lcCaloSequenceOut)
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
                                  ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                                  ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.ActIntPerXDecor' ),
                                  ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.AveIntPerXDecor' )]
    tauCaloInViewSequence += tauCaloRecoVDV

    # Make sure the required objects are still available at whole-event level
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence.SGInputLoader.Load += [( 'ILArHVScaleCorr' , 'ConditionStore+LArHVScaleCorrRecomputed' )]

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
                                     ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                                     ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.ActIntPerXDecor' ),
                                     ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.AveIntPerXDecor' )]
    tauCaloMVAInViewSequence += tauCaloMVARecoVDV

    # Make sure the required objects are still available at whole-event level
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence.SGInputLoader.Load += [( 'ILArHVScaleCorr' , 'ConditionStore+LArHVScaleCorrRecomputed' )]

    tauCaloMVASequence = seqAND("tauCaloMVASequence", [tauCaloMVAViewsMaker, tauCaloMVAInViewSequence ])
    return (tauCaloMVASequence, tauCaloMVAViewsMaker, sequenceOut)

def tauIdTrackSequence( RoIs , name):

    tauIdTrackSequence = seqAND(name)

    signName = "Tau"

    if ("Iso" in name) or ("TrackTwo" in name) or ("EF" in name):
      signName = 'TauIso'

    from TrigInDetConfig.InDetSetup import makeInDetAlgs
    viewAlgs, viewVerify = makeInDetAlgs( whichSignature=signName, separateTrackParticleCreator=signName, rois = RoIs )

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
    viewAlgs, viewVerify = makeInDetAlgs( whichSignature=signName, separateTrackParticleCreator=signName, rois = RoIs, viewVerifier = tauViewDataVerifierName )

    if "FTFIso" in name:
       viewVerify.DataObjects += [( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloOnlyMVA' )]
    else:
       viewVerify.DataObjects += [( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloOnly')]

    viewVerify.DataObjects += [( 'xAOD::TauTrackContainer' , 'StoreGateSvc+HLT_tautrack_dummy' ),
                               ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+' + RoIs ),
                               ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                               ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.AveIntPerXDecor' ),
                               ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.ActIntPerXDecor' ),
                               ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+TAUCaloRoIs' )]

    # Make sure the required objects are still available at whole-event level
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    from IOVDbSvc.CondDB import conddb
    if not conddb.folderRequested( "PixelClustering/PixelClusNNCalib" ):
      topSequence.SGInputLoader.Load += [( 'TTrainedNetworkCollection' , 'ConditionStore+PixelClusterNN' ),
                                         ( 'TTrainedNetworkCollection' , 'ConditionStore+PixelClusterNNWithTrack' )]
      viewVerify.DataObjects += [( 'TTrainedNetworkCollection' , 'ConditionStore+PixelClusterNN' ),
                                 ( 'TTrainedNetworkCollection' , 'ConditionStore+PixelClusterNNWithTrack' )]

    if globalflags.InputFormat.is_bytestream():
      viewVerify.DataObjects += [( 'InDetBSErrContainer' , 'StoreGateSvc+PixelByteStreamErrs' ),
                                    ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' ) ]
    else:
      topSequence.SGInputLoader.Load += [( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' )]
      viewVerify.DataObjects += [( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' )]

    for viewAlg in viewAlgs:
       tauIdTrackSequence += viewAlg
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
    viewAlgs, viewVerify = makeInDetAlgs( whichSignature='TauCore', separateTrackParticleCreator="TauCore", rois = RoIs )

    for viewAlg in viewAlgs:
       if "InDetTrigTrackParticleCreatorAlg" in viewAlg.name():
         TrackCollection = viewAlg.TrackName

    if "MVA" in name:
       viewVerify.DataObjects += [( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloOnlyMVA' )]
    else:
       viewVerify.DataObjects += [( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloOnly')]


    viewVerify.DataObjects += [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+' + RoIs ),
                               ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' )] #For some reason not picked up properly

    from IOVDbSvc.CondDB import conddb
    if not conddb.folderRequested( "PixelClustering/PixelClusNNCalib" ):
      from AthenaCommon.AlgSequence import AlgSequence
      topSequence = AlgSequence()
      topSequence.SGInputLoader.Load += [( 'TTrainedNetworkCollection' , 'ConditionStore+PixelClusterNN' ),
                                         ( 'TTrainedNetworkCollection' , 'ConditionStore+PixelClusterNNWithTrack' )]
      viewVerify.DataObjects += [( 'TTrainedNetworkCollection' , 'ConditionStore+PixelClusterNN' ),
                                 ( 'TTrainedNetworkCollection' , 'ConditionStore+PixelClusterNNWithTrack' )]

    tauTrackRoiUpdaterAlg = _algoTauTrackRoiUpdater(inputRoIs = RoIs, tracks = TrackCollection)

    viewAlgs.append(tauTrackRoiUpdaterAlg)

    tauCoreTrackSequence += viewAlgs

    sequenceOut = TrackCollection

    return tauCoreTrackSequence, sequenceOut

def tauFTFTrackTwoSequence(ConfigFlags):

    RecoSequenceName = "tauFTFTrackTwoInViewSequence"

    newRoITool                    = ViewCreatorFetchFromViewROITool()
    newRoITool.RoisWriteHandleKey = recordable("HLT_Roi_Tau_TrackTwo") #RoI collection recorded to EDM
    newRoITool.InViewRoIs         = "UpdatedTrackRoI" #input RoIs from calo only step 

    ftfTrackTwoViewsMaker                   = EventViewCreatorAlgorithm("IMFTFTrackTwo")
    ftfTrackTwoViewsMaker.RoIsLink          = "roi"
    ftfTrackTwoViewsMaker.RoITool           = newRoITool
    ftfTrackTwoViewsMaker.InViewRoIs        = "RoiForTauCore"
    ftfTrackTwoViewsMaker.Views             = "TAUFTFTrackTwoViews"
    ftfTrackTwoViewsMaker.ViewFallThrough   = True
    ftfTrackTwoViewsMaker.RequireParentView = True
    ftfTrackTwoViewsMaker.ViewNodeName      = RecoSequenceName

    (tauFTFTrackTwoInViewSequence, sequenceOut) = tauIdTrackSequence( ftfTrackTwoViewsMaker.InViewRoIs, RecoSequenceName)

    tauFastTrackTwoSequence = seqAND("tauFastTrackTwoSequence", [ftfTrackTwoViewsMaker, tauFTFTrackTwoInViewSequence ])
    return (tauFastTrackTwoSequence, ftfTrackTwoViewsMaker, sequenceOut)

def tauFTFTrackSequence(ConfigFlags):

    RecoSequenceName = "tauFTFTrackInViewSequence"

    newRoITool                    = ViewCreatorFetchFromViewROITool()
    newRoITool.RoisWriteHandleKey = recordable("HLT_Roi_Tau_Track") #RoI collection recorded to EDM
    newRoITool.InViewRoIs         = "UpdatedCaloRoI" #input RoIs from calo only step  

    ftfTrackViewsMaker                   = EventViewCreatorAlgorithm("IMFTFTrack")
    ftfTrackViewsMaker.RoIsLink          = "roi"
    ftfTrackViewsMaker.RoITool           = newRoITool
    ftfTrackViewsMaker.InViewRoIs        = "RoiForTau"
    ftfTrackViewsMaker.Views             = "TAUFTFTrackViews"
    ftfTrackViewsMaker.ViewFallThrough   = True
    ftfTrackViewsMaker.RequireParentView = True
    ftfTrackViewsMaker.ViewNodeName      = RecoSequenceName

    (tauFTFTrackInViewSequence, sequenceOut) = tauIdTrackSequence( ftfTrackViewsMaker.InViewRoIs, RecoSequenceName)

    tauFastTrackSequence = seqAND("tauFastTrackSequence", [ftfTrackViewsMaker, tauFTFTrackInViewSequence ])
    return (tauFastTrackSequence, ftfTrackViewsMaker, sequenceOut)

def tauFTFIdSequence(ConfigFlags):

    RecoSequenceName = "tauFTFIdInViewSequence"

    newRoITool                    = ViewCreatorFetchFromViewROITool()
    newRoITool.RoisWriteHandleKey = recordable("HLT_Roi_Tau") #RoI collection recorded to EDM
    newRoITool.InViewRoIs         = "UpdatedCaloRoI" #input RoIs from calo only step

    ftfIdViewsMaker                   = EventViewCreatorAlgorithm("IMFTFId")
    ftfIdViewsMaker.RoIsLink          = "roi"
    ftfIdViewsMaker.RoITool           = newRoITool
    ftfIdViewsMaker.InViewRoIs        = "RoiForTau"
    ftfIdViewsMaker.Views             = "TAUFTFIdViews"
    ftfIdViewsMaker.ViewFallThrough   = True
    ftfIdViewsMaker.RequireParentView = True
    ftfIdViewsMaker.ViewNodeName      = RecoSequenceName

    (tauFTFIdInViewSequence, sequenceOut) = tauIdTrackSequence( ftfIdViewsMaker.InViewRoIs, RecoSequenceName)

    tauFastTrackIdSequence = seqAND("tauFastTrackIdSequence", [ftfIdViewsMaker, tauFTFIdInViewSequence ])
    return (tauFastTrackIdSequence, ftfIdViewsMaker, sequenceOut)

def tauFTFCoreMVASequence(ConfigFlags):

    RecoSequenceName = "tauFTFCoreMVAInViewSequence"

    newRoITool                             = ViewCreatorFetchFromViewROITool()
    newRoITool.RoisWriteHandleKey          = recordable("HLT_Roi_TauCore_MVA") #RoI collection recorded to EDM                                
    newRoITool.InViewRoIs                  = "UpdatedCaloRoI" #input RoIs from calo only step        

    ftfCoreMVAViewsMaker                   = EventViewCreatorAlgorithm("IMFTFCoreMVA")
    ftfCoreMVAViewsMaker.RoIsLink          = "roi"
    ftfCoreMVAViewsMaker.RoITool           = newRoITool
    ftfCoreMVAViewsMaker.InViewRoIs        = "RoiForTauCore"
    ftfCoreMVAViewsMaker.Views             = "TAUFTFCoreMVAViews"
    ftfCoreMVAViewsMaker.ViewFallThrough   = True
    ftfCoreMVAViewsMaker.RequireParentView = True
    ftfCoreMVAViewsMaker.ViewNodeName      = RecoSequenceName

    (tauFTFCoreMVAInViewSequence, sequenceOut) = tauCoreTrackSequence( ftfCoreMVAViewsMaker.InViewRoIs, RecoSequenceName)

    tauFastTrackCoreMVASequence = seqAND("tauFastTrackCoreMVASequence", [ftfCoreMVAViewsMaker, tauFTFCoreMVAInViewSequence ])
    return (tauFastTrackCoreMVASequence, ftfCoreMVAViewsMaker, sequenceOut)

def tauFTFCoreSequence(ConfigFlags):

    RecoSequenceName = "tauFTFCoreInViewSequence"

    newRoITool                          = ViewCreatorFetchFromViewROITool()
    newRoITool.RoisWriteHandleKey       = recordable("HLT_Roi_TauCore") #RoI collection recorded to EDM           
    newRoITool.InViewRoIs               = "UpdatedCaloRoI" #input RoIs from calo only step   

    ftfCoreViewsMaker                   = EventViewCreatorAlgorithm("IMFTFCore")
    ftfCoreViewsMaker.RoIsLink          = "roi"
    ftfCoreViewsMaker.RoITool           = newRoITool
    ftfCoreViewsMaker.InViewRoIs        = "RoiForTauCore"
    ftfCoreViewsMaker.Views             = "TAUFTFCoreViews"
    ftfCoreViewsMaker.ViewFallThrough   = True
    ftfCoreViewsMaker.RequireParentView = True
    ftfCoreViewsMaker.ViewNodeName      = RecoSequenceName

    (tauFTFCoreInViewSequence, sequenceOut) = tauCoreTrackSequence( ftfCoreViewsMaker.InViewRoIs, RecoSequenceName)

    tauFastTrackCoreSequence = seqAND("tauFastTrackCoreSequence", [ftfCoreViewsMaker, tauFTFCoreInViewSequence ])
    return (tauFastTrackCoreSequence, ftfCoreViewsMaker, sequenceOut)

def tauFTFIsoSequence(ConfigFlags):

    RecoSequenceName = "tauFTFIsoInViewSequence"

    newRoITool                         = ViewCreatorFetchFromViewROITool()
    newRoITool.RoisWriteHandleKey      = recordable("HLT_Roi_TauIso_TauID") #RoI collection recorded to EDM
    newRoITool.InViewRoIs              = "UpdatedTrackRoI" #input RoIs from calo only step

    ftfIsoViewsMaker                   = EventViewCreatorAlgorithm("IMFTFIso")
    ftfIsoViewsMaker.RoIsLink          = "roi"
    ftfIsoViewsMaker.RoITool           = newRoITool
    ftfIsoViewsMaker.InViewRoIs        = "RoiForTauCore"
    ftfIsoViewsMaker.Views             = "TAUFTFIsoViews"
    ftfIsoViewsMaker.ViewFallThrough   = True
    ftfIsoViewsMaker.RequireParentView = True
    ftfIsoViewsMaker.ViewNodeName      = RecoSequenceName

    (tauFTFIsoInViewSequence, sequenceOut) = tauIdTrackSequence( ftfIsoViewsMaker.InViewRoIs, RecoSequenceName)

    tauFastTrackIsoSequence = seqAND("tauFastTrackIsoSequence", [ftfIsoViewsMaker, tauFTFIsoInViewSequence ])
    return (tauFastTrackIsoSequence, ftfIsoViewsMaker, sequenceOut)

def tauEFSequence(ConfigFlags):

    RecoSequenceName = "tauEFInViewSequence"

    newRoITool                     = ViewCreatorFetchFromViewROITool()
    newRoITool.RoisWriteHandleKey  = recordable("HLT_Roi_TauID") #RoI collection recorded to EDM
    newRoITool.InViewRoIs          = "UpdatedTrackRoI" #input RoIs from calo only step      

    efViewsMaker                   = EventViewCreatorAlgorithm("IMTauEF")
    efViewsMaker.RoIsLink          = "roi"  
    efViewsMaker.RoITool           = newRoITool
    efViewsMaker.InViewRoIs        = "RoiForTauCore"
    efViewsMaker.Views             = "TAUEFViews"
    efViewsMaker.ViewFallThrough   = True
    efViewsMaker.RequireParentView = True
    efViewsMaker.ViewNodeName      = RecoSequenceName

    (tauEFInViewSequence, sequenceOut) = tauIdTrackSequence( efViewsMaker.InViewRoIs, RecoSequenceName)

    tauEFSequence = seqAND("tauEFSequence", [efViewsMaker, tauEFInViewSequence ])
    return (tauEFSequence, efViewsMaker, sequenceOut)
