################################
# setup of alignment monitoring

from AthenaMonitoring.DQMonFlags import DQMonFlags
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
InDetAlignMonManager = AthenaMonManager( name                = "InDetAlignMonManager",
                                         FileKey             = DQMonFlags.monManFileKey(),
                                         ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                                         DataType            = DQMonFlags.monManDataType(),
					 Environment         = "user",
                                         ManualRunLBSetup    = True,
                                         Run                 = 1,
                                         LumiBlock           = 1)


from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import InDetAlignMon__TrackSelectionTool

# All tracks selection
allSelection = InDetAlignMon__TrackSelectionTool(
        name = "InDetAlignMonTrackSelectionTool",
        PassAllTracks = True, ## Uncomment this line to bypass track slection
)
ToolSvc += allSelection

# Align tracks selection

from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetTrtDriftCircleCutTool
trtDCtool = InDet__InDetTrtDriftCircleCutTool(
    name = "TrtHitsEtaCutTool",
    #OutputLevel = newInDetAlignAlg_Options["outputLevel"],
    UseNewParameterization = True,
    UseActiveFractionSvc = False
)
ToolSvc += trtDCtool
print trtDCtool

#from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
InDetTrackSummaryTool = ToolSvc.InDetTrackSummaryTool
#import egammaRec.EMCommonTrackSummary
#InDetTrackSummaryTool = ToolSvc.InDetTrackSummaryTool #egammaRec.EMCommonTrackSummary.egammaInDetTrackSummaryTool

from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
trackSelector = InDet__InDetDetailedTrackSelectorTool(
    name = "TrackSelector",
    OutputLevel = 7,
    #OutputLevel = DEBUG,
    TrackSummaryTool = InDetTrackSummaryTool,
    pTMin = 10000.,
    IPd0Max = 500.,
    IPz0Max = 500.,
    #nHitSct = 4,
    nHitPix = 1,
    nHitPixPhysical = 1,
    nHitBLayerPlusPix = 0,
    nHitBLayer = 0,
    nHitSi = 9,
    nHitSiPhysical = 7,
    nHitTrt = 0,
    #useEtaDepententMinHitTrt = True,
    TrtDCCutTool = trtDCtool,
#    addToMinHitTrt = -3
)
trackSelector.useEtaDepententMinHitTrt = True
ToolSvc += trackSelector


alignTrackSelection = InDetAlignMon__TrackSelectionTool(
                                                        name = "InDetAlignMonAlignTrackSelectionTool",
                                                        #PassAllTracks = True, ## Uncomment this line to bypass track slection
                                                        TrackSelectorTool = trackSelector
                                                        )
ToolSvc += alignTrackSelection


from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonResiduals
from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonGenericTracks
from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonEfficiencies
tracks = trackCollections[0]

for trackCollection in trackCollections:
        # Selected tracks
        print "Loading monitoring"
        InDetAlignMonResiduals = IDAlignMonResiduals(
                name = "InDetAlignMonResiduals_"+trackCollection,
                trackSelection = alignTrackSelection,#allSelection,
                tracksName = trackCollection,
                useExtendedPlots = True,
                triggerChainName = "all",
                Pixel_Manager = InDetKeys.PixelManager(),
                SCT_Manager = InDetKeys.SCT_Manager(),
                TRT_Manager = InDetKeys.TRT_Manager(),
                minPIXResXFillRange = -0.1,
                maxPIXResXFillRange =  0.1,
                #minPIXResXFillRange = -0.5,
                #maxPIXResXFillRange =  0.5,
                minPIXResYFillRange = -0.5,
                maxPIXResYFillRange =  0.5,
                minSCTResFillRange  = -0.2,
                maxSCTResFillRange =   0.2,
                #minSCTResFillRange  = -0.5,
                #maxSCTResFillRange =   0.5,
                NSplitMap = 4,
                RangeOfPullHistos  =   5

        )
        print InDetAlignMonResiduals

        #ToolSvc += InDetAlignMonResiduals
        InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonResiduals ]

        InDetAlignMonGenericTracks = IDAlignMonGenericTracks(
                name = "InDetAlignMonGenericTracks_"+trackCollection,
                trackSelection = alignTrackSelection,#allSelection,
                tracksName = trackCollection,
                useExtendedPlots = True,
                triggerChainName = "all",
                VxPrimContainerName = InDetKeys.xAODVertexContainer()
        )
        #ToolSvc += InDetAlignMonGenericTracks
        InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonGenericTracks ]

        InDetAlignMonSelectedTracksEfficiencies = IDAlignMonEfficiencies (
                name = "InDetAlignMonEfficiencies_"+trackCollection,
                trackSelection = alignTrackSelection,#allSelection,
                tracksName = trackCollection,
                triggerChainName = "all",
                #HoleSearch = InDetExtendedHoleSearchTool,
                Pixel_Manager = InDetKeys.PixelManager(),
                SCT_Manager = InDetKeys.SCT_Manager(),
                TRT_Manager = InDetKeys.TRT_Manager()
        )
        #ToolSvc += InDetAlignMonSelectedTracksEfficiencies
        InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonSelectedTracksEfficiencies ]


from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import InDetAlignMonBeamSpot
InDetAlignMonBeamSpot_noTrig = InDetAlignMonBeamSpot (name  = "InDetAlignMonBeamSpot_noTrig",
                                                                                                          extrapolator                   = ElectronTrkExtrapolator,
#                                                                                                          vxContainerName                = InDetKeys.PrimaryVertices(),
                                                                                                          vxContainerName                = "PrimaryVertices",
						                                                          trackContainerName  = "InDetTrackParticles",
						                                                          useBeamspot  = True,
                                                                                                         vxContainerWithBeamConstraint  = InDetFlags.useBeamConstraint(),
						                                    #                     vxContainerWithBeamConstraint  = False,
						                                                          OutputLevel= 1)
#ToolSvc += InDetAlignMonBeamSpot_noTrig
InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonBeamSpot_noTrig ]

# write out BS info
#from AthenaCommon.AppMgr    import ServiceMgr as svcMgr
#from IOVDbSvc.IOVDbSvcConf  import IOVDbSvc
#svcMgr += IOVDbSvc()
#IOVDbSvc.dbConnection  = "impl=cool;techno=sqlite;schema=beamspot.db;X:TESTCOOL"



if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()

THistSvc         = Service( "THistSvc" )

histOutput       = DQMonFlags.monManFileKey()+" DATAFILE='monitoring.root' OPT='RECREATE'"
THistSvc.Output += [histOutput]
#InDetAlignMonManager.FileKey = "IDAlignMon"
topSequence += InDetAlignMonManager
print InDetAlignMonManager
