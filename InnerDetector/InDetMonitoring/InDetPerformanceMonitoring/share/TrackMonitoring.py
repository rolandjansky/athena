################################
import os
#from runzmumu_run2paper import useWeightInMonitoring
try:
    useWeightInMonitoring
except:
    useWeightInMonitoring = False
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
print " == runzmumu_script == "
print allSelection

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


# salva 13/April/2017 #from InDetTrackSelectionTool.InDetTrackSelectionToolConf   import InDet__InDetTrackSelectionTool           #loading the InDetTrackSelectionTool

m_TrackSelectorTool_NoCut = InDet__InDetTrackSelectionTool(name         = "InDetTrackSelectionToolNoCut",
							       UseTrkTrackTools = True,
							       CutLevel = "NoCut",
							       TrackSummaryTool    = InDetTrackSummaryTool,
							       Extrapolator        = InDetExtrapolator)
ToolSvc +=  m_TrackSelectorTool_NoCut
print " == runzmumu_script == TrackSelectorTool_NoCut "
print m_TrackSelectorTool_NoCut

m_TrackSelectorTool_TP = InDet__InDetTrackSelectionTool(name         = "InDetTrackSelectionToolTightPrimary",
                                                        UseTrkTrackTools = True,
                                                        CutLevel = "TightPrimary",
                                                        minPt = 4000,
                                                        minNPixelHitsPhysical = 2,
                                                        minNSctHitsPhysical = 2,
                                                        TrackSummaryTool    = InDetTrackSummaryTool,
                                                        Extrapolator        = InDetExtrapolator)
ToolSvc +=  m_TrackSelectorTool_TP
print " == runzmumu_script == TrackSelectorTool_TP "
print m_TrackSelectorTool_TP



# define the loose primary and tight primary selections to be used in the monitoring
LPSelection = InDetAlignMon__TrackSelectionTool(
	name = "InDetAlignMonTracksSelectionTool_LP",
	PrimVtxContainerName = InDetKeys.xAODVertexContainer(),
	UseIDTrackSelectionTool = True,
	IDTrackSelectionTool = m_TrackSelectorTool_NoCut,
	#TrackSelectorTool    = InDetDetailedTrackSelector_Default
	)
ToolSvc += LPSelection
print " == runzmumu_script == LPSelection "
print LPSelection

TPSelection = InDetAlignMon__TrackSelectionTool(
	name = "InDetAlignMonTracksSelectionTool_TP",
	PrimVtxContainerName = InDetKeys.xAODVertexContainer(),
	UseIDTrackSelectionTool = True,
	IDTrackSelectionTool = m_TrackSelectorTool_TP,
	)
ToolSvc += TPSelection
print " == runzmumu_script == TPSelection "
print TPSelection

allSelection = InDetAlignMon__TrackSelectionTool(name = "InDetAlignMonAlignTrackSelectionTool",
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
        theTrackSelection = LPSelection
        if ("CombinedInDetTracks" in trackCollection):
                theTrackSelection = TPSelection
        print " == runzmumu_script == monitoring for ",trackCollection," --> ",theTrackSelection
        # Selected tracks
        print "== runzmumu_script == Loading IDAlignMonResiduals for trackCollection:",trackCollection
        InDetAlignMonResiduals = IDAlignMonResiduals(
                OutputLevel = DEBUG,
                name = "InDetAlignMonResiduals_"+trackCollection,
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
                applyHistWeight = useWeightInMonitoring,
                hWeightHistName = "h_eventMuVsTrkEta",
                hWeightInFileName = "usr/WorkDir/21.0.68/InstallArea/x86_64-slc6-gcc62-opt/src/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/share/hWeight.root"
              #ROOT FILE SHOULD BE TAKEN FROM CALIB AREA USING PATH RESOLVER!!! NEEDS TO CHANGE!!!
        )
        print InDetAlignMonResiduals

        ToolSvc += InDetAlignMonResiduals
        InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonResiduals ]

        InDetAlignMonGenericTracks = IDAlignMonGenericTracks(
                name = "InDetAlignMonGenericTracks_"+trackCollection,
                trackSelection = theTrackSelection,
                tracksName = trackCollection,
                useExtendedPlots = True,
                triggerChainName = "Tracks",
                VxPrimContainerName = InDetKeys.PrimaryVertices(),
                d0BsRange = 0.090,
                z0Range = 160.,
                applyHistWeight = useWeightInMonitoring,
                hWeightHistName = "h_eventMuVsTrkEta",
                hWeightInFileName = "usr/WorkDir/21.0.23/InstallArea/x86_64-slc6-gcc62-opt/src/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/share/hWeight.root"
                 #ROOT FILE SHOULD BE TAKEN FROM CALIB AREA USING PATH RESOLVER!!! NEEDS TO CHANGE!!!
        )
        print InDetAlignMonGenericTracks
        
        ToolSvc += InDetAlignMonGenericTracks
        InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonGenericTracks ]

        InDetAlignMonSelectedTracksEfficiencies = IDAlignMonEfficiencies (
                name = "InDetAlignMonEfficiencies_"+trackCollection,
                trackSelection = theTrackSelection,
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


if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()

THistSvc         = Service( "THistSvc" )

histOutput       = DQMonFlags.monManFileKey()+" DATAFILE='monitoring.root' OPT='RECREATE'"
THistSvc.Output += [histOutput]
#InDetAlignMonManager.FileKey = "IDAlignMon"
topSequence += InDetAlignMonManager
print InDetAlignMonManager
