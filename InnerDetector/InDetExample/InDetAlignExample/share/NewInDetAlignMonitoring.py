################################
# setup of alignment monitoring



from AthenaMonitoring.DQMonFlags import DQMonFlags
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
#InDetAlignMonManager = AthenaMonManager( name                = "InDetAlignMonManager",
#FileKey             = DQMonFlags.monManFileKey(),
#                                         ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
#                                         DataType            = DQMonFlags.monManDataType(),
#                                         Environment         = "user",
#                                         ManualRunLBSetup    = True,
#                                         Run                 = 1,
#                                         LumiBlock           = 1)
#

InDetAlignMonManager = AthenaMonManager( name                = "InDetAlignMonManager",
                                         FileKey             = DQMonFlags.monManFileKey(),
                                         ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                                         DataType            = DQMonFlags.monManDataType(),
                                         Environment         = DQMonFlags.monManEnvironment(),
                                         ManualRunLBSetup    = DQMonFlags.monManManualRunLBSetup(),
                                         Run                 = DQMonFlags.monManRun(),
                                         LumiBlock           = DQMonFlags.monManLumiBlock())
# track collection to monitor

tracks = "ExtendedTracks"
    
if Cosmics:
    tracks = "CombinedInDetTracks"
    trackCollections = [tracks]

# if we're running Si only (TRT is off) we need to use different track collection
if not newInDetAlignAlg_Options["useTRT"]:
    tracks = "ResolvedTracks"

if newInDetAlignAlg_Options["doBSConstraint"] == False and newInDetAlignAlg_Options["doPVConstraint"] == False:
    #trackCollections = [tracks,"AlignTracks","AlignTracksNormalRefitted"]
    trackCollections  = [tracks,"AlignTracks"]
elif newInDetAlignAlg_Options["doBSConstraint"] == True and newInDetAlignAlg_Options["doPVConstraint"] == False:
    trackCollections = [tracks,"AlignTracks","AlignTracksNormalRefitted","AlignTracksBeamspotConstrained"]
elif newInDetAlignAlg_Options["doBSConstraint"] == False and newInDetAlignAlg_Options["doPVConstraint"] == True:
    trackCollections = [tracks,"AlignTracks","AlignTracksNormalRefitted","AlignTracksVertexConstrained"]
else:
    trackCollections = [tracks,"AlignTracks","AlignTracksNormalRefitted","AlignTracksBeamspotConstrained","AlignTracksVertexConstrained"]

trackCollections.append("ForwardTracks")

from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import InDetAlignMon__TrackSelectionTool
from InDetTrackSelectionTool.InDetTrackSelectionToolConf   import InDet__InDetTrackSelectionTool           #loading the InDetTrackSelectionTool
from InDetTrackSelectorTool.InDetTrackSelectorToolConf     import InDet__InDetDetailedTrackSelectorTool


#trackCollections.append("ForwardTracks")


# Default Detailed TrackSelectorTool. To avoid the use of an unconfigured TrackSelectionTool

InDetDetailedTrackSelector_Default = InDet__InDetDetailedTrackSelectorTool(name             = "DefaultDetailedSelector",
                                                                           TrackSummaryTool = InDetTrackSummaryTool,
                                                                           Extrapolator     = InDetExtrapolator)

ToolSvc += InDetDetailedTrackSelector_Default


# All tracks selection
allSelection = InDetAlignMon__TrackSelectionTool(
    name = "InDetAlignMonTrackSelectionTool",
    PrimVtxContainerName = InDetKeys.xAODVertexContainer(),
    PassAllTracks = True, ## Uncomment this line to bypass track slection
    TrackSelectorTool    = InDetDetailedTrackSelector_Default
)


ToolSvc+= allSelection

# Tracks For LoosePrimary Selection


m_TrackSelectorTool_LoosePrimary = InDet__InDetTrackSelectionTool(name         = "InDetTrackSelectorLoosePrimary",
                                                                  UseTrkTrackTools = True,
                                                                  minPt = 5000,
                                                                  maxD0 = 100000,
                                                                  maxZ0SinTheta = 150,
                                                                  minNTrtHits = 0,
                                                                  CutLevel = "LoosePrimary",
                                                                  TrackSummaryTool    = InDetTrackSummaryTool,
                                                                  Extrapolator        = InDetExtrapolator)




ToolSvc +=  m_TrackSelectorTool_LoosePrimary


LPSelection = InDetAlignMon__TrackSelectionTool(
    name = "InDetAlignMonTracksSelectionTool_LP",
    PrimVtxContainerName = InDetKeys.xAODVertexContainer(),
    UseIDTrackSelectionTool = True,
    IDTrackSelectionTool = m_TrackSelectorTool_LoosePrimary,
    #TrackSelectorTool    = InDetDetailedTrackSelector_Default
    )

ToolSvc += LPSelection

#Track selector: Defined in NewInDetAlignAlgSetup
#trackSelector = The cuts are:
#               pTMin = newInDetAlignAlg_Options["PtCut"],
#               IPd0Max = 500.,
#               IPz0Max = 500.,
#               #nHitSct = 4,
#               nHitPix = 1,
#               nHitPixPhysical = 1,
#               nHitBLayerPlusPix = 0,
#               nHitBLayer = 0,
#               nHitSi = 9,
#               nHitSiPhysical = 7,
#               nHitTrt = 0,
#               TrtDCCutTool = trtDCtool,
#               addToMinHitTrt = -3



# Align tracks selection
if newInDetAlignAlg_Options["useTrackSelector"]:
    alignTrackSelection = InDetAlignMon__TrackSelectionTool(
                            name = "InDetAlignMonAlignTrackSelectionTool",
                            PrimVtxContainerName = InDetKeys.xAODVertexContainer(),
                            #PassAllTracks = True, ## Uncomment this line to bypass track slection
                            IDTrackSelectionTool = trackSelectorNew,
                            UseIDTrackSelectionTool = True
                            )
    ToolSvc += alignTrackSelection


from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonResiduals
from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonGenericTracks
from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonEfficiencies

printfunc (" <NewInDetAlignMonitoring> setting up " )
for trackCollection in trackCollections:
    # Selected tracks
    printfunc (" <NewInDetAlignMonitoring> (type 1) Loading monitor for track Collection", trackCollection, " selection", allSelection.name)
    InDetAlignMonResiduals = IDAlignMonResiduals(
        name = "InDetAlignMonResiduals_"+trackCollection,
        trackSelection = allSelection,
        tracksName = trackCollection,
        useExtendedPlots = True,
        doIBLLBPlots = True,
        doClusterSizeHistos = True,
        triggerChainName = "all",
        Pixel_Manager = InDetKeys.PixelManager(),
        SCT_Manager = InDetKeys.SCT_Manager(),
        TRT_Manager = InDetKeys.TRT_Manager(),
        minPIXResXFillRange = -0.08,
        maxPIXResXFillRange =  0.08,
        minPIXResYFillRange = -0.25,
        maxPIXResYFillRange =  0.25,
        minSCTResFillRange  = -0.15,
        maxSCTResFillRange =   0.15,
        minTRTResidualWindow =-2.5,
        maxTRTResidualWindow = 2.5,
        NSplitMap  = 1,
        RangeOfPullHistos  =   5.0
    )
    printfunc (InDetAlignMonResiduals)
    
    #ToolSvc += InDetAlignMonResiduals
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonResiduals ]

    InDetAlignMonGenericTracks = IDAlignMonGenericTracks(
        name = "InDetAlignMonGenericTracks_"+trackCollection,
        trackSelection = allSelection,
        tracksName = trackCollection,
        useExtendedPlots = True,
        doIP = False,
        #doIPmaps = True,
        triggerChainName = "all",
        d0Range = 5,
        z0Range = 200,
        #NTracksRange = 10,
        TrackToVertexIPEstimator = TrackingCommon.getTrackToVertexIPEstimator(),
        VxPrimContainerName = InDetKeys.PrimaryVertices()
    )
    if Cosmics:
        InDetAlignMonGenericTracks.d0Range = 500
        InDetAlignMonGenericTracks.z0Range = 1000
        
    #ToolSvc += InDetAlignMonGenericTracks
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonGenericTracks ]

    InDetAlignMonSelectedTracksEfficiencies = IDAlignMonEfficiencies (
        name = "InDetAlignMonEfficiencies_"+trackCollection,
        #doHoleSearch = False,
        trackSelection = allSelection,
        tracksName = trackCollection,
        triggerChainName = "all",
        #HoleSearch = InDetExtendedHoleSearchTool,
        Pixel_Manager = InDetKeys.PixelManager(),
        SCT_Manager = InDetKeys.SCT_Manager(),
        TRT_Manager = InDetKeys.TRT_Manager()
    )

    #ToolSvc += InDetAlignMonSelectedTracksEfficiencies
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonSelectedTracksEfficiencies ]


if True:
    InDetAlignMonResiduals = IDAlignMonResiduals(
        name = "InDetAlignMonResiduals_LoosePrimary",
        trackSelection = LPSelection,
        tracksName = "ExtendedTracks",
        useExtendedPlots = True,
        doClusterSizeHistos = True,
        triggerChainName = "LoosePrimary",
        Pixel_Manager = InDetKeys.PixelManager(),
        SCT_Manager = InDetKeys.SCT_Manager(),
        TRT_Manager = InDetKeys.TRT_Manager(),
        minPIXResXFillRange = -0.4,
        maxPIXResXFillRange =  0.4,
        minPIXResYFillRange = -0.80,
        maxPIXResYFillRange =  0.80,
        minSCTResFillRange  = -0.4,
        maxSCTResFillRange =   0.4,
        minTRTResidualWindow =-2.5,
        maxTRTResidualWindow = 2.5,
        NSplitMap  = 1,
        RangeOfPullHistos  =   5.0
        )
    printfunc (InDetAlignMonResiduals)
    
    #ToolSvc += InDetAlignMonResiduals
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonResiduals ]

    InDetAlignMonGenericTracks = IDAlignMonGenericTracks(
        name = "InDetAlignMonGenericTracks_LoosePrimary",
        trackSelection = LPSelection,
        tracksName = "ExtendedTracks",
        useExtendedPlots = True,
        triggerChainName = "LoosePrimary",
        d0Range = 5,
        z0Range = 200,
        #NTracksRange = 10,
        TrackToVertexIPEstimator = TrackingCommon.getTrackToVertexIPEstimator(),
        VxPrimContainerName = InDetKeys.PrimaryVertices()
        )
    #ToolSvc += InDetAlignMonGenericTracks
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonGenericTracks ]

    InDetAlignMonSelectedTracksEfficiencies = IDAlignMonEfficiencies (
        name = "InDetAlignMonEfficiencies_LoosePrimary",
        #doHoleSearch = False,
        trackSelection = LPSelection,
        tracksName = "ExtendedTracks",
        triggerChainName = "LoosePrimary",
        #HoleSearch = InDetExtendedHoleSearchTool,
        Pixel_Manager = InDetKeys.PixelManager(),
        SCT_Manager = InDetKeys.SCT_Manager(),
        TRT_Manager = InDetKeys.TRT_Manager()
        )

    #ToolSvc += InDetAlignMonSelectedTracksEfficiencies
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonSelectedTracksEfficiencies ]






#TriggerChainName is only the name passed to the monitoring folder

if newInDetAlignAlg_Options["useTrackSelector"]:
    printfunc (" <NewInDetAlignMonitoring> (type 2) Loading monitor for track Collection", tracks, " selection", alignTrackSelection.name)
    InDetAlignMonResiduals = IDAlignMonResiduals(
        name = "InDetAlignMonResiduals_"+tracks+"_sel",
        trackSelection = alignTrackSelection,
        tracksName = tracks,
        useExtendedPlots = True,
        doClusterSizeHistos = True,
        triggerChainName = "alignSelection",  
        Pixel_Manager = InDetKeys.PixelManager(),
        SCT_Manager = InDetKeys.SCT_Manager(),
        TRT_Manager = InDetKeys.TRT_Manager(),
        minPIXResXFillRange = -0.08,
        maxPIXResXFillRange =  0.08,
        minPIXResYFillRange = -0.25,
        maxPIXResYFillRange =  0.25,
        minSCTResFillRange  = -0.15,
        maxSCTResFillRange =   0.15,
        minTRTResidualWindow =-2.5,
        maxTRTResidualWindow = 2.5,
        NSplitMap  = 1,
        RangeOfPullHistos  =   5.0

    )
    printfunc ( InDetAlignMonResiduals)
    #ToolSvc += InDetAlignMonResiduals
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonResiduals ]

    InDetAlignMonGenericTracks = IDAlignMonGenericTracks(
        name = "InDetAlignMonGenericTracks_"+tracks+"_sel",
        trackSelection = alignTrackSelection,
        tracksName = tracks,
        useExtendedPlots = True,
        triggerChainName = "alignSelection",
        #doIPmaps = True,
        VxPrimContainerName = InDetKeys.PrimaryVertices(),
        TrackToVertexIPEstimator = TrackingCommon.getTrackToVertexIPEstimator()
    )
    #ToolSvc += InDetAlignMonGenericTracks
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonGenericTracks ]

    InDetAlignMonSelectedTracksEfficiencies = IDAlignMonEfficiencies (
        name = "InDetAlignMonEfficiencies_"+tracks+"_sel",
        trackSelection = alignTrackSelection,
        tracksName = tracks,
        #doHoleSearch = False,
        triggerChainName = "alignSelection",
        #HoleSearch = InDetExtendedHoleSearchTool,
        Pixel_Manager = InDetKeys.PixelManager(),
        SCT_Manager = InDetKeys.SCT_Manager(),
        TRT_Manager = InDetKeys.TRT_Manager()
    )
    #ToolSvc += InDetAlignMonSelectedTracksEfficiencies
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonSelectedTracksEfficiencies ]

if not Cosmics:
    from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import InDetAlignMonBeamSpot
    InDetAlignMonBeamSpot_noTrig = InDetAlignMonBeamSpot (name                           = "InDetAlignMonBeamSpot_noTrig",
                                                          vxContainerName                = InDetKeys.PrimaryVertices(),
                                                          vxContainerWithBeamConstraint  = InDetFlags.useBeamConstraint())
    #ToolSvc += InDetAlignMonBeamSpot_noTrig
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonBeamSpot_noTrig ]



#include("InDetPerformanceMonitoring/ElectronEoverPTracking.py")
#from InDetPerformanceMonitoring.InDetPerformanceMonitoringConf import IDPerfMonZmumu
#funIDPerfMonZmumu = IDPerfMonZmumu(name = 'IDPerfMonZmumu',
#                  ReFitterTool1 = MuonRefitterTool,
#                  ReFitterTool2 = MuonRefitterTool2,
#                  OutputTracksName =  "SelectedMuons", 
#                  isMC = True,
#                  doIsoSelection = False,
#                  OutputLevel =1)
# I hope is correct
#job+= funIDPerfMonZmumu


#RefitTrackCollections= ["SelectedMuonsRefit1","SelectedMuonsRefit2"]
#RefitTrackSelector  = InDet__InDetDetailedTrackSelectorTool(
#   name= "RefitTrackSelector",
#   OutputLevel=INFO,
#   TrackSummaryTool=InDetTrackSummaryTool,
#   pTMin = 10000.,
#   IPd0Max = 500.,
#   IPz0Max = 500.,
#   nHitPix = 1,
#   nHitPixPhysical = 1,
#   nHitBLayerPlusPix = 0,
#   nHitBLayer = 0,
#   nHitSi = 9,
#   nHitSiPhysical = 7,
#   nHitTrt = 0,
#   addToMinHitTrt=-3
#   )
#RefitTrackSelector.useEtaDepententMinHitTrt = True
#ToolSvc += RefitTrackSelector
'''
RefitAlignTrackSelection = InDetAlignMon__TrackSelectionTool(
    name = "InDetAlignMonAlignTrackSelectionTool",
    TrackSelectorTool = RefitTrackSelector),
    PrimVtxContainerName = InDetKeys.xAODVertexContainer(),
#ToolSvc+= RefitAlignTrackSelection


for trackCollection in RefitTrackCollections:
    InDetAlignMonResiduals = IDAlignMonResiduals(
                name = "InDetAlignMonResiduals_"+trackCollection,
                trackSelection = RefitAlignTrackSelection,
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
                NSplitMap = 1,
                RangeOfPullHistos  =   5
        )
    #ToolSvc += InDetAlignMonResiduals
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonResiduals ]
    InDetAlignMonGenericTracks = IDAlignMonGenericTracks(
                name = "InDetAlignMonGenericTracks_"+trackCollection,
                trackSelection = RefitAlignTrackSelection,#allSelection,
                tracksName = trackCollection,
                useExtendedPlots = True,
                triggerChainName = "all",
                VxPrimContainerName = InDetKeys.PrimaryVertices()
        )
    #ToolSvc += InDetAlignMonGenericTracks
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonGenericTracks ]

    InDetAlignMonSelectedTracksEfficiencies = IDAlignMonEfficiencies (
                name = "InDetAlignMonEfficiencies_"+trackCollection,
                trackSelection = RefitAlignTrackSelection,#allSelection,
                tracksName = trackCollection,
                triggerChainName = "all",
                #HoleSearch = InDetExtendedHoleSearchTool,
                Pixel_Manager = InDetKeys.PixelManager(),
                SCT_Manager = InDetKeys.SCT_Manager(),
                TRT_Manager = InDetKeys.TRT_Manager()
        )
    #ToolSvc += InDetAlignMonSelectedTracksEfficiencies
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonSelectedTracksEfficiencies ]

'''

if Cosmics:     
    printfunc (" <NewInDetAlignMonitoring> Setting track segment monitoring for various collections")
    InDetAlignMonManager.DataType = "cosmics"
    m_alignMonTrackSelectorTool  = []
    m_alignMonTrackSelectionTool = []
    # Cosmic running   
    #

    m_trackSelectorToolName       = ["AlignTracksTrackSelectorTool","AlignTracksTrackSelectorTool_Half",
                                     "InDetAlignCosmicTrackSelectorTool", "InDetAlignCosmicTrackSelector_Half",
                                     "PixTrackSelectorTool", "PixUpLowTrackSelectorTool",
                                     "SCTTrackSelectorTool", "SCTUpLowTrackSelectorTool",
                                     "SiTrackSelectorTool", "SiUpSiLowTrackSelectorTool",
                                     "TRTonlyTrackSelectorTool","TRTUpTRTLowTrackSelectorTool"]
    m_nHitBLayer                   = [ 0,  0,  0,  1,  2,  1,  0,  0,  2,  1,  0,  0]
    m_nHitPix                      = [ 1,  1,  1,  3,  5,  3,  0,  0,  5,  3,  0,  0]
    m_nHitSct                      = [ 8,  8,  8,  8,  0,  0, 14,  8, 14,  8,  0,  0]
    m_nHitTrt                      = [25, 25, 25, 25,  0,  0,  0,  0,  0,  0, 25, 25]

    m_alignMonTrackSelectionToolName = ["AlignTracksTrackSelectionTool","AlignTracksTrackSelectionTool_Half",
                                        "InDetAlignMonCosmicsTrackSelectionTool","InDetAlignMonTrackSelectionTool_Half",
                                        "PixTrackSelectionTool", "PixUpLowTrackSelectionTool",
                                        "SCTTrackSelectionTool","SCTUpLowTrackSelectionTool",
                                        "SiTrackSelectionTool", "SiUpSiLowTrackSelectionTool",
                                        "TRTonlyTrackSelectionTool","TRTUpTRTLowTrackSelectionTool" ]

    from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
    for i in range(len(m_alignMonTrackSelectionToolName)):
        printfunc (" <NewInDetAlignMonitoring> step ",i," --> setting track selector:", m_trackSelectorToolName[i]    )
        m_alignMonTrackSelectorTool.append(InDet__InDetDetailedTrackSelectorTool(name            = m_trackSelectorToolName[i],
                                                                                pTMin           = 2000,#1 GeV
                                                                                IPd0Max       = 1000.0,#no cut on d0 yet
                                                                                IPz0Max       = 1200.0,#actual cut is on sin(theta)*z0
                                                                                nHitBLayer     = m_nHitBLayer[i],
                                                                                nHitPix       = m_nHitPix[i],
                                                                                nHitSct       = m_nHitSct[i],
                                                                                nHitSi         = 0,
                                                                                nHitTrt       = m_nHitTrt[i],
                                                                                nSharedSct     = 0,
                                                                                TrackSummaryTool = InDetTrackSummaryTool,
                                                                                Extrapolator     = InDetExtrapolator))
        ToolSvc += m_alignMonTrackSelectorTool[i]
        if (InDetFlags.doPrintConfigurables()):
            printfunc (m_alignMonTrackSelectorTool[i])
    
           
        m_alignMonTrackSelectionTool.append(InDetAlignMon__TrackSelectionTool(name              = m_alignMonTrackSelectionToolName[i],
                                                                              ## Uncomment this line to bypass track slection
                                                                              #PassAllTracks      = True,
                                                                              #DoEventPhaseCut  = True,
                                                                              TrackSelectorTool   = m_alignMonTrackSelectorTool[i]))

        ToolSvc += m_alignMonTrackSelectionTool[i]
        if (InDetFlags.doPrintConfigurables()):
            printfunc (" <NewInDetAlignMonitoring> m_alignMonTrackSelectionTool[",i,"] =",  m_alignMonTrackSelectionToolName[i])
            printfunc (m_alignMonTrackSelectionTool[i])
        
    # Upper Vs Lower
    #
    from InDetTrackSplitterTool.InDetTrackSplitterToolConf import InDet__InDetTrackSplitterTool
    from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonTrackSegments

    m_trackSplitter       = []
    m_trackSplitterName   = ["TrackSplitterTool_AlignTracks","TrackSplitterTool_Combined","TrackSplitterTool_Pixel","TrackSplitterTool_SCT","TrackSplitterTool_Si","TrackSplitterTool_TRT"]
    #m_trackSplitterName   = ["TrackSplitterTool_AlignTracks","TrackSplitterTool_Combined"] # just to work with ALignTracks and CombinedInDetTracks
    m_upperTracksName     = ["AlignTracks_Upper", "Combined_Upper", "Pixel_Upper", "SCT_Upper", "Si_Upper", "TRT_Upper"]
    m_lowerTracksName     = ["AlignTracks_Lower", "Combined_Lower", "Pixel_Lower", "SCT_Lower", "Si_Lower", "TRT_Lower"]

    m_trackSegmentsUpLow     = []
    m_trackSegmentsUpLowName = ["InDetAlignMonTrackSegment_AlignTracks", "InDetAlignMonTrackSegments_Combined","InDetAlignMonTrackSegments_PixUpLow",
                                "InDetAlignMonTrackSegments_SCTUpLow", "InDetAlignMonTrackSegments_SiUpLow","InDetAlignMonTrackSegments_TRTUpLow"]
    
    m_inputTracksUpLow     = ["AlignTracks", "CombinedInDetTracks",InDetKeys.Tracks(),InDetKeys.PixelTracks(),InDetKeys.SCTTracks(),"ResolvedTracks",InDetKeys.TRTTracks()]

    m_d0Range               = [   100,   1000,    100,    100,    100,    100, 800.]
    m_deltaD0               = [   0.5,    0.5,    0.5,    0.8,   0.8,   0.5,     8.]
    m_deltaD02D             = [   1.0,    1.0,    1.0,    1.0,   1.0,   1.0,    10.]
    m_deltaPhi              = [ 0.002,   0.01,   0.002,  0.005, 0.005, 0.002, 0.01]
    m_deltaPhi2D            = [  0.02,   0.05,   0.02,   0.02,  0.02,  0.02,  0.05]
    m_deltaQoverPt          = [  0.02,    0.1,    0.02,   0.2,   0.05,  0.02,  0.1]
    m_deltaQoverPt2D        = [  0.05,    0.1,    0.05,   0.2,   0.05,  0.04,  0.1]

    for i in range(len(m_trackSplitterName)):
        printfunc (" <NewInDetAlignMonitoring> Setting track splitter:", m_trackSplitterName[i])
        m_trackSplitter.append(InDet__InDetTrackSplitterTool(name                 = m_trackSplitterName[i],
                                                             TrackFitter           = InDetTrackFitter,
                                                             OutputUpperTracksName = m_upperTracksName[i],
                                                             OutputLowerTracksName = m_lowerTracksName[i],
                                                             OutputLevel = WARNING))
       
        ToolSvc += m_trackSplitter[i]
        if (InDetFlags.doPrintConfigurables()):
            printfunc (m_trackSplitter[i])
            printfunc (" <NewInDetAlignMonitoring> step ",i," -> defined track segment monitoring for",m_trackSegmentsUpLowName[i],"using", m_upperTracksName[i],"and",  m_lowerTracksName[i], "   2*i+1=",2*i+1," m_alignMonTrackSelectionTool ? length=",len(m_alignMonTrackSelectionTool))
        m_trackSegmentsUpLow.append(IDAlignMonTrackSegments(name                = m_trackSegmentsUpLowName[i],
                                                            InputTracksName     = m_inputTracksUpLow[i],
                                                            UpperTracksName     = m_upperTracksName[i],
                                                            LowerTracksName     = m_lowerTracksName[i],
                                                            UseCTBSplitTracks   = False,
                                                            TrackSplitter       = m_trackSplitter[i],#Need the even ones
                                                            trackSelectionUp    = m_alignMonTrackSelectionTool[2*i+1],
                                                            trackSelectionLow   = m_alignMonTrackSelectionTool[2*i+1],
                                                            trackSumTool        = InDetTrackSummaryTool,
                                                            DeltaD0Range        = m_deltaD0[i],
                                                            DeltaD0Range2D      = m_deltaD02D[i],
                                                            DeltaPhiRange       = m_deltaPhi[i],
                                                            DeltaPhiRange2D     = m_deltaPhi2D[i],
                                                            DeltaQoverPtRange   = m_deltaQoverPt[i],
                                                            DeltaQoverPtRange2D = m_deltaQoverPt2D[i],
                                                            D0Range             = m_d0Range[i],
                                                            OutputLevel = WARNING))

        ToolSvc += m_trackSegmentsUpLow[i]
        InDetAlignMonManager.AthenaMonTools += [ m_trackSegmentsUpLow[i] ]
        printfunc (" <NewInDetAlignMonitoring> step ",i, m_trackSegmentsUpLowName[i]," added to the ToolSvc")

        # vvv trying to add monitoring of the splitted tracks
        # Upper track segments
        trackCollection = m_upperTracksName[i]
        
        printfunc (" <NewInDetAlignMonitoring> going to define InDetAlignMonResiduals for track collection: ",trackCollection)
        InDetAlignMonResiduals = IDAlignMonResiduals(
            name = "InDetAlignMonResiduals_"+trackCollection,
            trackSelection = allSelection,
            tracksName = trackCollection,
            useExtendedPlots = True,
            doClusterSizeHistos = True,
            triggerChainName = "all",
            Pixel_Manager = InDetKeys.PixelManager(),
            SCT_Manager = InDetKeys.SCT_Manager(),
            TRT_Manager = InDetKeys.TRT_Manager(),
            minPIXResXFillRange = -0.40,
            maxPIXResXFillRange =  0.40,
            minPIXResYFillRange = -0.80,
            maxPIXResYFillRange =  0.80,
            minSCTResFillRange  = -0.4,
            maxSCTResFillRange =   0.4,
            minTRTResidualWindow =-2.5,
            maxTRTResidualWindow = 2.5,
            NSplitMap  = 1,
            RangeOfPullHistos  =   5.00,
            OutputLevel = WARNING
            )
    
        ToolSvc += InDetAlignMonResiduals
        InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonResiduals ]
        printfunc (InDetAlignMonResiduals)

        printfunc (" <NewInDetAlignMonitoring> going to define InDetAlignMonGenericTracks for track collection: ",trackCollection)
        InDetAlignMonGenericTracks = IDAlignMonGenericTracks(
            name = "InDetAlignMonGenericTracks_"+trackCollection,
            trackSelection = allSelection,
            tracksName = trackCollection,
            useExtendedPlots = True,
            triggerChainName = "all",
            #doIPmaps = True,
            d0Range = 5,
            z0Range = 200,
            #NTracksRange = 10,
            VxPrimContainerName = InDetKeys.PrimaryVertices()
        )
        printfunc (InDetAlignMonGenericTracks)
        
        if Cosmics:
            InDetAlignMonGenericTracks.d0Range = 500
            InDetAlignMonGenericTracks.z0Range = 1000
        
        ToolSvc += InDetAlignMonGenericTracks
        InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonGenericTracks ]
        printfunc (" <NewInDetAlignMonitoring> going to define InDetAlignMonSelectedTracksEfficiencies for track collection: ",trackCollection)
        InDetAlignMonSelectedTracksEfficiencies = IDAlignMonEfficiencies (
            name = "InDetAlignMonEfficiencies_"+trackCollection,
            trackSelection = allSelection,
            #doHoleSearch= False,
            tracksName = trackCollection,
            triggerChainName = "all",
            #HoleSearch = InDetExtendedHoleSearchTool,
            Pixel_Manager = InDetKeys.PixelManager(),
            SCT_Manager = InDetKeys.SCT_Manager(),
            TRT_Manager = InDetKeys.TRT_Manager()
            )

        ToolSvc += InDetAlignMonSelectedTracksEfficiencies
        InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonSelectedTracksEfficiencies ]
        
        # Lower track segments
        trackCollection = m_lowerTracksName[i]
        
        printfunc (" <NewInDetAlignMonitoring> going to define InDetAlignMonResiduals for track collection: ",trackCollection)
        InDetAlignMonResiduals = IDAlignMonResiduals(
            name = "InDetAlignMonResiduals_"+trackCollection,
            trackSelection = allSelection,
            tracksName = trackCollection,
            useExtendedPlots = True,
            doClusterSizeHistos = True,
            triggerChainName = "all",
            Pixel_Manager = InDetKeys.PixelManager(),
            SCT_Manager = InDetKeys.SCT_Manager(),
            TRT_Manager = InDetKeys.TRT_Manager(),
            minPIXResXFillRange = -0.40,
            maxPIXResXFillRange =  0.40,
            minPIXResYFillRange = -0.80,
            maxPIXResYFillRange =  0.80,
            minSCTResFillRange  = -0.4,
            maxSCTResFillRange =   0.4,
            minTRTResidualWindow =-2.5,
            maxTRTResidualWindow = 2.5,
            NSplitMap  = 1,
            RangeOfPullHistos  =   5.00,
            OutputLevel = WARNING
            )
    
        ToolSvc += InDetAlignMonResiduals
        InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonResiduals ]
        printfunc (InDetAlignMonResiduals)

        printfunc (" <NewInDetAlignMonitoring> going to define InDetAlignMonGenericTracks for track collection: ",trackCollection)
        InDetAlignMonGenericTracks = IDAlignMonGenericTracks(
            name = "InDetAlignMonGenericTracks_"+trackCollection,
            trackSelection = allSelection,
            tracksName = trackCollection,
            useExtendedPlots = True,
            triggerChainName = "all",
            d0Range = 5,
            z0Range = 200,
            #doIPmaps = True,
            #NTracksRange = 10,
            VxPrimContainerName = InDetKeys.PrimaryVertices()
        )
        printfunc (InDetAlignMonGenericTracks)
        
        if Cosmics:
            InDetAlignMonGenericTracks.d0Range = 500
            InDetAlignMonGenericTracks.z0Range = 1000
        
        ToolSvc += InDetAlignMonGenericTracks
        InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonGenericTracks ]
        printfunc (" <NewInDetAlignMonitoring> going to define InDetAlignMonSelectedTracksEfficiencies for track collection: ",trackCollection)
        InDetAlignMonSelectedTracksEfficiencies = IDAlignMonEfficiencies (
            name = "InDetAlignMonEfficiencies_"+trackCollection,
            trackSelection = allSelection,
            #doHoleSearch= False,
            tracksName = trackCollection,
            triggerChainName = "all",
            #HoleSearch = InDetExtendedHoleSearchTool,
            Pixel_Manager = InDetKeys.PixelManager(),
            SCT_Manager = InDetKeys.SCT_Manager(),
            TRT_Manager = InDetKeys.TRT_Manager()
            )

        ToolSvc += InDetAlignMonSelectedTracksEfficiencies
        InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonSelectedTracksEfficiencies ]

        # ^^^
        if (InDetFlags.doPrintConfigurables()):
            printfunc (m_trackSegmentsUpLow[i])

     
    #
    # Subdetector Vs Subdetector
    #
    m_trackSegments_Sub  = []
    m_trackSegmentsName_Sub = ["InDetAlignMonTrackSegments_PixSCT","InDetAlignMonTrackSegments_SiTRT"]
    m_inputTracks_Sub      = [InDetKeys.Tracks(),InDetKeys.Tracks()]
    m_upperTracksName_Sub   = [InDetKeys.PixelTracks(),"ResolvedTracks"]
    m_trackSelectionUpper   = [2,6]
    m_lowerTracksName_Sub   = [InDetKeys.SCTTracks(),InDetKeys.TRTTracks()]
    m_trackSelectionLower   = [4,8]
    m_d0Range_Sub          = [   50, 800]
    m_deltaD0_Sub          = [  0.8,   8]
    m_deltaD02D_Sub      = [    5,  10]
    m_deltaPhi_Sub        = [0.005,0.01]
    m_deltaPhi2D_Sub        = [ 0.02,0.05]
    m_deltaQoverPt_Sub    = [ 0.05, 0.1]
    m_deltaQoverPt2D_Sub    = [ 0.05, 0.1]

    for i in range(2):
        m_trackSegments_Sub.append(IDAlignMonTrackSegments(name = m_trackSegmentsName_Sub[i],
                                                           InputTracksName       = m_inputTracks_Sub[i],
                                                           UpperTracksName       = m_upperTracksName_Sub[i],
                                                           LowerTracksName       = m_lowerTracksName_Sub[i],
                                                           trackSelectionUp     = m_alignMonTrackSelectionTool[m_trackSelectionUpper[i]],
                                                           trackSelectionLow       = m_alignMonTrackSelectionTool[m_trackSelectionLower[i]],
                                                           trackSumTool         = InDetTrackSummaryTool,
                                                           DeltaD0Range         = m_deltaD0_Sub[i],
                                                           DeltaD0Range2D         = m_deltaD02D_Sub[i],
                                                           DeltaPhiRange           = m_deltaPhi_Sub[i],
                                                           DeltaPhiRange2D       = m_deltaPhi2D_Sub[i],
                                                           DeltaQoverPtRange       = m_deltaQoverPt_Sub[i],
                                                           DeltaQoverPtRange2D   = m_deltaQoverPt2D_Sub[i],
                                                           D0Range               = m_d0Range_Sub[i]))

        ToolSvc += m_trackSegments_Sub[i]
        if (InDetFlags.doPrintConfigurables()):
            printfunc (m_trackSegments_Sub[i])
    
    #for i in range(5):
        # InDetAlignMonManager.AthenaMonTools += [ m_trackSegmentsUpLow[i] ]
    for i in range(2):
        InDetAlignMonManager.AthenaMonTools += [ m_trackSegments_Sub[i] ]

if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()


#include("AthenaMonitoring/DQMonFlagsConfig_jobOptions.py")
THistSvc         = Service( "THistSvc" )
#histOutput       = DQMonFlags.monManFileKey()+" DATAFILE='monitoring.root' OPT='RECREATE'"
histOutput       = "IDAlignMon DATAFILE='./monitoring.root' OPT='RECREATE'"
THistSvc.Output += [histOutput]
InDetAlignMonManager.FileKey = "IDAlignMon"
topSequence += InDetAlignMonManager
printfunc (InDetAlignMonManager)
