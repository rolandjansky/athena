#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "MUON D3PD part".

# This jobO should not be included more than once:
include.block( "MuonD3PDMaker/MuonD3PD_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
MuonD3PDStream_msg = logging.getLogger( 'MuonD3PD_prodJobOFragment' )

# Check if the configuration makes sense:
if prodFlags.WriteMuonD3PD.isVirtual:
    MuonD3PDStream_msg.error( "The MUON D3PD stream can't be virtual! " +
                              "It's a configuration error!" )
    raise NameError( "MUON D3PD set to be a virtual stream" )
    pass

# Check if there are chain 1 and chain 2
from MuonD3PDMaker.MuonD3PDMakerFlags import MuonD3PDFlags
from D3PDMakerCoreComps.resolveSGKey import testSGKey
MuonD3PDFlags.doNewChainOnly = not (testSGKey ('Analysis::MuonContainer', 'MuidMuonCollection')
                                    and testSGKey ('Analysis::MuonContainer', 'StacoMuonCollection'))

# Pre-sequence for filters....things to run before D3PD making
from AthenaCommon.AlgSequence import AlgSequence
preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())

# filter ID tracks with pT>4 GeV and at least 1 Pixel or SCT hit or hole
from TauD3PDMaker.makeTrackFilterAlg import makeTrackFilterAlg
trackFilter = makeTrackFilterAlg(TracksName = D3PDMakerFlags.TrackSGKey(),
                                 OutputTracksName='HighPtTracks',
                                 ptCut=4000., nSCTPix=0)
preseq += trackFilter

from RecExConfig.RecFlags import rec
# Fit the D3PD SG keys for truth to the filtered ones
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.TruthParticlesSGKey.set_Value_and_Lock('SpclMC_MUON')
D3PDMakerFlags.TruthSGKey.set_Value_and_Lock('GEN_EVENT_MUON')

# Filter Truth to keep only muons and fsr photons
if rec.doTruth():
    # get a handle on the job main sequence
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()    
    from McParticleTools.McParticleToolsConf import McVtxFilterTool
    from McParticleTools.McParticleToolsConf import TruthParticleCnvTool
    from McParticleAlgs.McParticleAlgsConf   import McAodFilter
    topSequence += McAodFilter()
    topSequence.McAodFilter.DoGenAod = True
    topSequence.McAodFilter.McVtxFilterTool = McVtxFilterTool()
    if rec.readESD():
        topSequence.McAodFilter.McVtxFilterTool.McEvents = "TruthEvent"
    if rec.readAOD():
        topSequence.McAodFilter.McVtxFilterTool.McEvents = "GEN_AOD"
    topSequence.McAodFilter.McVtxFilterTool.McEventsOutput = "GEN_EVENT_MUON"
    topSequence.McAodFilter.McVtxFilterTool.DoSignalProcessVtx = True    
    topSequence.McAodFilter.McVtxFilterTool.DecayPatterns += [ " -> 13|-13 " ]
    topSequence.McAodFilter.McVtxFilterTool.DecayPatterns += [ " 13  -> 22 + 13 " ]
    topSequence.McAodFilter.McVtxFilterTool.DecayPatterns += [ " -13 -> 22 + -13 " ]
    #topSequence.McAodFilter.DoTruthParticles = True
    #topSequence.McAodFilter.TruthParticleCnvTool = TruthParticleCnvTool()
    #topSequence.McAodFilter.TruthParticleCnvTool.McEvents = "GEN_EVENT_MUON"
    #topSequence.McAodFilter.TruthParticleCnvTool.TruthParticlesOutput = "SpclMC_MUON"
    #     from McParticleAlgs.JobOptCfg import McAodBuilder
    #     topSequence += McAodBuilder()
    #     if rec.readESD():
    #         topSequence.McAodBuilder.FilterTool.McEvents = "TruthEvent"
    #     if rec.readAOD():
    #         topSequence.McAodBuilder.FilterTool.McEvents = "GEN_AOD"
    #     topSequence.McAodBuilder.CnvTool.TruthParticlesOutput = "SpclMC_muon"


#
# Merge CaloMuonCollection to MuonCollections
# Requires MuonCollectionBuilders-00-00-14 or higher
MergeCaloCollections = False
StacoMergedCollection = "StacoMuonCollection"
MuidMergedCollection  = "MuidMuonCollection"
MuonMergedCollection  = "Muons"    
#
if MergeCaloCollections:
    
    StacoMergedCollection = "CStaco"
    MuidMergedCollection = "CMuid"
    MuonMergedCollection = "CMuons"    

    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr
    ToolSvc += CfgMgr.Rec__MuonPrintingTool( "MuonCombinedPrintingTool" )
    ToolSvc += CfgMgr.Rec__MuonOverlapTool( "MuonCombinedOverlapTool" )
    ToolSvc += CfgMgr.Rec__MuonOverlapResolverTool( "UnifiedChainOverlapResolverTool",
                                                    UpgradeMugirlToCombined = True,
                                                    EnforceRel16MuonContent = False)
    ToolSvc += CfgMgr.Rec__MuonOverlapResolverTool( "MuonCombinedOverlapResolverTool" )
    ToolSvc += CfgMgr.MuonQualityTool( name      = "UnifiedMuonQualityTool", IsUnified = True )
    
    # Merge Muons and CaloMuon
    MergeMuonsAndCaloMuonCollection = CfgMgr.Rec__MuonCollectionMerger(
        name                    = "MergeMuonsAndCaloMuonCollection",
        MuonContainerNames      = ["Muons","CaloMuonCollection"],
        OutputMuonCollection    = "MergedMuons",
        MuonIsolationTool = None,
        MuGirlLowBetaCollection = "",  # keep away from existing collection,
        AddClusterIsolationToAllMuons = False,
        OutputLevel = INFO
        )
    topSequence += MergeMuonsAndCaloMuonCollection
    BuildCMuonCollection = CfgMgr.Rec__MuonIntegratedCollectionBuilder (
        name                   = "BuildCMuonCollection",
        MuonQualityTool        = ToolSvc.UnifiedMuonQualityTool,
        MuonOverlapTool        = ToolSvc.MuonCombinedOverlapTool,
        MuonOverlapResolverTool= ToolSvc.UnifiedChainOverlapResolverTool,
        MuonPrintingTool       = None,
        DebugTextFile          = "",
        InputMuonCollection    = "MergedMuons",
        FinalMuonCollection    = MuonMergedCollection,
        DoLockContainers       = True,
        OutputLevel = INFO
        )
    topSequence += BuildCMuonCollection
    
# >>>>>Backward compatibility for Staco/Miud<<<<< 
    if not MuonD3PDFlags.doNewChainOnly:
        # Merge Muid and CaloMuon
        MergeMuidAndCaloMuonCollection = CfgMgr.Rec__MuonCollectionMerger(
            name                    = "MergeMuidAndCaloMuonCollection",
            MuonContainerNames      = ["MuidMuonCollection","CaloMuonCollection"],
            OutputMuonCollection    = "MergedMuid",
            MuonIsolationTool = None,
            MuGirlLowBetaCollection = "",  # keep away from existing collection,
            AddClusterIsolationToAllMuons = False,
            OutputLevel = INFO
            )
        topSequence += MergeMuidAndCaloMuonCollection
        BuildCMuidCollection = CfgMgr.Rec__MuonIntegratedCollectionBuilder (
            name                   = "BuildCMuidCollection",
            MuonQualityTool        = ToolSvc.UnifiedMuonQualityTool,
            MuonOverlapTool        = ToolSvc.MuonCombinedOverlapTool,
            MuonOverlapResolverTool= ToolSvc.UnifiedChainOverlapResolverTool,
            MuonPrintingTool       = None,
            DebugTextFile          = "",
            InputMuonCollection    = "MergedMuid",
            FinalMuonCollection    = MuidMergedCollection,
            DoLockContainers       = True,
            OutputLevel = INFO
            )
        topSequence += BuildCMuidCollection
        
        # Merge Staco and CaloMuon
        MergeStacoAndCaloMuonCollection = CfgMgr.Rec__MuonCollectionMerger(
            name                    = "MergeStacoAndCaloMuonCollection",
            MuonContainerNames      = ["StacoMuonCollection","CaloMuonCollection"],
            OutputMuonCollection    = "MergedStaco",
            MuonIsolationTool = None,
            MuGirlLowBetaCollection = "",  # keep away from existing collection,
            AddClusterIsolationToAllMuons = False,
            OutputLevel = INFO
            )
        topSequence += MergeStacoAndCaloMuonCollection
        BuildCStacoCollection = CfgMgr.Rec__MuonIntegratedCollectionBuilder (
            name                   = "BuildCStacoCollection",
            MuonQualityTool        = ToolSvc.UnifiedMuonQualityTool,
            MuonOverlapTool        = ToolSvc.MuonCombinedOverlapTool,
            MuonOverlapResolverTool= ToolSvc.UnifiedChainOverlapResolverTool,
            MuonPrintingTool       = None,
            DebugTextFile          = "",
            InputMuonCollection    = "MergedStaco",
            FinalMuonCollection    = StacoMergedCollection,
            DoLockContainers       = True,
            OutputLevel = INFO
            )
        topSequence += BuildCStacoCollection
# >>>>>----------------end------------------<<<<< 


##################################################################################
# Build the Muon D3PD
##################################################################################

# Construct the stream and file names for the MUON D3PD
streamName = prodFlags.WriteMuonD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteMuonD3PD )
MuonD3PDStream_msg.info( "Configuring Muon D3PD with streamName '%s' and fileName '%s'" % \
                         ( streamName, fileName ) )

# Create the D3PD stream itself:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
MuonD3PDStream = MSMgr.NewRootStream( streamName, fileName, "muon" )

# Event info
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
MuonD3PDStream+= EventInfoD3PDObject(10)

# Truth (from filtered truth collection)
if rec.doTruth():
    from TruthD3PDAnalysis.TruthD3PDAnalysisConf import D3PD__GenObjectsFilterTool
    # ... remove empty GenEvents
    LightGenEvtFilterTool = D3PD__GenObjectsFilterTool( "LightGenEvtFilterTool" )
    ToolSvc += LightGenEvtFilterTool
    LightGenEvtFilterTool.RemoveDummyEvents = True
    LightGenEvtFilterTool.RemoveInTimePileUp = True
    LightGenEvtFilterTool.Remove2BCPileUp = True
    LightGenEvtFilterTool.Remove800nsPileUp = True
    LightGenEvtFilterTool.RemoveCavernBkg = True
    LightGenEvtFilterTool.RemoveEmptyEvents = True
    from TruthD3PDMaker.GenEventD3PDObject import GenEventD3PDObject
    MuonD3PDStream += GenEventD3PDObject (level=0, filter=LightGenEvtFilterTool)

    from TruthD3PDMaker.TruthParticleD3PDObject import TruthParticleD3PDObject
    MuonD3PDStream += TruthParticleD3PDObject(1)

# The actual MuonD3PD object
from MuonD3PDMaker.MuonD3PDObject                    import MuonD3PDObject
#from MuonD3PDMaker.TruthMuonD3PDObject               import TruthMuonD3PDObject
from MuonD3PDMaker.MuonTriggerBitsD3PDObject         import MuonTriggerBitsD3PDObject

muonIncludes =     ['Kinematics','Charge','AllAuthor','Authors','TrkMatchQuality','Isolation',
                    'CaloEnergyLoss','AcceptMethods','ParametersAtPV','ComponentTrkParameters','MuonHitSummary',
                    'MuonSpectrometerHitSummary','TrkParameters','TrkCovDiag','TrkCovOffDiag','Quality',                      
                    'EFCBInfoIndex', 'EFMGInfoIndex','EFMEInfoIndex', 'PrimaryTrackDiagonalCovarianceAtPV',
                    'StacoTrackAssocIndex']

if rec.doTruth():
    muonIncludes += ['TruthInfoIndex']

# The 3 variants of muon objects
MuonsD3PDObject = MuonD3PDObject.copy()

# >>>>>Backward compatibility for Staco/Miud<<<<< 
if not MuonD3PDFlags.doNewChainOnly:
    StacoD3PDObject = MuonD3PDObject.copy()
    MuidD3PDObject  = MuonD3PDObject.copy()
# >>>>>----------------end------------------<<<<< 


# Trigger Decision
if D3PDMakerFlags.DoTrigger():
    from TriggerD3PDMaker.TrigDecisionD3PDObject           import TrigDecisionD3PDObject
    from TriggerD3PDMaker.TrigConfMetadata                 import addTrigConfMetadata
    from TriggerD3PDMaker.BGCodeD3PDObject                 import BGCodeD3PDObject
    from TriggerD3PDMaker.BunchStructureMetadata           import addBunchStructureMetadata
    addBunchStructureMetadata(MuonD3PDStream)
    # Trigger Decision + metadata    
    MuonD3PDStream += TrigDecisionD3PDObject(10)
    addTrigConfMetadata(MuonD3PDStream)    
    # Bunch group info
    MuonD3PDStream += BGCodeD3PDObject(2)
    
# Muon Trigger Info
from TrigMuonD3PDMaker.TrigMuonEFInfoD3PDObject        import TrigMuonEFInfoD3PDObject
MuonD3PDStream += TrigMuonEFInfoD3PDObject(0,
                                           include=['Decision'],
                                           exclude=['TrackDetails','SpectrometerTrackBasics','ExtrapolatedTrackBasics'],
                                           allowMissing = True)
MuonD3PDStream += TrigMuonEFInfoD3PDObject(0,
                                           name = "trig_EF_trigmugirl_",
                                           sgkey = "HLT_eMuonEFInfo",
                                           prefix = "trig_EF_trigmugirl_",
                                           exclude=['TrackDetails','SpectrometerTrackBasics','ExtrapolatedTrackBasics'],
                                           include=['Decision'],
                                           allowMissing = True)

# Segments and entry layer
from MuonD3PDMaker.MuonSegmentD3PDObject import MuonSegmentD3PDObject
MuonD3PDStream += MuonSegmentD3PDObject( level = 10, name = 'MuonSeg',
                                         sgkey='MuonSegments',
                                         prefix='muonseg_',
                                         include = ['MuonSegmentT0'],
                                         exclude = ['MuonboySegmentT0'],
                                         allowMissing=True
                                         )

from RecExConfig.RecFlags import rec
if rec.doTruth():
    from MuonD3PDMaker.TrackRecordD3PDObject import TrackRecordD3PDObject
    MuonD3PDStream += TrackRecordD3PDObject(level=0)
    
    
# Tracking Info (to be activated with 
from MuonRecExample.MuonRecFlags import muonRecFlags
TrackLevelOfDetail = 10

from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
TrackD3PDFlags.doTruth = False #rec.doTruth()
TrackD3PDFlags.storeTRTHitsOnTrack = False
TrackD3PDFlags.storePixelHitsOnTrack = False
TrackD3PDFlags.storeSCTHitsOnTrack = False
TrackD3PDFlags.storeBLayerHitsOnTrack = False
TrackD3PDFlags.storeCSCHitsOnTrack = True
TrackD3PDFlags.storeMDTHitsOnTrack = True
TrackD3PDFlags.storeRPCHitsOnTrack = True
TrackD3PDFlags.storeTGCHitsOnTrack = True
TrackD3PDFlags.storePullsAndResiduals = True
## Disable some things that are on by default
TrackD3PDFlags.storeVertexFitQuality = False
TrackD3PDFlags.storeVertexKinematics = False
TrackD3PDFlags.storeVertexPurity = False
TrackD3PDFlags.storeVertexTrackAssociation = False
TrackD3PDFlags.storeVertexTrackIndexAssociation = False
TrackD3PDFlags.storeTrackPredictionAtBLayer = False
TrackD3PDFlags.storeVertexType = False
TrackD3PDFlags.storeDetailedTruth = False #True

# Add TrackParticle D3PD

from TrackD3PDMaker.TrackD3PDObject import TrackD3PDObject
from TrackD3PDMaker.TrackD3PDObject import TrackParticleD3PDObject

MuonTrackPartD3PDObject = TrackD3PDObject(_label='muTP',
                                            _prefix='muTP_',
                                            _sgkey='MuonSpectrometerParticles',
                                            typeName='Rec::TrackParticleContainer',
                                            truthTarget='mc',
                                            truthPrefix='mc_',
                                            detailedTruthPrefix='detailed_mc_',
                                            truthMapKey='MuonSpectrometerParticlesTruth',
                                            detailedTruthMapKey='MuonSpectrometerTracksTruth',
                                            flags=TrackD3PDFlags)
MuonD3PDStream += MuonTrackPartD3PDObject(TrackLevelOfDetail)

# >>>>>Backward compatibility for Staco/Miud<<<<< 
if not MuonD3PDFlags.doNewChainOnly:
    MboyTrackPartD3PDObject = TrackD3PDObject(_label='mboyTP',
                                              _prefix='mboyTP_',
                                              _sgkey='MuonboyTrackParticles',
                                              typeName='Rec::TrackParticleContainer',
                                              truthTarget='mc',
                                              truthPrefix='mc_',
                                              detailedTruthPrefix='detailed_mc_',
                                              truthMapKey='MuonboyTrackTruthCollection',
                                              detailedTruthMapKey='DetailedTrackTruth',
                                              flags=TrackD3PDFlags)
    MuonD3PDStream += MboyTrackPartD3PDObject(TrackLevelOfDetail)
    
    import MuonD3PDMaker
    import D3PDMakerCoreComps
    from D3PDMakerCoreComps.IndexAssociation  import IndexAssociation
    IndexAssociation( StacoD3PDObject,
                      MuonD3PDMaker.MuonTrackParticleAssociationTool,
                      target = "mboyTP_",
                      prefix = "mboyTP_",
                      level = 0,
                      blockname = "StacoTrackAssocIndex",
                      Type = "MuonSpectrometer")
    
    MooreTrackPartD3PDObject = TrackD3PDObject(_label='mooreTP',
                                               _prefix='mooreTP_',
                                               _sgkey='MooreTrackParticles',
                                               typeName='Rec::TrackParticleContainer',
                                               truthTarget='mc',
                                               truthPrefix='mc_',
                                               detailedTruthPrefix='detailed_mc_',
                                               truthMapKey='MooreTrackParticlesTruth',
                                               detailedTruthMapKey='MooreTracksTruth',
                                               flags=TrackD3PDFlags)
    MuonD3PDStream += MooreTrackPartD3PDObject(TrackLevelOfDetail)
# >>>>>----------------end------------------<<<<< 

    
        
# ID Tracks (for TP studies)
from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags as IDTrackFlags
IDTrackFlags.doTruth = rec.doTruth()
IDTrackFlags.storeHitTruthMatching = rec.doTruth()
IDTrackFlags.storeTruthInfo=rec.doTruth()
IDTrackFlags.storeDiagonalCovarianceAsErrors = True
IDTrackFlags.storeBLayerHitsOnTrack = False
IDTrackFlags.storePixelHitsOnTrack = False
IDTrackFlags.storePixelHolesOnTrack = False
IDTrackFlags.storePixelOutliersOnTrack = False
IDTrackFlags.storeSCTHitsOnTrack = False
IDTrackFlags.storeSCTHolesOnTrack = False
IDTrackFlags.storeSCTOutliersOnTrack = False
IDTrackFlags.storeTRTHitsOnTrack = False
IDTrackFlags.storeTRTHolesOnTrack = False
IDTrackFlags.storeTRTOutliersOnTrack = False
IDTrackFlags.storeCSCHitsOnTrack = False
IDTrackFlags.storeMDTHitsOnTrack = False
IDTrackFlags.storeRPCHitsOnTrack = False
IDTrackFlags.storeTGCHitsOnTrack = False
IDTrackFlags.storeTrackFitQuality = True
IDTrackFlags.storeTrackMomentum = True
IDTrackFlags.storeTrackSummary = True
IDTrackFlags.storeTrackSummary.IDOutliers = False
IDTrackFlags.storeTrackSummary.PixelInfoPlus = False
IDTrackFlags.storeTrackSummary.SCTInfoPlus = False
IDTrackFlags.storeTrackSummary.TRTInfoPlus = False
IDTrackFlags.storeTrackSummary.InfoPlus = False
IDTrackFlags.storeTrackPredictionAtBLayer = False
IDTrackFlags.trackParametersAtBeamSpotLevelOfDetails = 0
IDTrackFlags.trackParametersAtGlobalPerigeeLevelOfDetails = 2
IDTrackFlags.trackParametersAtPrimaryVertexLevelOfDetails = 3  

IDTrackParticleD3PDObject = TrackD3PDObject(_label='trkpt4',
                                            _prefix='trkpt4_',
                                            _sgkey='HighPtTracks',
                                            typeName='Rec::TrackParticleContainer',
                                            truthMapKey='TrackParticleTruthCollection',
                                            SGKeyForTruth=D3PDMakerFlags.TrackSGKey(),
                                            flags=IDTrackFlags)

MuonD3PDStream += IDTrackParticleD3PDObject( 3, 'Tracks2',
                                             sgkey  = 'HighPtTracks',
                                             label  = 'trkpt4',
                                             prefix = 'trkpt4_')


# Finally add the 3 muon objects to the stream
MuonD3PDStream += MuonsD3PDObject(level=0, name='Muon', sgkey=MuonMergedCollection, prefix="mu_", include = muonIncludes)

# >>>>>Backward compatibility for Staco/Miud<<<<< 
if not MuonD3PDFlags.doNewChainOnly:
    MuonD3PDStream += StacoD3PDObject(level=0, name='Muon_staco', sgkey=StacoMergedCollection, prefix="mu_staco_", include = muonIncludes)
    MuonD3PDStream += MuidD3PDObject(level=0, name='Muon_muid',  sgkey=MuidMergedCollection,  prefix="mu_muid_", include = muonIncludes)
# >>>>>----------------end------------------<<<<< 









##### old stuff, could still be useful

#     # Truth   
#     if rec.doTruth():
#         #from TrackD3PDMaker.TruthTrackD3PDObject import TruthTrackD3PDObject          
#         #     MuonD3PDStream+= TruthTrackD3PDObject(levelOfDetail)
#         from MuonD3PDMaker.TrackRecordD3PDObject import TrackRecordD3PDObject
#         MuonD3PDStream += TrackRecordD3PDObject ("TrackRecord")




###### Trk::Track D3PD
#     MooreTrackD3PDObject =   TrackD3PDObject(_label='moore',
#                                              _prefix='mutrk_moore_',
#                                              _sgkey='MooreTracks',
#                                              typeName='TrackCollection',
#                                              truthTarget='mc',
#                                              truthMapKey='',
#                                              detailedTruthPrefix='detailed_mc_',
#                                              detailedTruthMapKey='MooreTracksTruth',
#                                              SGKeyForDetailedTruth='MooreTracks',
#                                              flags=TrackD3PDFlags)

#     MboyTrackD3PDObject =   TrackD3PDObject(_label='mboy',
#                                             _prefix='mutrk_mboy_',
#                                             _sgkey='ConvertedMBoyTracks',
#                                             typeName='TrackCollection', 
#                                             truthTarget='mc',
#                                             truthPrefix='mc_',
#                                             detailedTruthPrefix='detailed_mc_',
#                                             truthMapKey='ConvertedMBoyTracksTruth',
#                                             detailedTruthMapKey='ConvertedMBoyTracksTruth',
#                                             SGKeyForDetailedTruth='ConvertedMBoyTracksTruth',
#                                             flags=TrackD3PDFlags)
    
#     MuonTrackD3PDObject =   TrackD3PDObject(_label='muon',
#                                             _prefix='mutrk_',
#                                             _sgkey='MuonSpectrometerTracks',
#                                             typeName='TrackCollection', 
#                                             truthTarget='mc',
#                                             truthPrefix='mc_',
#                                             detailedTruthPrefix='detailed_mc_',
#                                             truthMapKey='MuonSpectrometerTrackTruthCollection',
#                                             detailedTruthMapKey='DetailedTrackTruth',
#                                             SGKeyForDetailedTruth='MuonSpectrometerTracks',
#                                             flags=TrackD3PDFlags)

#     MuonD3PDStream += MooreTrackD3PDObject(levelOfDetail)
#     MuonD3PDStream += MboyTrackD3PDObject(levelOfDetail)
#     MuonD3PDStream += MuonTrackD3PDObject(levelOfDetail)
