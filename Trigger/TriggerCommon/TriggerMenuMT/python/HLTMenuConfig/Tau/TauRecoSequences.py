#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR, seqAND
from AthenaCommon.GlobalFlags import globalflags
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorInitialROITool, ViewCreatorFetchFromViewROITool, ViewCreatorPreviousROITool
from TrigT2CaloCommon.CaloDef import HLTLCTopoRecoSequence
from TrigEDMConfig.TriggerEDMRun3 import recordable
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool
import AthenaCommon.CfgMgr as CfgMgr


#Retrieve short name for tau signature that can be used as suffix to be appended to the names of alg/tools
#Based on these names specific ID config is retrieved
#This utilizes name of the reco sequence from which checks specific string pattern
def _getTauSignatureShort( name ):
    signature = ""
    if "FTFTau" in name:
      signature = 'tauTau'
      signatureID = 'tauTau'
    elif "FTFCore" in name:
      signature = 'tauCore'
      signatureID = 'tauCore'
    elif "IsoInView" in name:
      signature = 'tauIso'
      signatureID = 'tauIso'
    elif "IsoBDT" in name:
      signature = 'tauIsoBDT'
      signatureID = 'tauIsoBDT'
    elif "TrackInView" in name:
      signature = 'tauTrk'
      signatureID = 'tauTau'
    elif "TrackTwo" in name:
      signature = 'tauTrkTwo'
      signatureID = 'tauIso'
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

def _algoTauRoiUpdater(inputRoIs, clusters):
    from TrigTauHypo.TrigTauHypoConf import TrigTauCaloRoiUpdater
    algo                               = TrigTauCaloRoiUpdater("TauCaloRoiUpdater")
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
    algo.Key_trigTauJetOutputContainer = recordable("HLT_TrigTauRecMerged_CaloOnly")
    algo.Key_trigTauTrackOutputContainer = "HLT_tautrack_dummy"
    return algo

def _algoTauTrackRoiUpdater(inputRoIs, tracks):
    from TrigTauHypo.TrigTauHypoConf import TrigTauTrackRoiUpdater
    algo                               = TrigTauTrackRoiUpdater("TrackRoiUpdater")
    algo.RoIInputKey                   = inputRoIs
    algo.RoIOutputKey                  = "UpdatedTrackRoI"
    algo.fastTracksKey                 = tracks
    algo.Key_trigTauJetInputContainer  = ""
    return algo

def _algoTauTrackBDTRoiUpdater(inputRoIs, tracks):
    from TrigTauHypo.TrigTauHypoConf import TrigTauTrackRoiUpdater
    algo                               = TrigTauTrackRoiUpdater("TrackRoiUpdaterBDT")
    algo.RoIInputKey                   = inputRoIs
    algo.RoIOutputKey                  = "UpdatedTrackBDTRoI"
    algo.fastTracksKey                 = tracks
    algo.useBDT                        = True
    algo.Key_trigTauJetInputContainer  = "HLT_TrigTauRecMerged_CaloOnly"
    return algo

def _algoTauPreselection(inputRoIs, tracks, step):
    from TrigTauRec.TrigTauRecConfigMT import TrigTauRecMerged_TauPreselection
    algo                                 = TrigTauRecMerged_TauPreselection(name= "TrigTauRecMerged_TauPreselection_"+step)
    algo.RoIInputKey                     = inputRoIs
    algo.L1RoIKey                        = "HLT_TAURoI"
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
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig

    algo                                 = TrigTauRecMerged_TauPrecision(name= "TrigTauRecMerged_TauPrecision_"+step)
    algo.RoIInputKey                     = inputRoIs
    algo.L1RoIKey                        = "HLT_TAURoI"
    algo.clustersKey                     = ""
    algo.Key_vertexInputContainer        = getInDetTrigConfig( "tauIso" ).vertex
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
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    algname = "TrigTauRecMerged_PrecisionMVA"
    
    algo                                 = TrigTauRecMerged_TauPrecisionMVA(name= algname, doMVATES=True, doTrackBDT=False, doRNN=True,doLLP=False) 
    algo.RoIInputKey                     = inputRoIs
    algo.L1RoIKey                        = "HLT_TAURoI"
    algo.clustersKey                     = ""
    algo.Key_vertexInputContainer        = getInDetTrigConfig( "tauIso" ).vertex
    algo.Key_trigTauJetInputContainer    = "HLT_TrigTauRecMerged_CaloOnly"
    algo.Key_trackPartInputContainer     = tracks
    algo.Key_trigTauTrackInputContainer  = "HLT_tautrack_dummy"
    algo.Key_trigTauJetOutputContainer   = recordable("HLT_TrigTauRecMerged_MVA")
    algo.Key_trigTauTrackOutputContainer = recordable("HLT_tautrack_MVA")
    algo.Key_trigJetSeedOutputKey        = recordable("HLT_jet_seed")
    return algo

def _algoTauPrecisionLLP(inputRoIs, tracks, step):
    from TrigTauRec.TrigTauRecConfigMT import TrigTauRecMerged_TauPrecisionMVA
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    algname = "TrigTauRecMerged_PrecisionLLP"

    algo                                 = TrigTauRecMerged_TauPrecisionMVA(name= algname, doMVATES=True, doTrackBDT=False, doRNN=False,doLLP=True) 
    algo.RoIInputKey                     = inputRoIs
    algo.L1RoIKey                        = "HLT_TAURoI"
    algo.clustersKey                     = ""
    algo.Key_vertexInputContainer        = getInDetTrigConfig( "tauIso" ).vertex
    algo.Key_trigTauJetInputContainer    = "HLT_TrigTauRecMerged_CaloOnly"
    algo.Key_trackPartInputContainer     = tracks
    algo.Key_trigTauTrackInputContainer  = "HLT_tautrack_dummy"
    algo.Key_trigTauJetOutputContainer   = recordable("HLT_TrigTauRecMerged_LLP")
    algo.Key_trigTauTrackOutputContainer = recordable("HLT_tautrack_LLP")
    algo.Key_trigJetSeedOutputKey        = recordable("HLT_jet_seed")
    return algo

def tauCaloRecoSequence(InViewRoIs, SeqName):
    global TauCaloJetContainer
    # lc sequence
    (lcTopoInViewSequence, lcCaloSequenceOut) = RecoFragmentsPool.retrieve(HLTLCTopoRecoSequence, None, RoIs=InViewRoIs)
    tauCaloRoiUpdaterAlg                      = _algoTauRoiUpdater(inputRoIs = InViewRoIs, clusters = lcCaloSequenceOut)
    updatedRoIs                               = tauCaloRoiUpdaterAlg.RoIOutputKey
    tauCaloOnlyAlg                            = _algoTauCaloOnly(L1RoIs = InViewRoIs,inputRoIs   = updatedRoIs, clusters = lcCaloSequenceOut)
    RecoSequence                              = parOR( SeqName, [lcTopoInViewSequence,tauCaloRoiUpdaterAlg,tauCaloOnlyAlg] )
    return (RecoSequence, tauCaloOnlyAlg.Key_trigTauJetOutputContainer)

def tauCaloMVARecoSequence(InViewRoIs, SeqName):
    global TauCaloJetContainer
    # lc sequence
    (lcTopoInViewSequence, lcCaloSequenceOut) = RecoFragmentsPool.retrieve(HLTLCTopoRecoSequence, None, RoIs=InViewRoIs)
    tauCaloRoiUpdaterAlg                      = _algoTauRoiUpdater(inputRoIs = InViewRoIs, clusters = lcCaloSequenceOut)
    updatedRoIs                               = tauCaloRoiUpdaterAlg.RoIOutputKey
    tauCaloOnlyMVAAlg	                      = _algoTauCaloOnlyMVA(L1RoIs = InViewRoIs,inputRoIs = updatedRoIs, clusters = lcCaloSequenceOut)
    RecoSequence                              = parOR( SeqName, [lcTopoInViewSequence,tauCaloRoiUpdaterAlg,tauCaloOnlyMVAAlg] )
    return (RecoSequence, tauCaloOnlyMVAAlg.Key_trigTauJetOutputContainer)

def tauCaloSequence(ConfigFlags):
    """ Creates L2 Fast Calo sequence for Taus"""
    # EV creator
    InViewRoIs                           = "HLT_TAURoI"
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
    tauCaloRecoVDV.DataObjects = [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+HLT_TAURoI' ),
                                  ( 'CaloBCIDAverage' , 'StoreGateSvc+CaloBCIDAverage' ),
                                  ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                                  ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.actualInteractionsPerCrossing' ),
                                  ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.averageInteractionsPerCrossing' )]
    tauCaloInViewSequence += tauCaloRecoVDV

    tauCaloSequence = seqAND("tauCaloSequence", [tauCaloViewsMaker, tauCaloInViewSequence ])
    return (tauCaloSequence, tauCaloViewsMaker, sequenceOut)    

def tauCaloMVASequence(ConfigFlags):
    """ Creates L2 Fast Calo sequence for Taus"""
    # EV creator
    InViewRoIs                              = "HLT_TAURoI"
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
    tauCaloMVARecoVDV.DataObjects = [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+HLT_TAURoI' ),
                                     ( 'CaloBCIDAverage' , 'StoreGateSvc+CaloBCIDAverage' ),
                                     ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                                     ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.actualInteractionsPerCrossing' ),
                                     ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.averageInteractionsPerCrossing' )]
    tauCaloMVAInViewSequence += tauCaloMVARecoVDV

    tauCaloMVASequence = seqAND("tauCaloMVASequence", [tauCaloMVAViewsMaker, tauCaloMVAInViewSequence ])
    return (tauCaloMVASequence, tauCaloMVAViewsMaker, sequenceOut)

def preSelSequence( RoIs, name):

    tauPreSelSequence = parOR(name)

    signatureName, signatureNameID = _getTauSignatureShort( name )
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( signatureNameID )

    ViewVerifyPreSel = CfgMgr.AthViews__ViewDataVerifier("tauPSViewDataVerifier_"+signatureName)
    ViewVerifyPreSel.DataObjects = [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+HLT_TAURoI'    ),
                                    ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % RoIs      ),
                                    ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.averageInteractionsPerCrossing'   ),
                                    ( 'xAOD::TauTrackContainer' , 'StoreGateSvc+HLT_tautrack_dummy' ),
                                    ( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloOnly' )]


    tauPreSelSequence += ViewVerifyPreSel

    if "TrackInView" in name:
      tauPreselectionAlg = _algoTauPreselection(inputRoIs = RoIs, tracks = IDTrigConfig.tracks_FTF(), step = "Track")
      ViewVerifyPreSel.DataObjects += [( 'xAOD::TrackParticleContainer' , 'StoreGateSvc+HLT_IDTrack_Tau_FTF' )]
    if "TrackTwo" in name:
      tauPreselectionAlg = _algoTauPreselection(inputRoIs = RoIs, tracks = IDTrigConfig.tracks_FTF(), step = "TrackTwo")
      ViewVerifyPreSel.DataObjects += [( 'xAOD::TrackParticleContainer' , 'StoreGateSvc+HLT_IDTrack_TauIso_FTF' )]

    tauPreSelSequence += tauPreselectionAlg

    sequenceOut = tauPreselectionAlg.Key_trigTauJetOutputContainer

    return tauPreSelSequence, sequenceOut


def tauIdSequence( RoIs, name):

    tauIdSequence = parOR(name)

    signatureName, signatureNameID = _getTauSignatureShort( name )
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( signatureNameID )

    ViewVerifyId = CfgMgr.AthViews__ViewDataVerifier("tauIdViewDataVerifier_"+signatureName)
    ViewVerifyId.DataObjects = [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+HLT_TAURoI'    ),
                                ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % RoIs      ),
                                ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.averageInteractionsPerCrossing'   ),
                                ( 'xAOD::VertexContainer', 'StoreGateSvc+'+getInDetTrigConfig( "tauIso" ).vertex),
                                ( 'xAOD::TauTrackContainer' , 'StoreGateSvc+HLT_tautrack_dummy' ),
                                ( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloOnly' ),
                                ( 'xAOD::TrackParticleContainer' , 'StoreGateSvc+'+IDTrigConfig.tracks_IDTrig() )]

    tauIdSequence+= ViewVerifyId

    tauPrecisionAlg = ""

    if "TrackInView" in name:
      tauPrecisionAlg = _algoTauPrecision(inputRoIs = RoIs, tracks = IDTrigConfig.tracks_IDTrig(), step = "Track")
      ViewVerifyId.DataObjects += [( 'xAOD::TauTrackContainer' , 'StoreGateSvc+HLT_tautrack_Presel'),
                                   ( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_Presel' )]
    elif "TrackTwo" in name:
      tauPrecisionAlg = _algoTauPrecision(inputRoIs = RoIs, tracks = IDTrigConfig.tracks_IDTrig(), step = "TrackTwo")
      ViewVerifyId.DataObjects += [( 'xAOD::TauTrackContainer' , 'StoreGateSvc+HLT_tautrack_Presel'),
                                   ( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_Presel' )]
    elif "MVA" in name:
      tauPrecisionAlg = _algoTauPrecisionMVA(inputRoIs = RoIs, tracks = IDTrigConfig.tracks_IDTrig(), step = "PrecisionMVA")
    elif "LLP" in name:
      tauPrecisionAlg = _algoTauPrecisionLLP(inputRoIs = RoIs, tracks = IDTrigConfig.tracks_IDTrig(), step = "PrecisionLLP")

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
                                 ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+HLT_TAURoI' ),
                                 ( 'xAOD::TauTrackContainer' , 'StoreGateSvc+HLT_tautrack_dummy' ),
                                 ( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloOnly' ),    
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
    
    from TrigInDetConfig.InDetPT import makeInDetPrecisionTracking
    #When run in a different view than FTF some data dependencies needs to be loaded through verifier
    #Pass verifier as an argument and it will automatically append necessary DataObjects@NOTE: Don't provide any verifier if loaded in the same view as FTF
    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( config = IDTrigConfig, verifier = ViewVerifyTrk, rois = RoIs )

    from TrigInDetConfig.TrigInDetPriVtxConfig import makeVertices
    vtxAlg = makeVertices( whichSignature       = signatureName, 
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

    from TrigInDetConfig.InDetSetup import makeInDetAlgs
    viewAlgs, viewVerify = makeInDetAlgs( config = IDTrigConfig, rois = RoIs )

    TrackCollection = IDTrigConfig.trkTracks_FTF()

    viewVerify.DataObjects += [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % RoIs ),
                               ( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloOnly')] 

    if 'Core' in signatureName:
      tauTrackRoiUpdaterAlg = _algoTauTrackRoiUpdater(inputRoIs = RoIs, tracks = TrackCollection)
      tauTrackRoiUpdaterAlgBDT = _algoTauTrackBDTRoiUpdater(inputRoIs = RoIs, tracks = TrackCollection)
      viewAlgs.append(tauTrackRoiUpdaterAlgBDT)
      viewAlgs.append(tauTrackRoiUpdaterAlg)

    tauFTFSequence += viewAlgs

    sequenceOut = TrackCollection

    return tauFTFSequence, sequenceOut


# ===============================================================================================                                                           
#   Reco sequence for FTFTau (ptOnly, track)                                                                  
# ===============================================================================================                                                            

def tauFTFTauSequence(ConfigFlags):

    RecoSequenceName                    = "tauFTFTrackInViewSequence"

    newRoITool                          = ViewCreatorFetchFromViewROITool()
    newRoITool.RoisWriteHandleKey       = recordable("HLT_Roi_Tau") #RoI collection recorded to EDM                                                     
    newRoITool.InViewRoIs               = "UpdatedCaloRoI" #input RoIs from calo only step                                                                  

    ftfTauViewsMaker                    = EventViewCreatorAlgorithm("IMFTFTau")
    ftfTauViewsMaker.mergeUsingFeature  = True
    ftfTauViewsMaker.RoITool            = newRoITool
    ftfTauViewsMaker.InViewRoIs         = "RoiForTau"
    ftfTauViewsMaker.Views              = "TAUFTFTauViews"
    ftfTauViewsMaker.ViewFallThrough    = True
    ftfTauViewsMaker.RequireParentView  = True
    ftfTauViewsMaker.ViewNodeName       = RecoSequenceName

    (tauFTFTauInViewSequence, sequenceOut) = tauFTFSequence( ftfTauViewsMaker.InViewRoIs, RecoSequenceName)

    tauFastTrackTauSequence = seqAND("tauFastTrackTauSequence", [ftfTauViewsMaker, tauFTFTauInViewSequence ])
    return (tauFastTrackTauSequence, ftfTauViewsMaker, sequenceOut)

# ===============================================================================================                                                           
#   Reco sequence for FTFTauCore + TrackRoIUpdater Alg (tracktwo, tracktwoMVA)                                                                  
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
#   Reco sequence for FTFTauIso (tracktwo, tracktwoMVA)                                                                  
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
#   Reco sequence for PreSelection algorithm (track)                                                                                
# ===============================================================================================                                                            

def tauPreSelSequence(ConfigFlags):

    RecoSequenceName                      = "preSelTrackInViewSequence"

    tauPreSelViewsMaker                   = EventViewCreatorAlgorithm("IMPreSel")
    tauPreSelViewsMaker.RoIsLink          = "roi"
    tauPreSelViewsMaker.RoITool           = ViewCreatorPreviousROITool()
    tauPreSelViewsMaker.InViewRoIs        = "RoiForTau"
    tauPreSelViewsMaker.Views             = "TAUPreSelViews"
    tauPreSelViewsMaker.ViewFallThrough   = True
    tauPreSelViewsMaker.RequireParentView = True
    tauPreSelViewsMaker.ViewNodeName      = RecoSequenceName

    (tauPreSelTrackInViewSequence, sequenceOut) = preSelSequence( tauPreSelViewsMaker.InViewRoIs, RecoSequenceName)

    tauPreSelTrkSequence = seqAND("tauPreSelTrkSequence", [tauPreSelViewsMaker, tauPreSelTrackInViewSequence ])
    return (tauPreSelTrkSequence, tauPreSelViewsMaker, sequenceOut)


# ===============================================================================================                                                            
#   Reco sequence for PreSelection algorithm (tracktwo)                                                                                                
# ===============================================================================================                                                            

def tauPreSelTTSequence(ConfigFlags):

    RecoSequenceName                      = "preSelTrackTwoInViewSequence"

    tauPreSelViewsMaker                   = EventViewCreatorAlgorithm("IMPreSel2")
    tauPreSelViewsMaker.RoIsLink          = "roi"
    tauPreSelViewsMaker.RoITool           = ViewCreatorPreviousROITool()
    tauPreSelViewsMaker.InViewRoIs        = "RoiForTauIso"
    tauPreSelViewsMaker.Views             = "TAUPreSelTTViews"
    tauPreSelViewsMaker.ViewFallThrough   = True
    tauPreSelViewsMaker.RequireParentView = True
    tauPreSelViewsMaker.ViewNodeName      = RecoSequenceName

    (tauPreSelTrackInViewSequence, sequenceOut) = preSelSequence( tauPreSelViewsMaker.InViewRoIs, RecoSequenceName)

    tauPreSelTTTrkSequence = seqAND("tauPreSelTTTrkSequence", [tauPreSelViewsMaker, tauPreSelTrackInViewSequence ])
    return (tauPreSelTTTrkSequence, tauPreSelViewsMaker, sequenceOut)


# ===============================================================================================                                                            
#   Reco sequence for Precision tracking (from PreSelection algorithm)   (track)                           
# ===============================================================================================                                                            

def tauPrecTrackSequence(ConfigFlags):

    RecoSequenceName                    = "precFTFTauPrecInViewSequence"

    tauPrecViewsMaker                   = EventViewCreatorAlgorithm("IMPrecTrack")
    tauPrecViewsMaker.RoIsLink          = "roi"
    tauPrecViewsMaker.RoITool           = ViewCreatorPreviousROITool()
    tauPrecViewsMaker.InViewRoIs        = "RoiForTau"
    tauPrecViewsMaker.Views             = "TAUPrecViews"
    tauPrecViewsMaker.ViewFallThrough   = True
    tauPrecViewsMaker.RequireParentView = True
    tauPrecViewsMaker.ViewNodeName      = RecoSequenceName

    (tauPrecTrackInViewSequence, sequenceOut) = precTrackSequence( tauPrecViewsMaker.InViewRoIs, RecoSequenceName)

    tauPrecTrkSequence = seqAND("tauPrecTrkSequence", [tauPrecViewsMaker, tauPrecTrackInViewSequence ])
    return (tauPrecTrkSequence, tauPrecViewsMaker, sequenceOut)

# ===============================================================================================                                                            
#   Reco sequence for Precision tracking (from FTF Iso or PreSelection algorithm)   (tracktwo, tracktwoMVA)                           
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
#    Reco sequence for Tau Precision Alg   (track)                                                              
# ===============================================================================================  

def tauTrackSequence(ConfigFlags):

    RecoSequenceName                     = "tauTrackInViewSequence"

    ftfTrackViewsMaker                   = EventViewCreatorAlgorithm("IMFTFTrack")
    ftfTrackViewsMaker.RoIsLink          = "roi"
    ftfTrackViewsMaker.RoITool           = ViewCreatorPreviousROITool()
    ftfTrackViewsMaker.InViewRoIs        = "RoiForTau"
    ftfTrackViewsMaker.Views             = "TAUFTFTrackViews"
    ftfTrackViewsMaker.ViewFallThrough   = True
    ftfTrackViewsMaker.RequireParentView = True
    ftfTrackViewsMaker.ViewNodeName      = RecoSequenceName

    (tauTrackInViewSequence, sequenceOut) = tauIdSequence( ftfTrackViewsMaker.InViewRoIs, RecoSequenceName)

    tauFastTrackSequence = seqAND("tauFastTrackSequence", [ftfTrackViewsMaker, tauTrackInViewSequence ])
    return (tauFastTrackSequence, ftfTrackViewsMaker, sequenceOut)

# ===============================================================================================                                                            
#    Reco sequence for Tau Precision Alg (tracktwo)
# ===============================================================================================  

def tauTrackTwoSequence(ConfigFlags):

    RecoSequenceName                        = "tauTrackTwoInViewSequence"

    ftfTrackTwoViewsMaker                   = EventViewCreatorAlgorithm("IMTrackTwo")
    ftfTrackTwoViewsMaker.RoIsLink          = "roi"
    ftfTrackTwoViewsMaker.RoITool           = ViewCreatorPreviousROITool()
    ftfTrackTwoViewsMaker.InViewRoIs        = "RoiForTauCore"
    ftfTrackTwoViewsMaker.Views             = "TAUTrackTwoViews"
    ftfTrackTwoViewsMaker.ViewFallThrough   = True
    ftfTrackTwoViewsMaker.RequireParentView = True
    ftfTrackTwoViewsMaker.ViewNodeName      = RecoSequenceName

    (tauTrackTwoInViewSequence, sequenceOut) = tauIdSequence( ftfTrackTwoViewsMaker.InViewRoIs, RecoSequenceName)

    tauFastTrackTwoSequence = seqAND("tauFastTrackTwoSequence", [ftfTrackTwoViewsMaker, tauTrackTwoInViewSequence ])
    return (tauFastTrackTwoSequence, ftfTrackTwoViewsMaker, sequenceOut)

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
