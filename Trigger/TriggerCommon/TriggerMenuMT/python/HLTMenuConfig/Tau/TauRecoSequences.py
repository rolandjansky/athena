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


#Retrieve short name for tau signature that can be used as suffix to be appended to the names of alg/tools
#Based on these names specific ID config is retrieved
#This utilizes name of the reco sequence from which checks specific string pattern
def _getTauSignatureShort( name ):
    #Do we want to set empty string as dummy? Some assert check instead?
    signature = ""
    if "FTFId" in name:
      signature = 'tauId'
    elif "FTFTrackInView" in name:
      signature = 'tauTrk'
    elif "FTFTrackTwo" in name:
      signature = 'tauTrkTwo'
    elif "FTFIso" in name:
      signature = 'tauIso'
    elif "EF" in name:
      signature = 'tauEF'

    #Append tauCore?
    return signature


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
    algo.Key_trigTauJetOutputContainer = recordable("HLT_TrigTauRecMerged_CaloOnly")
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
    algo.Key_trigTauJetInputContainer    = "HLT_TrigTauRecMerged_CaloOnly"
    algo.Key_trackPartInputContainer     = tracks
    algo.Key_trigTauTrackInputContainer  = "HLT_tautrack_dummy"
    algo.Key_trigTauJetOutputContainer   = recordable("HLT_TrigTauRecMerged_MVA")
    algo.Key_trigTauTrackOutputContainer = recordable("HLT_tautrack_MVA")
    algo.Key_trigJetSeedOutputKey        = recordable("HLT_jet_seed")
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
                                  ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                                  ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.ActIntPerXDecor' ),
                                  ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.AveIntPerXDecor' )]
    tauCaloInViewSequence += tauCaloRecoVDV

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
                                     ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                                     ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.ActIntPerXDecor' ),
                                     ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.AveIntPerXDecor' )]
    tauCaloMVAInViewSequence += tauCaloMVARecoVDV

    tauCaloMVASequence = seqAND("tauCaloMVASequence", [tauCaloMVAViewsMaker, tauCaloMVAInViewSequence ])
    return (tauCaloMVASequence, tauCaloMVAViewsMaker, sequenceOut)

def tauIdTrackSequence( RoIs , name):

    tauIdTrackSequence = seqAND(name)

    signatureName = _getTauSignatureShort( name )
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( signatureName )

    #TODO: are both FTF necessary for taus??
    from TrigInDetConfig.InDetSetup import makeInDetAlgs
    viewAlgs, viewVerify = makeInDetAlgs(config = IDTrigConfig, rois = RoIs)



    from TrigInDetConfig.InDetSetup import makeInDetAlgs
    #FIXME: are both FTF necessary for taus??
    #viewAlgs, viewVerify = makeInDetAlgs( config = IDTrigConfig, rois = RoIs)
    viewAlgs, viewVerify = makeInDetAlgs( config = IDTrigConfig, rois = RoIs, viewVerifier = 'tauViewDataVerifierName')

    viewVerify.DataObjects += [( 'xAOD::TauTrackContainer' , 'StoreGateSvc+HLT_tautrack_dummy' ),
                               ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+' + RoIs ),
                               ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                               ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.AveIntPerXDecor' ),
                               ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.ActIntPerXDecor' ),
                               ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+TAUCaloRoIs' ),
                               ( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloOnly')]

    # Make sure the required objects are still available at whole-event level
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    if globalflags.InputFormat.is_bytestream():
      viewVerify.DataObjects += [( 'IDCInDetBSErrContainer' , 'StoreGateSvc+PixelByteStreamErrs' ),
                                 ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' ) ]
    else:
      topSequence.SGInputLoader.Load += [( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' )]
      viewVerify.DataObjects += [( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' )]

    for viewAlg in viewAlgs:
       tauIdTrackSequence += viewAlg


    if "FTFTrackInView" in name:
      tauPreselectionAlg = _algoTauPreselection(inputRoIs = RoIs, tracks = IDTrigConfig.FT.tracksFTF(), step = "Track")
      tauIdTrackSequence += tauPreselectionAlg
    elif "TrackTwo" in name:
      tauPreselectionAlg = _algoTauPreselection(inputRoIs = RoIs, tracks = IDTrigConfig.FT.tracksFTF(), step = "TrackTwo")
      tauIdTrackSequence += tauPreselectionAlg      

    #Precision Tracking
    PTAlgs = [] #List of precision tracking algs 
    PTTracks = [] #List of TrackCollectionKeys
    PTTrackParticles = [] #List of TrackParticleKeys
    
    from TrigInDetConfig.InDetPT import makeInDetPrecisionTracking
    #When run in a different view than FTF some data dependencies needs to be loaded through verifier
    #Pass verifier as an argument and it will automatically append necessary DataObjects@NOTE: Don't provide any verifier if loaded in the same view as FTF
    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( config = IDTrigConfig, verifier = False, rois = RoIs )
    PTSeq = parOR("precisionTrackingIn"+signatureName, PTAlgs  )

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


    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( 'tauCore' )

    from TrigInDetConfig.InDetSetup import makeInDetAlgs
    viewAlgs, viewVerify = makeInDetAlgs( config = IDTrigConfig, rois = RoIs )

    for viewAlg in viewAlgs:
       if "InDetTrigTrackParticleCreatorAlg" in viewAlg.name():
         TrackCollection = viewAlg.TrackName

    viewVerify.DataObjects += [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+' + RoIs ),
                               ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+PixelByteStreamErrs' ),
                               ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' ),#For some reason not picked up properly
                               ( 'xAOD::TauJetContainer' , 'StoreGateSvc+HLT_TrigTauRecMerged_CaloOnly')] 

    tauTrackRoiUpdaterAlg = _algoTauTrackRoiUpdater(inputRoIs = RoIs, tracks = TrackCollection)

    viewAlgs.append(tauTrackRoiUpdaterAlg)

    tauCoreTrackSequence += viewAlgs

    sequenceOut = TrackCollection

    return tauCoreTrackSequence, sequenceOut

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

def tauFTFCoreSequence(ConfigFlags):

    RecoSequenceName = "tauFTFCoreInViewSequence"

    newRoITool                          = ViewCreatorFetchFromViewROITool()
    newRoITool.RoisWriteHandleKey       = recordable("HLT_Roi_TauCore") #RoI collection recorded to EDM           
    newRoITool.InViewRoIs               = "UpdatedCaloRoI" #input RoIs from calo only step   

    ftfCoreViewsMaker                   = EventViewCreatorAlgorithm("IMFTFCore")
    #ftfCoreViewsMaker.RoIsLink          = "roi"
    ftfCoreViewsMaker.mergeUsingFeature = True
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

