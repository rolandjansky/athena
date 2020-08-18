################################
# setup of alignment monitoring

from AthenaMonitoring.DQMonFlags import DQMonFlags
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager

InDetAlignMonManager = AthenaMonManager( name                = "InDetAlignMonManager",
                                         FileKey             = DQMonFlags.monManFileKey(),
                                         ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                                         DataType            = DQMonFlags.monManDataType(),
                                         Environment         = DQMonFlags.monManEnvironment(),
                                         ManualRunLBSetup    = DQMonFlags.monManManualRunLBSetup(),
                                         Run                 = DQMonFlags.monManRun(),
                                         LumiBlock           = DQMonFlags.monManLumiBlock())

tracks = InDetKeys.xAODTrackParticleContainer()

from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import InDetAlignMon__TrackSelectionTool
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool

# Tracks For LoosePrimary Selection

# All tracks selection
m_allSelection = InDetAlignMon__TrackSelectionTool(
    name = "InDetAlignMonTrackSelectionTool",
    PrimVtxContainerName = InDetKeys.xAODVertexContainer(),
    PassAllTracks = True, ## Uncomment this line to bypass track slection
)


m_TrackSelectorTool_LoosePrimary = InDet__InDetTrackSelectionTool(name         = "InDetTrackSelectorLoosePrimary",
                                                                  UseTrkTrackTools = True,
                                                                  minPt = 5000,
                                                                  maxD0 = 100000,
                                                                  maxZ0SinTheta = 150,
                                                                  minNTrtHits = 0,
                                                                  CutLevel = "LoosePrimary",
                                                                  TrackSummaryTool    = InDetTrackSummaryTool,
                                                                  Extrapolator        = InDetExtrapolator)
#ToolSvc += m_allSelection
#ToolSvc += m_TrackSelectorTool_LoosePrimary


#m_TrackSelectorTool_AlignTracks = InDet__InDetTrackSelectionTool(name         = "InDetTrackSelectorAlignTracks",
#                                                                 UseTrkTrackTools = True,
#                                                                 minPt = newInDetAlignAlg_Options["PtCut"],
#                                                                 maxD0 = 500,
#                                                                 maxZ0 = 500,
#                                                                 minNPixelHits = 1,
#                                                                 minNPixelHitsPhysical = 1,
#                                                                 minNSiHits = 5,
#                                                                 minNSiHitsPhysical = 7,
#                                                                 minNTrtHits  = 25,
#                                                                 CutLevel = "Loose",
#                                                                 TrackSummaryTool    = InDetTrackSummaryTool,
#                                                                 Extrapolator        = InDetExtrapolator)

#if Cosmics:
#    m_TrackSelectorTool_AlignTracks = InDet__InDetTrackSelectionTool(name         = "InDetTrackSelectorAlignTracks",
#                                                                     UseTrkTrackTools = True,
#                                                                     minPt = newInDetAlignAlg_Options["PtCut"],
#                                                                     maxD0 = 9999.,
#                                                                     maxZ0 = 9999.,
#                                                                     minNPixelHits = 1,
#                                                                     minNSctHits = 8,
#                                                                     minNSiHitsModTop = 2,
#                                                                     minNSiHitsModBottom = 2,
#                                                                     minNTrtHits = 25,
#                                                                     CutLevel = "Loose",
#                                                                     TrackSummaryTool    = InDetTrackSummaryTool,
#                                                                     Extrapolator        = InDetExtrapolator)
#
#ToolSvc += m_TrackSelectorTool_AlignTracks




LPSelection = InDetAlignMon__TrackSelectionTool(
    name = "InDetAlignMonTracksSelectionTool_LP",
    PrimVtxContainerName = InDetKeys.xAODVertexContainer(),
    UseIDTrackSelectionTool = True,
    IDTrackSelectionTool = m_TrackSelectorTool_LoosePrimary)
#ToolSvc += LPSelection


#AlignSelection = InDetAlignMon__TrackSelectionTool(
#    name = "InDetAlignMonTracksSelectionTool_AlignSelection",
#    PrimVtxContainerName = InDetKeys.xAODVertexContainer(),
#    UseIDTrackSelectionTool = True,
#    IDTrackSelectionTool = m_TrackSelectorTool_AlignTracks)
#ToolSvc += AlignSelection

# Align tracks selection
if newInDetAlignAlg_Options["useTrackSelector"]:
    alignTrackSelection = InDetAlignMon__TrackSelectionTool(
                            name = "InDetAlignMonAlignTrackSelectionTool",
                            PrimVtxContainerName = InDetKeys.xAODVertexContainer(),
                            UseIDTrackSelectionTool = True,
                            #PassAllTracks = True, ## Uncomment this line to bypass track slection
                            IDTrackSelectionTool = trackSelectorNew)
                            
    #ToolSvc += alignTrackSelection


from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonGenericTracks
from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonResiduals
from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonEfficiencies


InDetAlignMonResiduals = IDAlignMonResiduals(
    name = "InDetAlignMonResiduals_AlignSel",
    trackSelection = alignTrackSelection,
    #tracksName = "ExtendedTracks",
    tracksName = InDetKeys.xAODTrackParticleContainer(),
    useExtendedPlots = True,
    doClusterSizeHistos = True,
    triggerChainName = "AlignSel",
    Pixel_Manager = InDetKeys.PixelManager(),
    SCT_Manager = InDetKeys.SCT_Manager(),
    TRT_Manager = InDetKeys.TRT_Manager(),
    minPIXResXFillRange = -0.2,
    maxPIXResXFillRange =  0.2,
    minPIXResYFillRange = -0.5,
    maxPIXResYFillRange =  0.5,
    minSCTResFillRange  = -0.2,
    maxSCTResFillRange =   0.2,
    minTRTResidualWindow =-2.0,
    maxTRTResidualWindow = 2.0,
    NSplitMap  = 3,
    RangeOfPullHistos  =   5.0,
    OutputLevel=0
    )
printfunc (InDetAlignMonResiduals)

#ToolSvc += InDetAlignMonResiduals
InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonResiduals ]

InDetAlignMonGenericTracks = IDAlignMonGenericTracks(
    name = "InDetAlignMonGenericTracks_AlignSel",
    trackSelection = alignTrackSelection,
    tracksName = InDetKeys.xAODTrackParticleContainer(),
    useExtendedPlots = True,
    triggerChainName = "AlignSel",
    d0Range = 5,
    z0Range = 200,
    #NTracksRange = 10,
    VxPrimContainerName = InDetKeys.xAODVertexContainer()
    )
#ToolSvc += InDetAlignMonGenericTracks
InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonGenericTracks ]
if Cosmics:
    InDetAlignMonGenericTracks.d0Range = 500
    InDetAlignMonGenericTracks.z0Range = 1000

InDetAlignMonSelectedTracksEfficiencies = IDAlignMonEfficiencies (
    name = "InDetAlignMonEfficiencies_AlignSel",
    #doHoleSearch = False,
    trackSelection = alignTrackSelection,
    tracksName = InDetKeys.xAODTrackParticleContainer(),
    triggerChainName = "AlignSel",
    #HoleSearch = InDetExtendedHoleSearchTool,
    Pixel_Manager = InDetKeys.PixelManager(),
    SCT_Manager = InDetKeys.SCT_Manager(),
    TRT_Manager = InDetKeys.TRT_Manager()
    )

#ToolSvc += InDetAlignMonSelectedTracksEfficiencies
InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonSelectedTracksEfficiencies ]



InDetAlignMonResiduals = IDAlignMonResiduals(
    name = "InDetAlignMonResiduals_LoosePrimary",
    trackSelection = m_allSelection,
    #tracksName = "ExtendedTracks",
    tracksName = InDetKeys.xAODTrackParticleContainer(),
    useExtendedPlots = True,
    doClusterSizeHistos = True,
    triggerChainName = "LoosePrimary",
    Pixel_Manager = InDetKeys.PixelManager(),
    SCT_Manager = InDetKeys.SCT_Manager(),
    TRT_Manager = InDetKeys.TRT_Manager(),
    minPIXResXFillRange = -0.2,
    maxPIXResXFillRange =  0.2,
    minPIXResYFillRange = -0.5,
    maxPIXResYFillRange =  0.5,
    minSCTResFillRange  = -0.2,
    maxSCTResFillRange =   0.2,
    minTRTResidualWindow =-2.0,
    maxTRTResidualWindow = 2.0,
    NSplitMap  = 3,
    RangeOfPullHistos  =   5.0,
    OutputLevel=0
    )
printfunc (InDetAlignMonResiduals)

#ToolSvc += InDetAlignMonResiduals
InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonResiduals ]

InDetAlignMonGenericTracks = IDAlignMonGenericTracks(
    name = "InDetAlignMonGenericTracks_LoosePrimary",
    trackSelection = LPSelection,
    tracksName = InDetKeys.xAODTrackParticleContainer(),
    useExtendedPlots = True,
    triggerChainName = "LoosePrimary",
    d0Range = 5,
    z0Range = 200,
    #NTracksRange = 10,
    VxPrimContainerName = InDetKeys.xAODVertexContainer()
    )
#ToolSvc += InDetAlignMonGenericTracks
InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonGenericTracks ]
if Cosmics:
    InDetAlignMonGenericTracks.d0Range = 500
    InDetAlignMonGenericTracks.z0Range = 1000

InDetAlignMonSelectedTracksEfficiencies = IDAlignMonEfficiencies (
    name = "InDetAlignMonEfficiencies_LoosePrimary",
    #doHoleSearch = False,
    trackSelection = LPSelection,
    tracksName = InDetKeys.xAODTrackParticleContainer(),
    triggerChainName = "LoosePrimary",
    #HoleSearch = InDetExtendedHoleSearchTool,
    Pixel_Manager = InDetKeys.PixelManager(),
    SCT_Manager = InDetKeys.SCT_Manager(),
    TRT_Manager = InDetKeys.TRT_Manager()
    )

#ToolSvc += InDetAlignMonSelectedTracksEfficiencies
InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonSelectedTracksEfficiencies ]



InDetAlignMonResiduals = IDAlignMonResiduals(
    name = "InDetAlignMonResiduals_all",
    trackSelection = m_allSelection,
    #tracksName = "ExtendedTracks",
    tracksName = InDetKeys.xAODTrackParticleContainer(),
    useExtendedPlots = True,
    doClusterSizeHistos = True,
    triggerChainName = "all",
    Pixel_Manager = InDetKeys.PixelManager(),
    SCT_Manager = InDetKeys.SCT_Manager(),
    TRT_Manager = InDetKeys.TRT_Manager(),
    minPIXResXFillRange = -0.2,
    maxPIXResXFillRange =  0.2,
    minPIXResYFillRange = -0.5,
    maxPIXResYFillRange =  0.5,
    minSCTResFillRange  = -0.2,
    maxSCTResFillRange =   0.2,
    minTRTResidualWindow =-2.0,
    maxTRTResidualWindow = 2.0,
    NSplitMap  = 3,
    RangeOfPullHistos  =   5.0,
    OutputLevel=0
    )
printfunc (InDetAlignMonResiduals)

#ToolSvc += InDetAlignMonResiduals
InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonResiduals ]

InDetAlignMonGenericTracks = IDAlignMonGenericTracks(
    name = "InDetAlignMonGenericTracks_all",
    trackSelection = m_allSelection,
    tracksName = InDetKeys.xAODTrackParticleContainer(),
    useExtendedPlots = True,
    triggerChainName = "all",
    d0Range = 5,
    z0Range = 200,
    #NTracksRange = 10,
    VxPrimContainerName = InDetKeys.xAODVertexContainer()
    )
#ToolSvc += InDetAlignMonGenericTracks
InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonGenericTracks ]
if Cosmics:
    InDetAlignMonGenericTracks.d0Range = 500
    InDetAlignMonGenericTracks.z0Range = 1000

InDetAlignMonSelectedTracksEfficiencies = IDAlignMonEfficiencies (
    name = "InDetAlignMonEfficiencies_all",
    #doHoleSearch = False,
    trackSelection = m_allSelection,
    tracksName = InDetKeys.xAODTrackParticleContainer(),
    triggerChainName = "all",
    #HoleSearch = InDetExtendedHoleSearchTool,
    Pixel_Manager = InDetKeys.PixelManager(),
    SCT_Manager = InDetKeys.SCT_Manager(),
    TRT_Manager = InDetKeys.TRT_Manager()
    )

#ToolSvc += InDetAlignMonSelectedTracksEfficiencies
InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonSelectedTracksEfficiencies ]

#TriggerChainName is only the name passed to the monitoring folder

if not Cosmics:
    from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import InDetAlignMonBeamSpot
    InDetAlignMonBeamSpot_noTrig = InDetAlignMonBeamSpot (name                           = "InDetAlignMonBeamSpot_noTrig",
                                                          extrapolator                   = InDetExtrapolator,
                                                          vxContainerName                = InDetKeys.PrimaryVertices(),
                                                          vxContainerWithBeamConstraint  = InDetFlags.useBeamConstraint())
    #ToolSvc += InDetAlignMonBeamSpot_noTrig
    InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonBeamSpot_noTrig ]

###############################################    
# trying to implement the track segments for cosmics

if Cosmics:
    from InDetTrackSplitterTool.InDetTrackSplitterToolConf import InDet__InDetTrackSplitterTool
    from InDetTrackValidation.InDetTrackValidationConf import InDet__InDetSplittedTracksCreator 
    from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonTrackSegments
    
    #----- tools for track splliting ---- START ----
    splittertoolcomb= InDet__InDetTrackSplitterTool(name="VARO_SplitterTool",
                                                    TrackFitter           = InDetTrackFitter, #InDetTrackFitter,
                                                    OutputUpperTracksName = "TracksUpperSplit",
                                                    OutputLowerTracksName = "TracksLowerSplit",
                                                    OutputLevel = INFO) 
    #ToolSvc += splittertoolcomb
    printfunc (splittertoolcomb)

    # tool to convert to xAOD::TrackParticles 
    from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
    InDetxAODSplitParticleCreatorTool = Trk__TrackParticleCreatorTool(name = "InDetSplitxAODParticleCreatorTool", 
                                                                      Extrapolator            = InDetExtrapolator,
                                                                      TrackSummaryTool        = InDetTrackSummaryToolSharedHits,
                                                                      KeepParameters          = True, 
                                                                      OutputLevel = DEBUG)
    #ToolSvc += InDetxAODSplitParticleCreatorTool
    #----- tools for track splliting ---- COMPLETED ----

    #
    #### first task #### prepare the segment track selectors
    #
    
    #--> needed ? InDetAlignMonManager.DataType = "cosmics"
    m_alignMonTrackSelectorTool  = []
    m_alignMonTrackSelectionTool = []

    m_trackSelectorToolName       = ["AlignTracksTrackSelectorTool", "CombinedTracksTrackSelectorTool"]

    m_nHitBLayer                   = [ 0,  0]
    m_nHitPix                      = [ 1,  1]
    m_nHitSct                      = [ 7,  7]
    m_nHitSi                       = [ 9,  9]
    m_nHitTrt                      = [25, 25]

    m_alignMonTrackSelectionToolName = ["AlignTracksTrackSelectionTool", "CombinedTracksSelectionTool"]

    from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
    for i in range(len(m_alignMonTrackSelectionToolName)):
        printfunc (" <NewInDetAlignMonitoring> step ",i," --> setting track SELECTOR:", m_trackSelectorToolName[i]    )
        m_alignMonTrackSelectorTool.append(InDet__InDetDetailedTrackSelectorTool(name            = m_trackSelectorToolName[i],
                                                                                pTMin            = 1000,#1 GeV
                                                                                IPd0Max          = 1000.0,#no cut on d0 yet
                                                                                IPz0Max          = 1200.0,#actual cut is on sin(theta)*z0
                                                                                nHitBLayer       = m_nHitBLayer[i],
                                                                                nHitPix          = m_nHitPix[i],
                                                                                nHitSct          = m_nHitSct[i],
                                                                                nHitSi           = m_nHitSi[i],
                                                                                nHitTrt          = m_nHitTrt[i],
                                                                                nSharedSct       = 0,
                                                                                TrackSummaryTool = InDetTrackSummaryTool,
                                                                                Extrapolator     = InDetExtrapolator))
        #ToolSvc += m_alignMonTrackSelectorTool[i]
        if (InDetFlags.doPrintConfigurables()):
            printfunc (m_alignMonTrackSelectorTool[i])
    
           
        m_alignMonTrackSelectionTool.append(InDetAlignMon__TrackSelectionTool(name              = m_alignMonTrackSelectionToolName[i],
                                                                              ## Uncomment this line to bypass track slection
                                                                              #PassAllTracks      = True,
                                                                              #DoEventPhaseCut  = True,
                                                                              TrackSelectorTool   = m_alignMonTrackSelectorTool[i]))

        #ToolSvc += m_alignMonTrackSelectionTool[i]
        if (InDetFlags.doPrintConfigurables()):
            printfunc (" <NewInDetAlignMonitoring> ** SELECTION ** m_alignMonTrackSelectionTool[",i,"] =",  m_alignMonTrackSelectionToolName[i])
            printfunc (m_alignMonTrackSelectionTool[i])

    
    #
    ##### second task #### define the input track collections and the output segment collections
    #
    m_trackSplitter       = []
    m_trackSplitterName   = ["TrackSplitterTool_AlignTracks","TrackSplitterTool_Combined"]
    m_upperTracksName     = ["AlignTracks_Upper",            "Combined_Upper"]
    m_lowerTracksName     = ["AlignTracks_Lower",            "Combined_Lower"]
    #m_Splitter            = []
    
    m_trackSegmentsUpLow     = []
    m_trackSegmentsUpLowName = ["InDetAlignMonTrackSegment_AlignTracks", "InDetAlignMonTrackSegments_Combined"]
    
    m_inputTracksUpLow    = ["AlignTracks", "CombinedInDetTracks"]
    m_d0Range             = [ 200.0, 200.0]
    m_deltaD0             = [  0.25,  0.25]
    m_deltaD02D           = [   1.0,   1.0]
    m_deltaPhi            = [ 0.002, 0.002]
    m_deltaPhi2D          = [  0.02,  0.02]
    m_deltaQoverPt        = [ 0.010, 0.010]
    m_deltaQoverPt2D      = [  0.05,  0.05]

    for i in range(len(m_trackSplitterName)):
        printfunc (" <NewInDetAlignMonitoring> Setting track splitter",i,":", m_trackSplitterName[i], " for upper:",m_upperTracksName[i]," and lower:",  m_lowerTracksName[i] )
        m_trackSplitter.append(InDet__InDetTrackSplitterTool(name                  = m_trackSplitterName[i],
                                                             TrackFitter           = InDetTrackFitter,
                                                             OutputUpperTracksName = m_upperTracksName[i],
                                                             OutputLowerTracksName = m_lowerTracksName[i],
                                                             OutputLevel = INFO))
        #ToolSvc += m_trackSplitter[i] # add the track splitter to the ToolSvc
        if InDetFlags.doPrintConfigurables(): printfunc (m_trackSplitter[i])
        printfunc (" ")
        printfunc (" **** WARNING **** ")
        printfunc (" * ")
        printfunc (" *  in case of crash due to AttributeError: InDet__InDetSplittedTracksCreator object has no attribute takeUpperSegment ")
        printfunc (" *  please check out the following package: InnerDetector/InDetValidation/InDetTrackValidation ")
        printfunc (" *")
        printfunc (" *  thanks for your understanding"    )
        printfunc (" ")
        printfunc (" ***************** ")

            
        # monitoring part of the track segments matchinv
        m_trackSegmentsUpLow.append(IDAlignMonTrackSegments(name                = m_trackSegmentsUpLowName[i],
                                                            InputTracksName     = m_inputTracksUpLow[i],
                                                            UpperTracksName     = m_upperTracksName[i],
                                                            LowerTracksName     = m_lowerTracksName[i],
                                                            UseCTBSplitTracks   = False, # if True -> tracks were split somewhere else. if False -> tracks splitted here
                                                            TrackSplitter       = m_trackSplitter[i], #Need the even ones
                                                            trackSelectionUp    = m_alignMonTrackSelectionTool[i],#[2*i+1],
                                                            trackSelectionLow   = m_alignMonTrackSelectionTool[i],#[2*i+1],
                                                            trackSumTool        = InDetTrackSummaryTool,
                                                            DeltaD0Range        = m_deltaD0[i],
                                                            DeltaD0Range2D      = m_deltaD02D[i],
                                                            DeltaPhiRange       = m_deltaPhi[i],
                                                            DeltaPhiRange2D     = m_deltaPhi2D[i],
                                                            DeltaQoverPtRange   = m_deltaQoverPt[i],
                                                            DeltaQoverPtRange2D = m_deltaQoverPt2D[i],
                                                            D0Range             = m_d0Range[i],
                                                            OutputLevel = WARNING))

        #ToolSvc += m_trackSegmentsUpLow[i]
        InDetAlignMonManager.AthenaMonTools += [ m_trackSegmentsUpLow[i] ]
        printfunc (" <NewInDetAlignMonitoring> step ",i, m_trackSegmentsUpLowName[i]," added to the ToolSvc")

    #
    #### third task #### define monitoring histograms for each cosmic ray track segment 
    #
    # 1) create and fill the track-collections list
    m_trkcolls = []
    for i in range(len(m_trackSplitterName)):
        m_trkcolls.append(m_upperTracksName[i]+"Split") 
        m_trkcolls.append(m_lowerTracksName[i]+"Split")

        # 2) convert the tracks to xAOD
        # 2.1 ) upper track segments
        splitter_TakeUpper=InDet__InDetSplittedTracksCreator(name=m_inputTracksUpLow[i]+'_Splitter_TakeUpper',
                                                   TrackSplitterTool     = splittertoolcomb,
                                                   TrackCollection       = m_inputTracksUpLow[i], #"Tracks",
                                                   OutputTrackCollection = m_upperTracksName[i]+"Split", #"Tracks_split",
                                                   takeUpperSegment      = True,
                                                   takeLowerSegment      = False,
                                                   OutputLevel = INFO)
        topSequence += splitter_TakeUpper
        printfunc (splitter_TakeUpper)

        xAODSplitTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg(name=m_inputTracksUpLow[i]+'_Splitter_TakeUpper_xAOD',#name = 'InDetSplitTrackParticles',
                                                                      xAODContainerName ="xAOD::TrackParticleContainer", #'InDetSplitTrackParticles',
                                                                      xAODTrackParticlesFromTracksContainerName =  m_upperTracksName[i]+"Split",#'InDetSplitTrackParticles',
                                                                      TrackParticleCreator = InDetxAODSplitParticleCreatorTool,
                                                                      TrackContainerName = m_upperTracksName[i]+"Split", #'Tracks_split',
                                                                      ConvertTrackParticles = False,
                                                                      ConvertTracks = True,
                                                                      AddTruthLink = False,
                                                                      OutputLevel = INFO)
        topSequence += xAODSplitTrackParticleCnvAlg
        
        # 2.2 ) lower track segments (same as above)
        splitter_TakeLower=InDet__InDetSplittedTracksCreator(name=m_inputTracksUpLow[i]+'_Splitter_TakeLower',
                                                   TrackSplitterTool     = splittertoolcomb,
                                                   TrackCollection       = m_inputTracksUpLow[i], #"Tracks",
                                                   OutputTrackCollection = m_lowerTracksName[i]+"Split", 
                                                   takeUpperSegment      = False,
                                                   takeLowerSegment      = True,
                                                   OutputLevel = INFO)
        topSequence += splitter_TakeLower
        printfunc (splitter_TakeLower)

        xAODSplitTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg(name=m_inputTracksUpLow[i]+'_Splitter_TakeLower_xAOD',
                                                                      xAODContainerName ="xAOD::TrackParticleContainer", 
                                                                      xAODTrackParticlesFromTracksContainerName =  m_lowerTracksName[i]+"Split",
                                                                      TrackParticleCreator = InDetxAODSplitParticleCreatorTool,
                                                                      TrackContainerName = m_lowerTracksName[i]+"Split",
                                                                      ConvertTrackParticles = False,
                                                                      ConvertTracks = True,
                                                                      AddTruthLink = False,
                                                                      OutputLevel = INFO)
        topSequence += xAODSplitTrackParticleCnvAlg

        
    # 3) now create the monitoring histos
    for i in range(len(m_trkcolls)):
        printfunc (" <NewInDetAlignMonitoring> going to define InDetAlignMonResiduals for track collection: ", m_trkcolls[i])
        InDetAlignMonResiduals = IDAlignMonResiduals(
            name = "InDetAlignMonResiduals_"+m_trkcolls[i],
            trackSelection = m_allSelection,
            tracksName = m_trkcolls[i],
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
        #ToolSvc += InDetAlignMonResiduals
        InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonResiduals ]

        InDetAlignMonGenericTracks = IDAlignMonGenericTracks(
            name = "InDetAlignMonGenericTracks_"+m_trkcolls[i],
            trackSelection = m_allSelection,
            tracksName = m_trkcolls[i],
            useExtendedPlots = True,
            triggerChainName = "all",
            d0Range = 500,
            z0Range = 1000,
            VxPrimContainerName = InDetKeys.xAODVertexContainer()
            )
        #ToolSvc += InDetAlignMonGenericTracks
        InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonGenericTracks ]
        
        InDetAlignMonSelectedTracksEfficiencies = IDAlignMonEfficiencies (
            name = "InDetAlignMonEfficiencies_"+m_trkcolls[i],
            trackSelection = m_allSelection,
            tracksName = m_trkcolls[i],
            #doHoleSearch = False,
            triggerChainName = "all",
            #HoleSearch = InDetExtendedHoleSearchTool,
            Pixel_Manager = InDetKeys.PixelManager(),
            SCT_Manager = InDetKeys.SCT_Manager(),
            TRT_Manager = InDetKeys.TRT_Manager()
            )
        #ToolSvc += InDetAlignMonSelectedTracksEfficiencies
        InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonSelectedTracksEfficiencies ]
        

###############################################    
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
