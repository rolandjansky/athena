################################
import os
try:
    useWeightInMonitoring
except:
    useWeightInMonitoring = False

try:
    fileWithWeights
except:
    fileWithWeights = "usr/WorkDir/21.0.68/InstallArea/x86_64-slc6-gcc62-opt/src/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/share/hWeight.root"
###############################
# setup of alignment monitoring
from AthenaMonitoring.DQMonFlags import DQMonFlags
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
InDetAlignMonManager = AthenaMonManager( name                = "IDAlignmentMonManager",
                                         FileKey             = DQMonFlags.monManFileKey(),
                                         ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                                         DataType            = DQMonFlags.monManDataType(),
					 Environment         = "user",
                                         ManualRunLBSetup    = True,
                                         Run                 = 1,
                                         LumiBlock           = 1,
                                         OutputLevel         = INFO)


from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import InDetAlignMon__TrackSelectionTool

# All tracks selection
#allSelection = InDetAlignMon__TrackSelectionTool( name = "InDetAlignMonTrackSelectionTool",
#                                                  PassAllTracks = True, ## Uncomment this line to bypass track slection
#)
#ToolSvc += allSelection
#print (' == TrackMonitoring == ')
#print (allSelection)

# Align tracks selection

InDetTrackSummaryTool = ToolSvc.InDetTrackSummaryTool

m_TrackSelectorTool_NoCut = InDet__InDetTrackSelectionTool(name             = "InDetTrackSelectionToolNoCut",
                                                           UseTrkTrackTools = True,
                                                           CutLevel         = "NoCut",
                                                           TrackSummaryTool = InDetTrackSummaryTool,
                                                           Extrapolator     = InDetExtrapolator)
ToolSvc +=  m_TrackSelectorTool_NoCut
print (' == TrackMonitoring == TrackSelectorTool_NoCut ')
print (m_TrackSelectorTool_NoCut)

m_TrackSelectorTool_TP = InDet__InDetTrackSelectionTool(name         = "InDetTrackSelectionToolTightPrimary",
                                                        UseTrkTrackTools = True,
                                                        CutLevel = "TightPrimary",
                                                        minPt = 0,
                                                        minNPixelHitsPhysical = 2,
                                                        minNSctHitsPhysical = 2,
                                                        TrackSummaryTool    = InDetTrackSummaryTool,
                                                        Extrapolator        = InDetExtrapolator)
ToolSvc +=  m_TrackSelectorTool_TP
print (' == TrackMonitoring == TrackSelectorTool_TP')
print (m_TrackSelectorTool_TP)



# define the loose primary and tight primary selections to be used in the monitoring
LPSelection = InDetAlignMon__TrackSelectionTool( name = "InDetAlignMonTracksSelectionTool_LP",
                                                 PrimVtxContainerName = InDetKeys.xAODVertexContainer(),
                                                 UseIDTrackSelectionTool = True,
                                                 IDTrackSelectionTool = m_TrackSelectorTool_NoCut,
                                             )
ToolSvc += LPSelection
print (' == TrackMonitoring == LPSelection')
print (LPSelection)

TPSelection = InDetAlignMon__TrackSelectionTool(
	name = "InDetAlignMonTracksSelectionTool_TP",
	PrimVtxContainerName = InDetKeys.xAODVertexContainer(),
	UseIDTrackSelectionTool = True,
	IDTrackSelectionTool = m_TrackSelectorTool_TP,
	)
ToolSvc += TPSelection
print (' == TrackMonitoring == TPSelection ')
print (TPSelection)

allSelection = InDetAlignMon__TrackSelectionTool(name = "InDetAlignMonAlignTrackSelectionTool",
                                                 PassAllTracks = True, ## Uncomment this line to bypass track slection
                                                 PrimVtxContainerName = InDetKeys.xAODVertexContainer(),
                                                 UseIDTrackSelectionTool = True,
                                                 IDTrackSelectionTool = m_TrackSelectorTool_TP
                                                 )
ToolSvc += allSelection

# hole search tool
from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
InDetHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "InDetHoleSearchTool")
ToolSvc += InDetHoleSearchTool

# InDetAlignment monitoring
from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonResiduals
from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonGenericTracks
from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonEfficiencies


for trackCollection in trackCollections:
        #theTrackSelection = LPSelection
        #theTrackSelection = TPSelection
        theTrackSelection = allSelection
        if ("CombinedInDetTracks" in trackCollection):
                theTrackSelection = TPSelection
        # Selected tracks
        InDetAlignMonResiduals = IDAlignMonResiduals(name = "InDetAlignMonResiduals_"+trackCollection,
                                                     trackSelection = theTrackSelection,
                                                     tracksName = trackCollection,
                                                     useExtendedPlots = True,
                                                     triggerChainName = "Tracks",
                                                     Pixel_Manager = InDetKeys.PixelManager(),
                                                     SCT_Manager = InDetKeys.SCT_Manager(),
                                                     TRT_Manager = InDetKeys.TRT_Manager(),
                                                     minPIXResXFillRange = -0.050,
                                                     maxPIXResXFillRange =  0.050,
                                                     minPIXResYFillRange = -0.360,
                                                     maxPIXResYFillRange =  0.360,
                                                     minSCTResFillRange  = -0.090,
                                                     maxSCTResFillRange =   0.090,
                                                     minTRTResidualWindow = -0.550,
                                                     maxTRTResidualWindow =  0.550,
                                                     NSplitMap = 4,
                                                     RangeOfPullHistos  =   5,
                                                     PtRange = 50., # GeV
                                                     NBinsPtRange = 25,
                                                     xHitErrorRange = 0.06,
                                                     maxPtforECHistos = 100., # to be set to 10 or 5 GeV
                                                     applyHistWeight = useWeightInMonitoring,
                                                     hWeightHistName = "h_eventMuVsTrkEta",
                                                     hWeightInFileName = fileWithWeights,
                                                     OutputLevel = INFO
                                                 )
        print (InDetAlignMonResiduals)

        InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonResiduals ]

        InDetAlignMonGenericTracks = IDAlignMonGenericTracks(name = "InDetAlignMonGenericTracks_"+trackCollection,
                                                             trackSelection = theTrackSelection,
                                                             tracksName = trackCollection,
                                                             useExtendedPlots = True,
                                                             triggerChainName = "Tracks",
                                                             #VxPrimContainerName = InDetKeys.PrimaryVertices(),
                                                             VxPrimContainerName = InDetKeys.xAODVertexContainer(), # Salva 24/OCT/2020 test for R22
                                                             d0BsRange = 0.090,
                                                             z0Range = 160.,
                                                             applyHistWeight = useWeightInMonitoring,
                                                             hWeightHistName = "h_eventMuVsTrkEta",
                                                             hWeightInFileName = fileWithWeights,
                                                             OutputLevel = INFO
                                                         )
        print (InDetAlignMonGenericTracks)
        
        InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonGenericTracks ]

        InDetAlignMonSelectedTracksEfficiencies = IDAlignMonEfficiencies (name = "InDetAlignMonEfficiencies_"+trackCollection,
                                                                          trackSelection = theTrackSelection,
                                                                          tracksName = trackCollection,
                                                                          triggerChainName = "Tracks",
                                                                          HoleSearch = InDetHoleSearchTool,
                                                                          Pixel_Manager = InDetKeys.PixelManager(),
                                                                          SCT_Manager = InDetKeys.SCT_Manager(),
                                                                          TRT_Manager = InDetKeys.TRT_Manager(),
                                                                          OutputLevel = INFO
                                                                      )

        #InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonSelectedTracksEfficiencies ]


from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import InDetAlignMonBeamSpot
InDetAlignMonBeamSpot_noTrig = InDetAlignMonBeamSpot (name  = "InDetAlignMonBeamSpot_noTrig",
                                                      extrapolator                   = ElectronTrkExtrapolator,
                                                      # vxContainerName                = InDetKeys.PrimaryVertices(),
                                                      vxContainerName                = "PrimaryVertices",
                                                      trackContainerName  = "InDetTrackParticles",
                                                      useBeamspot  = True,
                                                      vxContainerWithBeamConstraint  = InDetFlags.useBeamConstraint(),
                                                      OutputLevel= 1)


if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()

THistSvc         = Service( "THistSvc" )

histOutput       = DQMonFlags.monManFileKey()+" DATAFILE='monitoring.root' OPT='RECREATE'"
THistSvc.Output += [histOutput]
#InDetAlignMonManager.FileKey = "IDAlignMon"
topSequence += InDetAlignMonManager
print (InDetAlignMonManager)
