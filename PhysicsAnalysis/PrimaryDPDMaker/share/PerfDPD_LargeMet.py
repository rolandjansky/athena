##-----------------------------------------------------------------------------
## Name: PrimaryDPD_Stream.py
##
## Author: Ulla Blumenschein (Goettingen)
## Email:  Ulla.Blumenschein@cern.ch
##
## Based on:  PrimaryDPD_PhotonLargeMetStream.py by Karsten Koeneke (DESY)
##
## Description: This defines the content of the LargeMet DPD output stream.
##
##-----------------------------------------------------------------------------

# If this file is already included, don't include it again
include.block("PrimaryDPDMaker/PerfDPD_LargeMet.py")


## for messaging
from AthenaCommon.Logging import logging
metDPDStream_msg = logging.getLogger( 'PrimaryDPD_LargeMetStream' )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags_LargeMetStream import primLargeMetDPD
from RecExConfig.RecFlags import rec

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr 



##====================================================================
## Write the used options to the log file
##====================================================================
if rec.OutputLevel() <= INFO:
    metDPDStream_msg.info('Values of all PerfDPD_LargeMet flags:')
    print primLargeMetDPD
    pass




# ============================================================================
# Create a sub-sequence where all needed algorithms will run in
# ============================================================================
import AthenaCommon.CfgMgr as CfgMgr
desdMissingEtSequence = CfgMgr.AthSequencer( 'DESDM_MET_Sequence',
                                             StopOverride = True )
topSequence += desdMissingEtSequence






##============================================================================
## Define the skimming (event selection) for the DESD_MET  output stream
##============================================================================
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler


from D2PDMaker.D2PDMakerConf import D2PDMissingEtSelector
desdMissingEtSequence += D2PDMissingEtSelector( "LargeMetStream_HighMetSigFilter",
                                      OutputLevel              = INFO,
                                      inputCollection          = 'MET_LocHadTopo',
                                      outputCollection         = 'DESDM_METStream_HighMetNeutrinoCollection',
                                      minNumberPassed          = 1,
                                      missingEtSignificanceMin = 5.0
                                      )

desdMissingEtSequence += D2PDMissingEtSelector( "LargeMetStream_LowMetSigFilter",
                                      OutputLevel              = INFO,
                                      inputCollection          = 'MET_LocHadTopo',
                                      outputCollection         = 'DESDM_METStream_LowMetNeutrinoCollection',
                                      minNumberPassed          = 1,
                                      missingEtSignificanceMax = 5.0
                                      )


from PrimaryDPDMaker.TauFilter import TauFilter


# Tauid 0,1,2 : loose, medium, tight
# 10,11,12: Safe
# 13,14,15: Safe Calo


desdMissingEtSequence += TauFilter("LargeMetStream_WTauFilter")
desdMissingEtSequence.LargeMetStream_WTauFilter.tauCollectionName = "TauRecContainer"    
desdMissingEtSequence.LargeMetStream_WTauFilter.cutTauEtMin       = primLargeMetDPD.WTauFilterEt()*Units.GeV
desdMissingEtSequence.LargeMetStream_WTauFilter.doSelection       = primLargeMetDPD.WTauFilterPresel()
desdMissingEtSequence.LargeMetStream_WTauFilter.cutTauId1          = 11
desdMissingEtSequence.LargeMetStream_WTauFilter.cutTauId2          = 11
if primLargeMetDPD.WTauFilterOptID():
    desdMissingEtSequence.LargeMetStream_WTauFilter.cutNTrk            = -1
    desdMissingEtSequence.LargeMetStream_WTauFilter.cutEMrad1          = 0.08
    desdMissingEtSequence.LargeMetStream_WTauFilter.cutEMrad3          = 0.15
    desdMissingEtSequence.LargeMetStream_WTauFilter.cutRtrack1         = 0.09
    desdMissingEtSequence.LargeMetStream_WTauFilter.cutRtrack3         = 0.12
    desdMissingEtSequence.LargeMetStream_WTauFilter.cutFtrack1         = 0.06
    desdMissingEtSequence.LargeMetStream_WTauFilter.cutFtrack3         = 0.12


runningIndex = 0
TriggerNames = {}
LMetTrigCmdString=""

if primLargeMetDPD.ApplyTriggerSkimming() :
    # Have to include this jobOption to set up the trigger info!!!
    include("PrimaryDPDMaker/SetupTrigDecisionTool.py")

    # Now, define the trigger filters
    from PrimaryDPDMaker.TriggerFilter import TriggerFilter

    globalTriggerPrescale = primLargeMetDPD.GlobalTriggerPrescale


    for k, v in primLargeMetDPD.TauTriggerNames.iteritems():
        TriggerNames[k] = v
        pass
    pass

    for trigName in TriggerNames.keys() :
        runningIndex += 1
        TriggerFilterName = "LargeMetStream_TriggerFilter_"+trigName
        desdMissingEtSequence += TriggerFilter( TriggerFilterName,
                                      trigger = trigName )
        prescale = globalTriggerPrescale*TriggerNames[trigName] if TriggerNames[trigName]>0 else 1
        TriggerPrescaleName = "LargeMetStream_Prescale_"+trigName
        desdMissingEtSequence += PrimaryDPDPrescaler(TriggerPrescaleName,
                                           AcceptAlgs = [TriggerFilterName],
                                           Prescale   =  prescale)
        LMetTrigCmdString += TriggerPrescaleName
        if not runningIndex == TriggerNames.__len__() :
            LMetTrigCmdString += " or "
            pass
        pass
    pass
pass





# Define the prescaled filters

desdMissingEtSequence += PrimaryDPDPrescaler( "LargeMetStream_HighMetSigPrescaler",
                               AcceptAlgs = ["LargeMetStream_HighMetSigFilter"],
                               Prescale   = primLargeMetDPD.HighMetSigFilterPrescale() )

desdMissingEtSequence += PrimaryDPDPrescaler( "LargeMetStream_LowMetSigPrescaler",
                               AcceptAlgs = ["LargeMetStream_LowMetSigFilter"],
                               Prescale   = primLargeMetDPD.LowMetSigFilterPrescale() )


desdMissingEtSequence += PrimaryDPDPrescaler( "LargeMetStream_WMetPrescaler",
                               AcceptAlgs = ["LargeMetStream_WMetFilter"],
                               Prescale   = primLargeMetDPD.WMetFilterPrescale() )


desdMissingEtSequence += PrimaryDPDPrescaler( "LargeMetStream_WTauPrescaler",
                               AcceptAlgs = ["LargeMetStream_WTauFilter"],
                               Prescale   = primLargeMetDPD.WTauFilterPrescale() )



# Create a subsequence: Only when the first algorithm returns isEventAccepted, the rest is executed
from AthenaCommon.AlgSequence import AthSequencer
thinSeqDESDM_MET = AthSequencer("DESDM_MET_ThinningSequence")
desdMissingEtSequence += thinSeqDESDM_MET

# Create the combined decission
thinSeqDESDM_MET += LogicalFilterCombiner("StreamDESD_MET_AcceptEvent")
thinSeqDESDM_MET.StreamDESD_MET_AcceptEvent.cmdstring = "LargeMetStream_HighMetSigPrescaler or LargeMetStream_LowMetSigPrescaler"
if primLargeMetDPD.ApplyTriggerSkimming() :
    thinSeqDESDM_MET.StreamDESD_MET_AcceptEvent.cmdstring+=" or "
    thinSeqDESDM_MET.StreamDESD_MET_AcceptEvent.cmdstring+=LMetTrigCmdString

from PrimaryDPDMaker.PrimaryDPDFlags import desdEventSkimmingFilterNamesList
desdEventSkimmingFilterNamesList.append( "StreamDESD_MET_AcceptEvent" )



##====================================================================
## Define the thinning for the LargeMet DPD output stream
##====================================================================
if primDPD.ApplyThinning() and primLargeMetDPD.ApplyThinning() and not primLargeMetDPD.WriteFullESD() and not primDPD.WriteLargeMetStream.isVirtual and not primDPD.isVirtual() :
    from PrimaryDPDMaker.ThinObjects import *

    #---------------------------------------------------
    # PixelCluster, and SCT_Cluster Thinning (ESD)
    #---------------------------------------------------
    if primLargeMetDPD.ApplyPixelClusterThinning() \
       or primLargeMetDPD.ApplySCTClusterThinning() :
        from PrimaryDPDMaker.PrimaryDPDMakerConf import ThinContainers
        thinSeqDESDM_MET += ThinContainers("ThinContainersInLargeMetStream")
        thinSeqDESDM_MET.ThinContainersInLargeMetStream.thinSvc                     = 'ThinningSvc/LargeMetStreamThinning'

        if rec.readESD() and primLargeMetDPD.ApplyPixelClusterThinning() :
            thinSeqDESDM_MET.ThinContainersInLargeMetStream.thinPixelClusters           = True
            thinSeqDESDM_MET.ThinContainersInLargeMetStream.pixelClusterCollectionName  = 'PixelClusters'
            thinSeqDESDM_MET.ThinContainersInLargeMetStream.maxDeltaEtaPixelTauObject = 0.5
            thinSeqDESDM_MET.ThinContainersInLargeMetStream.maxDeltaPhiPixelTauObject = 0.5
            thinSeqDESDM_MET.ThinContainersInLargeMetStream.maxDeltaZPixelTauObject   = 200            
            pass
        if rec.readESD() and primLargeMetDPD.ApplySCTClusterThinning() :
            thinSeqDESDM_MET.ThinContainersInLargeMetStream.thinSCTClusters             = True
            thinSeqDESDM_MET.ThinContainersInLargeMetStream.sctClusterCollectionName    = 'SCT_Clusters'
            thinSeqDESDM_MET.ThinContainersInLargeMetStream.maxDeltaEtaSCTTauObject = 0.5
            thinSeqDESDM_MET.ThinContainersInLargeMetStream.maxDeltaPhiSCTTauObject = 0.5
            thinSeqDESDM_MET.ThinContainersInLargeMetStream.maxDeltaZSCTTauObject   = 200
            pass
         
        thinSeqDESDM_MET.ThinContainersInLargeMetStream.keepObjectsNearTaus         = True
        thinSeqDESDM_MET.ThinContainersInLargeMetStream.minEtTau                    = 15.0*Units.GeV
        thinSeqDESDM_MET.ThinContainersInLargeMetStream.maxDeltaRTauObject          = 0.45
        pass

    if rec.readESD() and primLargeMetDPD.ApplyTRTDriftCircleThinning():
        from PrimaryDPDMaker.PrimaryDPDMakerConf import ThinContainers
        thinSeqDESDM_MET += ThinContainers("ThinTrtInLargeMetStream")
        thinSeqDESDM_MET.ThinTrtInLargeMetStream.thinSvc                        = 'ThinningSvc/LargeMetStreamThinning'
        thinSeqDESDM_MET.ThinTrtInLargeMetStream.thinTRTDriftCircles            = True
        thinSeqDESDM_MET.ThinTrtInLargeMetStream.trtDriftCirclesCollectionName  = 'TRT_DriftCircles'        
        thinSeqDESDM_MET.ThinTrtInLargeMetStream.keepObjectsNearTaus            = True        
        thinSeqDESDM_MET.ThinTrtInLargeMetStream.minEtTau                    = 15.0*Units.GeV
        thinSeqDESDM_MET.ThinTrtInLargeMetStream.maxDeltaEtaTauObject        = 0.45
        thinSeqDESDM_MET.ThinTrtInLargeMetStream.maxDeltaPhiTauObject        = 0.45
        thinSeqDESDM_MET.ThinTrtInLargeMetStream.useDeltaEtaPhiTau           = True        
        thinSeqDESDM_MET.ThinTrtInLargeMetStream.maxDeltaEtaTRTTauObject = 0.5
        thinSeqDESDM_MET.ThinTrtInLargeMetStream.maxDeltaPhiTRTTauObject = 0.5
        thinSeqDESDM_MET.ThinTrtInLargeMetStream.maxDeltaZTRTTauObject   = 200
        pass

    
    if rec.readESD() and primLargeMetDPD.ApplyTrackThinning():
        from PrimaryDPDMaker.PrimaryDPDMakerConf import ThinContainers
        thinSeqDESDM_MET += ThinContainers("ThinTracksInLargeMetStream")
        thinSeqDESDM_MET.ThinTracksInLargeMetStream.thinSvc                     = 'ThinningSvc/LargeMetStreamThinning'
        thinSeqDESDM_MET.ThinTracksInLargeMetStream.thinTracks                  = True
        thinSeqDESDM_MET.ThinTracksInLargeMetStream.tracksCollectionName        = 'Tracks'
        thinSeqDESDM_MET.ThinTracksInLargeMetStream.keepObjectsNearTaus         = True
        thinSeqDESDM_MET.ThinTracksInLargeMetStream.minEtTau                    = 15.0*Units.GeV
        thinSeqDESDM_MET.ThinTracksInLargeMetStream.maxDeltaRTauObject          = 0.45
        pass

    #---------------------------------------------------
    # L1Calo Trigger Tower Thinner (only available on ESD)
    #---------------------------------------------------
    if rec.readESD() and  primLargeMetDPD.ApplyTriggerTowerThinning() :
        from TrigT1CaloCalibTools.TrigT1CaloCalibToolsConf import TriggerTowerThinningAlg
        thinSeqDESDM_MET += TriggerTowerThinningAlg( "TriggerTowerThinnerInLargeMetStream",
                                                     OutputLevel       = INFO,
                                                     ThinService       = 'ThinningSvc/LargeMetStreamThinning',
                                                     MinADC            = 36,
                                                     MinCaloCellEnergy = 0.8,
                                                     UseRandom         = True,
                                                     MinRandom         = 0.01 )
        pass

    pass




##====================================================================
## Define the LargeMet DPD output stream
##====================================================================

streamName = primDPD.WriteLargeMetStream.StreamName
fileName   = buildFileName( primDPD.WriteLargeMetStream )
if primDPD.WriteLargeMetStream.isVirtual or primDPD.isVirtual() :
    LargeMetStream=MSMgr.NewVirtualStream( streamName, fileName )
    pass
else:
    LargeMetStream=MSMgr.NewPoolStream( streamName, fileName )
    pass

if primDPD.ApplySkimming() and primLargeMetDPD.ApplySkimming():
    LargeMetStream.AcceptAlgs( [ "StreamDESD_MET_AcceptEvent" ] )
    pass



#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList

#excludeList = dpdOutput.excludeESDOnlyContainersList()
#ExcludeList=[]
excludeList = [
    'CaloShowerContainer#EMTopoCluster430_Data',
    'CaloCellLinkContainer#EMTopoCluster430_Link',
    'MdtCsmContainer#MDTCSM',
    'RpcPadContainer#RPCPAD',
    'Muon::MdtPrepDataContainer#MDT_DriftCircles',
    'InDetRawDataContainer#TRT_RDOs',
    'TrackCollection#StandaloneTRTTracks',
    'InDet::TRT_DriftCircleContainer#TRT_DriftCircles',
    'TrackCollection#ResolvedPixelTracks',
    'InDetRawDataContainer#TRT_RDOs',
    'TRT_RDO_Container#TRT_RDOs',
    'SCT_RawDataContainer#SCT_RDOs',
    'SCT_RDO_Container#SCT_RDOs',
    'InDetRawDataContainer#PixelRDOs',
    'PixelRDO_Container#PixelRDOs',
    'InDet::PixelClusterContainer#PixelClusters',
    'InDet::SCT_ClusterContainer#SCT_Clusters',
    'InDet::TRT_DriftCircleContainer#TRT_DriftCircles',
    'InDet::PixelGangedClusterAmbiguities#*',
    'Rec::TrackParticleContainer#Combined_TrackParticles',
    'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationCombined_Electron_EFID',
    'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationIOTRT_CosmicsN_EFID',
    'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationTRTOnly_Electron_EFID',
    'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationTRTOnly_FullScan_EFID',
    'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationTRTOnly_Muon_EFID',
    'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationTRTOnly_Tau_EFID',
    'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Bjet_EFID',
    'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Bphysics_EFID',
    'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_CosmicsN_EFID',
    'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Electron_EFID',
    'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_FullScan_EFID',
    'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Muon_EFID',
    'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Photon_EFID',
    'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Tau_EFID',
    'Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_minBias_EFID',
    'Rec::TrackParticleContainer#MooreTrackParticles',
    'Rec::TrackParticleContainer#MuGirlRefittedTrackParticles',
    'Rec::TrackParticleContainer#MuTagIMOTrackParticles',
    'Rec::TrackParticleContainer#MuTagTrackParticles',
    'Rec::TrackParticleContainer#MuidCombTrackParticles',
    'Rec::TrackParticleContainer#MuidExtrTrackParticles',
    'Rec::TrackParticleContainer#MuonboyMuonSpectroOnlyTrackParticles',
    'Rec::TrackParticleContainer#MuonboyTrackParticles',
    'Rec::TrackParticleContainer#StacoTrackParticles',
    'TrackCollection#HLT_InDetTrigTrackSlimmer_Bjet_EFID',
    'TrackCollection#HLT_InDetTrigTrackSlimmer_Bphysics_EFID',
    'TrackCollection#HLT_InDetTrigTrackSlimmer_CosmicsN_EFID',
    'TrackCollection#HLT_InDetTrigTrackSlimmer_Electron_EFID',
    'TrackCollection#HLT_InDetTrigTrackSlimmer_FullScan_EFID',
    'TrackCollection#HLT_InDetTrigTrackSlimmer_Muon_EFID',
    'TrackCollection#HLT_InDetTrigTrackSlimmer_Photon_EFID',
    'TrackCollection#HLT_InDetTrigTrackSlimmer_Tau_EFID',
    'TrackCollection#Combined_Tracks',
    'TrackCollection#ConvertedMBoyMuonSpectroOnlyTracks',
    'TrackCollection#ConvertedMBoyTracks',
    'TrackCollection#ConvertedMuIdCBTracks',
    'TrackCollection#ConvertedMuTagTracks',
    'TrackCollection#ConvertedStacoTracks',
    'TrackCollection#MooreTracks',
    'TrackCollection#MuGirlRefittedTracks',
    'TrackCollection#MuTagIMOTracks',
    'TrackCollection#MuidExtrapolatedTracks',
    #'CaloCellLinkContainer#CaloTopoCluster_Link',
    #'CaloShowerContainer#CaloTopoCluster_Data',
    #'TriggerTowerCollection#TriggerTowers',
    'SegmentCollection#MooreSegments',
    #'CaloClusterContainer#CaloTopoCluster',
    'TrackCollection#Tracks',
    'JetCollection#Cone7H1TowerJets',
    'JetCollection#AntiKt6LCTopoJets',
    'JetCollection#Cone4H1TowerJets',
    'TileL2Container#TileL2Cnt',
    'Trk::SegmentCollection#MooreSegments',
    #'DataVector<LVL1::TriggerTower>#TriggerTowers',
    'DataVector<LVL1::TriggerTower>#TriggerTowersMuon',
    'TileDigitsContainer#TileDigitsFlt',
    'MuonFeatureDetailsContainer#HLT',
    'TrigL2BphysContainer#HLT_L2BMuMuXFex',
    'TrigL2BphysContainer#HLT_L2DiMuXFex',
    'TrigL2BphysContainer#HLT_L2DsPhiPiFexDs',
    'TrigL2BphysContainer#HLT_L2DsPhiPiFexPhi',
    'TrigL2BphysContainer#HLT_L2Jpsiee',
    'TrigL2BphysContainer#L2TrackMass',
    'TrigL2BphysContainer#HLT_TrigDiMuon',
    'Muon::CscStripPrepDataContainer_tlp1_CSC_Measurements',
    'Trk::SegmentCollection#MuGirlSegments',
    'TileRawChannelContainer_p1_TileRawChannelFlt',
    'LArDigitContainer#LArDigitContainer_EMClust',
    'Trk::SegmentCollection#ConvertedMBoySegments',
    'egDetailContainer#egDetailContainer',
    'egDetailContainer#SofteDetailContainer'   
    ]


#dpdOutput.addBasicOutput(streamName)

excludeList = list(set(excludeList))

dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,excludeList)
#dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,"")



##====================================================================
## Finalize the thinning of the containers for this stream
##====================================================================
if primDPD.ApplyThinning() and primLargeMetDPD.ApplyThinning() and not primDPD.WriteLargeMetStream.isVirtual and not primDPD.isVirtual() :
    from AthenaServices.Configurables import ThinningSvc, createThinningSvc

    # Create the thinning service
    augStream=MSMgr.GetStream( primDPD.WriteLargeMetStream.StreamName )
    evtStream=augStream.GetEventStream()
    svcMgr += createThinningSvc( svcName="LargeMetStreamThinning", outStreams=[evtStream] )

    if primLargeMetDPD.ApplyPixelClusterThinning() \
       or primLargeMetDPD.ApplyTRTDriftCircleThinning() \
       or primLargeMetDPD.ApplySCTClusterThinning() :
        
        from RegionSelector.RegionSelectorConf import RegSelSvc
        svcMgr += RegSelSvc("RegSelSvcInLargeMetStream")
        svcMgr.RegSelSvcInLargeMetStream.enableID    = True
        svcMgr.RegSelSvcInLargeMetStream.enablePixel = True
        svcMgr.RegSelSvcInLargeMetStream.enableSCT   = True
        svcMgr.RegSelSvcInLargeMetStream.enableTRT   = True

        # Configure the thinning algorithm to use this RegionSelectorService
        thinSeqDESDM_MET.ThinContainersInLargeMetStream.RegionSelectorTool = svcMgr.RegSelSvcInLargeMetStream
        thinSeqDESDM_MET.ThinTrtInLargeMetStream.RegionSelectorTool        = svcMgr.RegSelSvcInLargeMetStream

        from SCT_Cabling.SCT_CablingConf import SCT_CablingSvc
        svcMgr+=SCT_CablingSvc()
        
        IOVDbSvc = Service("IOVDbSvc")
        from IOVDbSvc.CondDB import conddb
        if not conddb.folderRequested('/SCT/DAQ/Configuration/ROD'):
            conddb.addFolder("SCT","/SCT/DAQ/Configuration/ROD")
        if not conddb.folderRequested('/SCT/DAQ/Configuration/MUR'):
            conddb.addFolder("SCT","/SCT/DAQ/Configuration/MUR")
        if not conddb.folderRequested('/SCT/DAQ/Configuration/RODMUR'):
            conddb.addFolder("SCT","/SCT/DAQ/Configuration/RODMUR")
        if not conddb.folderRequested('/SCT/DAQ/Configuration/Geog'):
            conddb.addFolder("SCT","/SCT/DAQ/Configuration/Geog")
        svcMgr.SCT_CablingSvc.DataSource = "CORACOOL"
        pass
    pass  # End: if primDPD.ApplyThinning()
    
