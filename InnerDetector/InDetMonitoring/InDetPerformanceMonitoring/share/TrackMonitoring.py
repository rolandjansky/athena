################################
import os
###############################
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



from InDetTrackSelectionTool.InDetTrackSelectionToolConf   import InDet__InDetTrackSelectionTool           #loading the InDetTrackSelectionTool

m_TrackSelectorTool_NoCut = InDet__InDetTrackSelectionTool(name         = "InDetTrackSelectorNoCut",
							       UseTrkTrackTools = True,
							       CutLevel = "NoCut",
							       TrackSummaryTool    = InDetTrackSummaryTool,
							       Extrapolator        = InDetExtrapolator)



ToolSvc +=  m_TrackSelectorTool_NoCut

LPSelection = InDetAlignMon__TrackSelectionTool(
	name = "InDetAlignMonTracksSelectionTool_LP",
	PrimVtxContainerName = InDetKeys.xAODVertexContainer(),
	UseIDTrackSelectionTool = True,
	IDTrackSelectionTool = m_TrackSelectorTool_NoCut,
	#TrackSelectorTool    = InDetDetailedTrackSelector_Default
	)

ToolSvc += LPSelection


allSelection = InDetAlignMon__TrackSelectionTool(
                                                        name = "InDetAlignMonAlignTrackSelectionTool",
                                                        PassAllTracks = True, ## Uncomment this line to bypass track slection
							PrimVtxContainerName = InDetKeys.xAODVertexContainer(),
							UseIDTrackSelectionTool = True,
							IDTrackSelectionTool = m_TrackSelectorTool_NoCut,
							#TrackSelectorTool    = InDetDetailedTrackSelector_Default
							)
ToolSvc += allSelection


from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonResiduals
from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonGenericTracks
from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonEfficiencies


for trackCollection in trackCollections:
        # Selected tracks
        print "Loading monitoring"
        InDetAlignMonResiduals = IDAlignMonResiduals(
                name = "InDetAlignMonResiduals_"+trackCollection,
                trackSelection = LPSelection,
                tracksName = trackCollection,
                useExtendedPlots = True,
                triggerChainName = "Tracks",
                Pixel_Manager = InDetKeys.PixelManager(),
                SCT_Manager = InDetKeys.SCT_Manager(),
                TRT_Manager = InDetKeys.TRT_Manager(),
                minPIXResXFillRange = -0.065,
                maxPIXResXFillRange =  0.065,
                minPIXResYFillRange = -0.5,
                maxPIXResYFillRange =  0.5,
                minSCTResFillRange  = -0.110,
                maxSCTResFillRange =   0.110,
                minTRTResidualWindow = -0.5,
                maxTRTResidualWindow =  0.5,
                NSplitMap = 4,
                RangeOfPullHistos  =   5,
                applyHistWeight = True,
                hWeightInFileName = os.getenv("TestArea")+"/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/share/hWeight.root"
        )
        print InDetAlignMonResiduals

        ToolSvc += InDetAlignMonResiduals
        InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonResiduals ]

        InDetAlignMonGenericTracks = IDAlignMonGenericTracks(
                name = "InDetAlignMonGenericTracks_"+trackCollection,
                trackSelection = LPSelection,
                tracksName = trackCollection,
                useExtendedPlots = True,
                triggerChainName = "Tracks",
                VxPrimContainerName = InDetKeys.PrimaryVertices(),
                d0BsRange = 0.1,
                z0Range = 150.,
                applyHistWeight = True,
                hWeightInFileName = os.getenv("TestArea")+"/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/share/hWeight.root"

        )
        print InDetAlignMonGenericTracks
        
        ToolSvc += InDetAlignMonGenericTracks
        InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonGenericTracks ]

        InDetAlignMonSelectedTracksEfficiencies = IDAlignMonEfficiencies (
                name = "InDetAlignMonEfficiencies_"+trackCollection,
                trackSelection = LPSelection,
                tracksName = trackCollection,
                triggerChainName = "Tracks",
                #HoleSearch = InDetExtendedHoleSearchTool,
                Pixel_Manager = InDetKeys.PixelManager(),
                SCT_Manager = InDetKeys.SCT_Manager(),
                TRT_Manager = InDetKeys.TRT_Manager()
        )
        ToolSvc += InDetAlignMonSelectedTracksEfficiencies
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
#InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonBeamSpot_noTrig ]

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
