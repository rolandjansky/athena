#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR, seqAND
from AthenaCommon.GlobalFlags import globalflags
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorInitialROITool, ViewCreatorFetchFromViewROITool, ViewCreatorPreviousROITool
from TrigT2CaloCommon.CaloDef import HLTLCTopoRecoSequence
from TrigEDMConfig.TriggerEDMRun3 import recordable
from TriggerMenuMT.HLT.Menu.MenuComponents import RecoFragmentsPool
import AthenaCommon.CfgMgr as CfgMgr


#Retrieve short name for tau signature that can be used as suffix to be appended to the names of alg/tools
#Based on these names specific ID config is retrieved
#This utilizes name of the reco sequence from which checks specific string pattern
def _getTauSignatureShort( name ):
    signature = ""
    if "FTFCore" in name:
      signature = 'tauCore'
      signatureID = 'tauCore'
    elif "IsoInView" in name:
      signature = 'tauIso'
      signatureID = 'tauIso'
    elif "IsoBDT" in name:
      signature = 'tauIsoBDT'
      signatureID = 'tauIsoBDT'
    elif "MVA" in name:
      signature = 'tauMVA'
      signatureID = 'tauIso'
    elif "LLP" in name:
      signature = 'tauLLP'
      signatureID = 'tauIso'
    else:
      raise Exception( "getTauSignatureShort() called with incorrect non existent slice: "+name )
      return None

    return signature, signatureID

def _algoTauRoiUpdater(name, inputRoIs, clusters):
    from TrigTauHypo.TrigTauHypoConf import TrigTauCaloRoiUpdater

    algo                               = TrigTauCaloRoiUpdater("TauCaloRoiUpdater")
    algo.RoIInputKey                   = inputRoIs
    algo.RoIOutputKey                  = "UpdatedCaloRoI"
    algo.CaloClustersKey               = clusters
    return algo

def _algoTauCaloOnlyMVA(name, inputRoIs, clusters):
    from TrigTauRec.TrigTauRecConfig import TrigTauRecMerged_TauCaloOnlyMVA

    algo                               = TrigTauRecMerged_TauCaloOnlyMVA(name="TrigTauRecMerged_TauCaloOnlyMVA")
    algo.RoIInputKey                   = inputRoIs
    algo.clustersKey                   = clusters
    algo.Key_vertexInputContainer      = ""
    algo.Key_trackPartInputContainer   = ""
    algo.Key_trigTauJetInputContainer  = ""
    algo.Key_trigTauTrackInputContainer  = ""
    algo.Key_trigTauJetOutputContainer = recordable("HLT_TrigTauRecMerged_CaloMVAOnly")
    algo.Key_trigTauTrackOutputContainer = "HLT_tautrack_dummy"
    return algo

def _algoTauTrackRoiUpdater(inputRoIs, tracks):
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    config = getInDetTrigConfig("tauIso")
    from TrigTauHypo.TrigTauHypoConf import TrigTauTrackRoiUpdater
    algo                               = TrigTauTrackRoiUpdater("TrackRoiUpdater")
    algo.etaHalfWidth                  = config.etaHalfWidth
    algo.phiHalfWidth                  = config.phiHalfWidth
    algo.z0HalfWidth                   = config.zedHalfWidth
    algo.RoIInputKey                   = inputRoIs
    algo.RoIOutputKey                  = "UpdatedTrackRoI"
    algo.fastTracksKey                 = tracks
    algo.Key_trigTauJetInputContainer  = ""
    return algo

def _algoTauTrackBDTRoiUpdater(inputRoIs, tracks):
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    config = getInDetTrigConfig("tauIso")
    from TrigTauHypo.TrigTauHypoConf import TrigTauTrackRoiUpdater
    algo                               = TrigTauTrackRoiUpdater("TrackRoiUpdaterBDT")
    algo.etaHalfWidth                  = config.etaHalfWidth
    algo.phiHalfWidth                  = config.phiHalfWidth
    algo.z0HalfWidth                   = config.zedHalfWidth
    algo.RoIInputKey                   = inputRoIs
    algo.RoIOutputKey                  = "UpdatedTrackBDTRoI"
    algo.fastTracksKey                 = tracks
    algo.useBDT                        = True
    algo.Key_trigTauJetInputContainer  = "HLT_TrigTauRecMerged_CaloMVAOnly"
    return algo

def _algoTauPrecision(name, inputRoIs, tracks):
    from TrigTauRec.TrigTauRecConfig import TrigTauRecMerged_TauPrecisionMVA
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig

    if "MVA" in name:
      algo                                 = TrigTauRecMerged_TauPrecisionMVA(name= "TrigTauRecMerged_TauPrecision_PrecisionMVA", doTrackBDT=False, doRNN=True, doLLP=False) 
      algo.Key_trigTauJetOutputContainer   = recordable("HLT_TrigTauRecMerged_MVA")
      algo.Key_trigTauTrackOutputContainer = recordable("HLT_tautrack_MVA")
    elif "LLP" in name:
      algo                                 = TrigTauRecMerged_TauPrecisionMVA(name= "TrigTauRecMerged_TauPrecision_PrecisionLLP", doTrackBDT=False, doRNN=False,doLLP=True)
      algo.Key_trigTauJetOutputContainer   = recordable("HLT_TrigTauRecMerged_LLP")
      algo.Key_trigTauTrackOutputContainer = recordable("HLT_tautrack_LLP")
    else:
      raise Exception( "_algoTauPrecisio : called with incorrect non existent name: "+name )
      return None

    algo.Key_trigTauTrackInputContainer  = "HLT_tautrack_dummy"
    algo.Key_trigTauJetInputContainer    = "HLT_TrigTauRecMerged_CaloMVAOnly"
    algo.Key_trigJetSeedOutputKey        = recordable("HLT_jet_seed")

    algo.RoIInputKey                     = inputRoIs
    algo.clustersKey                     = ""
    algo.Key_vertexInputContainer        = getInDetTrigConfig( "tauIso" ).vertex
    algo.Key_trackPartInputContainer     = tracks

    return algo

def tauCaloMVARecoSequence(flags, InViewRoIs, SeqName):
    global TauCaloJetContainer
    # lc sequence
    (lcTopoInViewSequence, lcCaloSequenceOut) = RecoFragmentsPool.retrieve(HLTLCTopoRecoSequence, flags, RoIs=InViewRoIs)
    tauCaloRoiUpdaterAlg                      = _algoTauRoiUpdater(SeqName, inputRoIs = InViewRoIs, clusters = lcCaloSequenceOut)
    updatedRoIs                               = tauCaloRoiUpdaterAlg.RoIOutputKey
    tauCaloOnlyMVAAlg	                      = _algoTauCaloOnlyMVA(SeqName, inputRoIs = updatedRoIs, clusters = lcCaloSequenceOut)
    RecoSequence                              = parOR( SeqName, [lcTopoInViewSequence,tauCaloRoiUpdaterAlg,tauCaloOnlyMVAAlg] )
    return (RecoSequence, tauCaloOnlyMVAAlg.Key_trigTauJetOutputContainer)

def tauCaloMVASequence(flags):
    """ Creates L2 Fast Calo sequence for Taus"""
    # EV creator
    InViewRoIs                              = "CaloMVA_RoIs"
    RecoSequenceName                        = "tauCaloMVAInViewSequence"

    tauCaloMVAViewsMaker                    = EventViewCreatorAlgorithm( "IMtauCaloMVA")
    tauCaloMVAViewsMaker.ViewFallThrough    = True
    tauCaloMVAViewsMaker.RoIsLink           = "initialRoI"
    tauCaloMVAViewsMaker.RoITool            = ViewCreatorInitialROITool()
    tauCaloMVAViewsMaker.InViewRoIs         = InViewRoIs
    tauCaloMVAViewsMaker.Views              = "TAUCaloMVAViews"
    tauCaloMVAViewsMaker.ViewNodeName       = RecoSequenceName
    (tauCaloMVAInViewSequence, sequenceOut) = tauCaloMVARecoSequence(flags, InViewRoIs, RecoSequenceName)

    tauCaloMVARecoVDV = CfgMgr.AthViews__ViewDataVerifier( "tauCaloMVARecoVDV" )
    tauCaloMVARecoVDV.DataObjects = [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s'%(InViewRoIs)),
                                     ( 'CaloBCIDAverage' , 'StoreGateSvc+CaloBCIDAverage' ),
                                     ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                                     ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.actualInteractionsPerCrossing' ),
                                     ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.averageInteractionsPerCrossing' )]
    tauCaloMVAInViewSequence += tauCaloMVARecoVDV

    tauCaloMVASequence = seqAND("tauCaloMVASequence", [tauCaloMVAViewsMaker, tauCaloMVAInViewSequence ])
    return (tauCaloMVASequence, tauCaloMVAViewsMaker, sequenceOut)

def tauIdSequence( RoIs, name):

    tauIdSequence = parOR(name)

    signatureName, signatureNameID = _getTauSignatureShort( name )
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( signatureNameID )

    ViewVerifyId = CfgMgr.AthViews__ViewDataVerifier("tauIdViewDataVerifier_"+signatureName)
    ViewVerifyId.DataObjects = [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % RoIs      ),
                                ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.averageInteractionsPerCrossing'   ),
                                ( 'xAOD::VertexContainer', 'StoreGateSvc+'+getInDetTrigConfig( "tauIso" ).vertex),
                                ( 'xAOD::TauTrackContainer' , 'StoreGateSvc+HLT_tautrack_dummy' ),
                                ( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloMVAOnly' ),
                                ( 'xAOD::TrackParticleContainer' , 'StoreGateSvc+'+IDTrigConfig.tracks_IDTrig() )]

    tauIdSequence+= ViewVerifyId

    tauPrecisionAlg = _algoTauPrecision(name, inputRoIs = RoIs, tracks = IDTrigConfig.tracks_IDTrig())

    tauIdSequence += tauPrecisionAlg

    sequenceOut = tauPrecisionAlg.Key_trigTauJetOutputContainer

    return tauIdSequence, sequenceOut


def precTrackSequence( RoIs , name):

    signatureName, signatureNameID = _getTauSignatureShort( name )
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( signatureNameID )

    ViewVerifyTrk = CfgMgr.AthViews__ViewDataVerifier("tauViewDataVerifier_"+signatureName)
    ViewVerifyTrk.DataObjects = [( 'xAOD::TrackParticleContainer' , 'StoreGateSvc+%s' % IDTrigConfig.tracks_FTF() ),
                                 ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.averageInteractionsPerCrossing' ),
                                 ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % RoIs ),
                                 ( 'xAOD::TauTrackContainer' , 'StoreGateSvc+HLT_tautrack_dummy' ),
                                 ( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloMVAOnly' ),    
                                 ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_FlaggedCondData_TRIG' ),
                                 ( 'xAOD::IParticleContainer' , 'StoreGateSvc+%s' % IDTrigConfig.tracks_FTF() )]

    # Make sure the required objects are still available at whole-event level
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    if globalflags.InputFormat.is_bytestream():
      ViewVerifyTrk.DataObjects += [( 'IDCInDetBSErrContainer' , 'StoreGateSvc+PixelByteStreamErrs' ),
                                 ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' ) ]
    else:
      topSequence.SGInputLoader.Load += [( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' )]
      ViewVerifyTrk.DataObjects += [( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' )]

    #Precision Tracking
    PTAlgs = [] #List of precision tracking algs 
    PTTracks = [] #List of TrackCollectionKeys
    PTTrackParticles = [] #List of TrackParticleKeys
    
    from TrigInDetConfig.InDetTrigPrecisionTracking import makeInDetTrigPrecisionTracking
    #When run in a different view than FTF some data dependencies needs to be loaded through verifier
    #Pass verifier as an argument and it will automatically append necessary DataObjects@NOTE: Don't provide any verifier if loaded in the same view as FTF
    PTTracks, PTTrackParticles, PTAlgs = makeInDetTrigPrecisionTracking( config = IDTrigConfig, verifier = ViewVerifyTrk, rois = RoIs )

    from TrigInDetConfig.InDetTrigVertices import makeInDetTrigVertices
    vtxAlg = makeInDetTrigVertices( whichSignature       = signatureName, 
                           inputTrackCollection = IDTrigConfig.tracks_IDTrig(), 
                           outputVtxCollection  = IDTrigConfig.vertex, 
                           config               = IDTrigConfig, 
                           adaptiveVertex       = IDTrigConfig.adaptiveVertex ) 

    trackSequence = parOR(name, [ViewVerifyTrk] + PTAlgs + vtxAlg )

    #Get last tracks from the list as input for other alg       
    sequenceOut = PTTrackParticles[-1]

    return trackSequence, sequenceOut

def tauFTFSequence( RoIs, name ):

    tauFTFSequence = parOR(name)

    signatureName, signatureNameID = _getTauSignatureShort( name )
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( signatureNameID )

    from TrigInDetConfig.InDetTrigFastTracking import makeInDetTrigFastTracking
    viewAlgs, viewVerify = makeInDetTrigFastTracking( config = IDTrigConfig, rois = RoIs )

    TrackCollection = IDTrigConfig.trkTracks_FTF()

    viewVerify.DataObjects += [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % RoIs ),
                               ( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloMVAOnly')] 

    if 'Core' in signatureName:
      tauTrackRoiUpdaterAlg = _algoTauTrackRoiUpdater(inputRoIs = RoIs, tracks = TrackCollection)
      tauTrackRoiUpdaterAlgBDT = _algoTauTrackBDTRoiUpdater(inputRoIs = RoIs, tracks = TrackCollection)
      viewAlgs.append(tauTrackRoiUpdaterAlgBDT)
      viewAlgs.append(tauTrackRoiUpdaterAlg)

    tauFTFSequence += viewAlgs

    sequenceOut = TrackCollection

    return tauFTFSequence, sequenceOut

# ===============================================================================================                                                           
#   Reco sequence for FTFTauCore + TrackRoIUpdater Alg (tracktwoMVA)                                                                  
# ===============================================================================================  

def tauFTFCoreSequence(ConfigFlags):

    RecoSequenceName                    = "tauFTFCoreInViewSequence"

    newRoITool                          = ViewCreatorFetchFromViewROITool()
    newRoITool.RoisWriteHandleKey       = recordable("HLT_Roi_TauCore") #RoI collection recorded to EDM           
    newRoITool.InViewRoIs               = "UpdatedCaloRoI" #input RoIs from calo only step   

    ftfCoreViewsMaker                   = EventViewCreatorAlgorithm("IMFTFCore")
    ftfCoreViewsMaker.mergeUsingFeature = True
    ftfCoreViewsMaker.RoITool           = newRoITool
    ftfCoreViewsMaker.InViewRoIs        = "RoiForTauCore"
    ftfCoreViewsMaker.Views             = "TAUFTFCoreViews"
    ftfCoreViewsMaker.ViewFallThrough   = True
    ftfCoreViewsMaker.RequireParentView = True
    ftfCoreViewsMaker.ViewNodeName      = RecoSequenceName

    (tauFTFCoreInViewSequence, sequenceOut) = tauFTFSequence( ftfCoreViewsMaker.InViewRoIs, RecoSequenceName)

    tauFastTrackCoreSequence = seqAND("tauFastTrackCoreSequence", [ftfCoreViewsMaker, tauFTFCoreInViewSequence ])
    return (tauFastTrackCoreSequence, ftfCoreViewsMaker, sequenceOut)

# ===============================================================================================                                                          
#   Reco sequence for FTFTauIso (tracktwoMVA)                                                                  
# ===============================================================================================  

def tauFTFIsoSequence(ConfigFlags):

    RecoSequenceName                   = "tauFTFIsoInViewSequence"

    newRoITool                         = ViewCreatorFetchFromViewROITool()
    newRoITool.RoisWriteHandleKey      = recordable("HLT_Roi_TauIso") #RoI collection recorded to EDM
    newRoITool.InViewRoIs              = "UpdatedTrackRoI" #input RoIs from calo only step

    ftfIsoViewsMaker                   = EventViewCreatorAlgorithm("IMFTFIso")
    ftfIsoViewsMaker.RoIsLink          = "roi"
    ftfIsoViewsMaker.RoITool           = newRoITool
    ftfIsoViewsMaker.InViewRoIs        = "RoiForTauIso"
    ftfIsoViewsMaker.Views             = "TAUFTFIsoViews"
    ftfIsoViewsMaker.ViewFallThrough   = True
    ftfIsoViewsMaker.RequireParentView = True
    ftfIsoViewsMaker.ViewNodeName      = RecoSequenceName

    (tauFTFIsoInViewSequence, sequenceOut) = tauFTFSequence( ftfIsoViewsMaker.InViewRoIs, RecoSequenceName)

    tauFastTrackIsoSequence = seqAND("tauFastTrackIsoSequence", [ftfIsoViewsMaker, tauFTFIsoInViewSequence ])
    return (tauFastTrackIsoSequence, ftfIsoViewsMaker, sequenceOut)

# ===============================================================================================                                                                                                  
#   Reco sequence for FTFTauIsoBDT (tracktwoMVABDT)                                                                                                                                                
# ===============================================================================================                                                                                                  

def tauFTFIsoBDTSequence(ConfigFlags):

    RecoSequenceName                   = "tauFTFIsoBDTInViewSequence"

    newRoITool                         = ViewCreatorFetchFromViewROITool()
    newRoITool.RoisWriteHandleKey      = recordable("HLT_Roi_TauIsoBDT") #RoI collection recorded to EDM                                                                                           
    newRoITool.InViewRoIs              = "UpdatedTrackBDTRoI" #input RoIs from calo only step                                                                                                      

    ftfIsoViewsMaker                   = EventViewCreatorAlgorithm("IMFTFIsoBDT")
    ftfIsoViewsMaker.RoIsLink          = "roi"
    ftfIsoViewsMaker.RoITool           = newRoITool
    ftfIsoViewsMaker.InViewRoIs        = "RoiForTauIsoBDT"
    ftfIsoViewsMaker.Views             = "TAUFTFIsoBDTViews"
    ftfIsoViewsMaker.ViewFallThrough   = True
    ftfIsoViewsMaker.RequireParentView = True
    ftfIsoViewsMaker.ViewNodeName      = RecoSequenceName

    (tauFTFIsoBDTInViewSequence, sequenceOut) = tauFTFSequence( ftfIsoViewsMaker.InViewRoIs, RecoSequenceName)

    tauFastTrackIsoBDTSequence = seqAND("tauFastTrackIsoBDTSequence", [ftfIsoViewsMaker, tauFTFIsoBDTInViewSequence ])
    return (tauFastTrackIsoBDTSequence, ftfIsoViewsMaker, sequenceOut)

# ===============================================================================================                                                            
#   Reco sequence for Precision tracking (from FTF Iso algorithm)   (tracktwoMVA)                           
# ===============================================================================================                                                            

def tauPrecIsoTrackSequence(ConfigFlags):

    RecoSequenceName                       = "precFTFIsoInViewSequence"

    tauPrecIsoViewsMaker                   = EventViewCreatorAlgorithm("IMPrecIsoTrack")
    tauPrecIsoViewsMaker.RoIsLink          = "roi"
    tauPrecIsoViewsMaker.RoITool           = ViewCreatorPreviousROITool()
    tauPrecIsoViewsMaker.InViewRoIs        = "RoiForTauIso"
    tauPrecIsoViewsMaker.Views             = "TAUPrecIsoViews"
    tauPrecIsoViewsMaker.ViewFallThrough   = True
    tauPrecIsoViewsMaker.RequireParentView = True
    tauPrecIsoViewsMaker.ViewNodeName      = RecoSequenceName

    (tauPrecIsoTrackInViewSequence, sequenceOut) = precTrackSequence( tauPrecIsoViewsMaker.InViewRoIs, RecoSequenceName)

    tauPrecIsoTrkSequence = seqAND("tauPrecIsoTrkSequence", [tauPrecIsoViewsMaker, tauPrecIsoTrackInViewSequence ])
    return (tauPrecIsoTrkSequence, tauPrecIsoViewsMaker, sequenceOut)

# ===============================================================================================                                                            
#    Reco sequence for Tau Precision MVA Alg (tracktwoMVA)                                                                                 
# ===============================================================================================                                                            

def tauMVASequence(ConfigFlags):

    RecoSequenceName = "tauMVAInViewSequence"

    mvaViewsMaker                   = EventViewCreatorAlgorithm("IMTauMVA")
    mvaViewsMaker.RoIsLink          = "roi"
    mvaViewsMaker.RoITool           = ViewCreatorPreviousROITool()
    mvaViewsMaker.InViewRoIs        = "RoiForTauIso"
    mvaViewsMaker.Views             = "TAUMVAViews"
    mvaViewsMaker.ViewFallThrough   = True
    mvaViewsMaker.RequireParentView = True
    mvaViewsMaker.ViewNodeName      = RecoSequenceName

    (tauMVAInViewSequence, sequenceOut) = tauIdSequence( mvaViewsMaker.InViewRoIs, RecoSequenceName)

    tauSequence = seqAND("tauSequence", [mvaViewsMaker, tauMVAInViewSequence ])
    return (tauSequence, mvaViewsMaker, sequenceOut)

# ===============================================================================================                                                            
#    Reco sequence for Tau Precision LLP Alg (tracktwoLLP)                                                                                 
# ===============================================================================================                                                            

def tauLLPSequence(ConfigFlags):

    RecoSequenceName = "tauLLPInViewSequence"

    mvaViewsMaker                   = EventViewCreatorAlgorithm("IMTauLLP")
    mvaViewsMaker.RoIsLink          = "roi"
    mvaViewsMaker.RoITool           = ViewCreatorPreviousROITool()
    mvaViewsMaker.InViewRoIs        = "RoiForTauIso"
    mvaViewsMaker.Views             = "TAULLPViews"
    mvaViewsMaker.ViewFallThrough   = True
    mvaViewsMaker.RequireParentView = True
    mvaViewsMaker.ViewNodeName      = RecoSequenceName

    (tauLLPInViewSequence, sequenceOut) = tauIdSequence( mvaViewsMaker.InViewRoIs, RecoSequenceName)

    tauSequence = seqAND("tauLLPSequence", [mvaViewsMaker, tauLLPInViewSequence ])
    return (tauSequence, mvaViewsMaker, sequenceOut)
