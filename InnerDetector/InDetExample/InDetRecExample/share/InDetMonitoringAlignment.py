## set to true in case you would like to write out your own InDetAlignMon.root
InDetAlignMonDoOutput = False

## Set to True if you want to run the TruthComparison
InDetAlignMonDoTruth = False

## Set to True if you want to use RefittedTracks collection as opposed to ExtendedTracks (in collisions data)
## should use this when reading ESDs (ExtendedTracks does not exist, and Tracks is missing TrackParameters for hits)
## To use RefittedTracks collection requires also that InDetFlags.doRefit=True
InDetAlignMonReadESD = False

##Set to True if you want to use TriggerAware Alignment Monitoring
doTriggerAwareMonitoring = False


# Setup vertexing sequence without beam constraint.
# NOTE: Include the following BEFORE adding InDetAlignMonManager to topSequence.
# include('InDetAlignmentMonitoring/InDetAlignmentMonitoring_vertexing.py')


from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool           #loading the InDetTrackSelectionTool
from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import InDetAlignMon__TrackSelectionTool
#Here are to load the track selection tool.
#The alignMonTrackSelectionTool is a custom made trackselection tool that interface the monitoring tools to the track selection and make some 
#high level cuts (if everything passes, comTime, EventPhase, PrimaryVertex ..)
#It can call the InDetDetailedTrackSelectorTool or the InDetTrackSelectionTool, that actually make the track selection

m_alignMonTrackSelectorTool     =[]
m_alignMonTrackSelectionTool    =[]



if not jobproperties.Beam.beamType()=='cosmics': 
    #
    # Collisions and single beam running
    #
    # Track selector that implement the Tracking CP supported cuts for Run2 using loosePrimary cut
    m_alignMonTrackSelectorTool.append(InDet__InDetTrackSelectionTool(name         = "InDetTrackSelectionTool_LoosePrimary",
                                                                         UseTrkTrackTools = True,
                                                                         minPt = 5000,
                                                                         maxD0 = 100000,
                                                                         maxZ0SinTheta = 150,
                                                                         CutLevel = "LoosePrimary",
                                                                         TrackSummaryTool    = InDetTrackSummaryTool,
                                                                         Extrapolator        = InDetExtrapolator))
                                                                         
    
    # Track selector that implement the Tracking CP supported cuts for Run2 using tightPrimary cut
    m_alignMonTrackSelectorTool.append(InDet__InDetTrackSelectionTool(name         = "InDetTrackSelectionTool_NoTRT",
                                                                         UseTrkTrackTools = True,
                                                                         CutLevel = "TightPrimary",
                                                                         maxNPixelHoles = 1,
                                                                         minNBothInnermostLayersHits = 0,
                                                                         minNInnermostLayerHits = 1,
                                                                         minPt = 5000,
                                                                         TrackSummaryTool    = InDetTrackSummaryTool,
                                                                         Extrapolator        = InDetExtrapolator))
   
    if jobproperties.Beam.beamType()=='singlebeam':
        m_alignMonTrackSelectorTool[0].minPt   = 0.0
        m_alignMonTrackSelectorTool[0].maxZ0SinTheta = 100000.0
        m_alignMonTrackSelectorTool[1].minPt   = 0.0
        m_alignMonTrackSelectorTool[1].maxZ0SinTheta = 100000.0
    
    #Adding the tools to the Tool Service
        
    ToolSvc += m_alignMonTrackSelectorTool[0]
    ToolSvc += m_alignMonTrackSelectorTool[1]
    
    
    #Here we start creating the custom Track Selection tools.
    
    m_alignMonTrackSelectionTool.append(InDetAlignMon__TrackSelectionTool(name                     = "InDetAlignMonTrackSelectionTool_LoosePrimary",
                                                                          #PassAllTracks           = True, ## Uncomment this line to bypass track slection
                                                                          IDTrackSelectionTool     = m_alignMonTrackSelectorTool[0],
                                                                          PrimVtxContainerName     = InDetKeys.xAODVertexContainer(),
                                                                          UseIDTrackSelectionTool  = True))
    
    m_alignMonTrackSelectionTool.append(InDetAlignMon__TrackSelectionTool(name                     = "InDetAlignMonTrackSelectionTool_LoosePrimary_NoTRT",
                                                                          #PassAllTracks           = True, ## Uncomment this line to bypass track slection
                                                                          IDTrackSelectionTool     = m_alignMonTrackSelectorTool[1],
                                                                          PrimVtxContainerName     = InDetKeys.xAODVertexContainer(),
                                                                          UseIDTrackSelectionTool  = True))

    
    #Adding the TrackSelectionTools to the Tool Service
    ToolSvc += m_alignMonTrackSelectionTool[0]
    ToolSvc += m_alignMonTrackSelectionTool[1]
        
    #if (InDetFlags.doPrintConfigurables()):
    print m_alignMonTrackSelectionTool[0]
    print m_alignMonTrackSelectionTool[1]
                
    #Starting the creation of the monitoring tools
        
    
    
    if not jobproperties.Beam.beamType()=='singlebeam':
        
        from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonSivsTRT
        InDetAlignMonSivsTRT_noTrig = IDAlignMonSivsTRT (name                                = "InDetAlignMonSivsTRT_noTrig",
                                                         trackSelection                      = m_alignMonTrackSelectionTool[0])

        ToolSvc += InDetAlignMonSivsTRT_noTrig
        if (InDetFlags.doPrintConfigurables()):
            print InDetAlignMonSivsTRT_noTrig


        from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import InDetAlignMonBeamSpot
        InDetAlignMonBeamSpot_noTrig = InDetAlignMonBeamSpot (name                           = "InDetAlignMonBeamSpot_noTrig",
                                                              extrapolator                   = InDetExtrapolator,
                                                              vxContainerName                = InDetKeys.PrimaryVertices(),
                                                              vxContainerWithBeamConstraint  = InDetFlags.useBeamConstraint())
        
        ToolSvc += InDetAlignMonBeamSpot_noTrig
        if (InDetFlags.doPrintConfigurables()):
            print InDetAlignMonBeamSpot_noTrig


        # Note this is not to be included in the tool service
        from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonNtuple
        InDetAlignMonNtuple = IDAlignMonNtuple (name                                         = "InDetAlignMonNtuple",
                                                tracksName                                   = InDetKeys.ExtendedTracks(),
                                                tracksTruthName                              = InDetKeys.ExtendedTracksTruth())
        
else:
    #
    # Cosmics Running
    #
    
    # Original set of selectors

    m_trackSelectorToolName          = ["InDetAlignCosmicTrackSelectorTool","InDetAlignCosmicTrackSelector_Half","PixTrackSelectorTool",
                                        "PixUpLowTrackSelectorTool","SCTTrackSelectorTool","SCTUpLowTrackSelectorTool","SiTrackSelectorTool",
                                        "SiUpSiLowTrackSelectorTool","TRTonlyTrackSelectorTool","TRTUpTRTLowTrackSelectorTool"]

    
    
    m_minNInnermostLayerHits         = [ 0, 1,2,1, 0,0, 2,1, 0, 0]
    m_minNPixelHits                  = [ 3, 3,5,3, 0,0, 5,3, 0, 0]
    m_minNSctHits                    = [ 8, 8,0,0,14,8,14,8, 0, 0]
    m_minNTrtHits                    = [30,25,0,0, 0,0, 0,0,30,30]

    m_alignMonTrackSelectionToolName = ["InDetAlignMonTrackSelectionTool","InDetAlignMonTrackSelectionTool_Half","PixTrackSelectionTool",
                                        "PixUpLowTrackSelectionTool","SCTTrackSelectionTool","SCTUpLowTrackSelectionTool","SiTrackSelectionTool",
                                        "SiUpSiLowTrackSelectionTool","TRTonlyTrackSelectionTool","TRTUpTRTLowTrackSelectionTool"]

    
    
    for i in range(10):        
        m_alignMonTrackSelectorTool.append(InDet__InDetTrackSelectionTool(name                   = m_trackSelectorToolName[i],
                                                                          minPt                  = 1000.0,#1 GeV
                                                                          maxD0                  = 50.0,#no cut on d0 yet
                                                                          maxZ0SinTheta          = 1200.0,#actual cut is on sin(theta)*z0
                                                                          minNInnermostLayerHits = m_minNInnermostLayerHits[i], 
                                                                          minNPixelHits          = m_minNPixelHits[i], 
                                                                          minNSctHits            = m_minNSctHits[i], 
                                                                          minNTrtHits            = m_minNTrtHits[i], 
                                                                          UseTrkTrackTools       = True,
                                                                          CutLevel               = "LoosePrimary",
                                                                          TrackSummaryTool       = InDetTrackSummaryTool,
                                                                          Extrapolator           = InDetExtrapolator))
        ToolSvc += m_alignMonTrackSelectorTool[i]
        if (InDetFlags.doPrintConfigurables()):
            print m_alignMonTrackSelectorTool[i]
            
        
    
            
        m_alignMonTrackSelectionTool.append(InDetAlignMon__TrackSelectionTool(name                = m_alignMonTrackSelectionToolName[i],
                                                                              ## Uncomment this line to bypass track selection
                                                                              #PassAllTracks      = True,
                                                                              #DoEventPhaseCut    = True,
                                                                              UseIDTrackSelectionTool  = True,
                                                                              IDTrackSelectionTool     = m_alignMonTrackSelectorTool[i]))
        
        if jobproperties.Beam.beamType()=='singlebeam':
            m_alignMonTrackSelectionTool[i].PassAllTracks = True

        ToolSvc += m_alignMonTrackSelectionTool[i]
        if (InDetFlags.doPrintConfigurables()):
            print m_alignMonTrackSelectionTool[i]
    

#
# Residuals
#
from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonResiduals
InDetAlignMonResiduals_noTrig = IDAlignMonResiduals (name           = "InDetAlignMonResiduals_noTrig",
                                                     trackSelection = m_alignMonTrackSelectionTool[1],
                                                     iUpdator       = InDetUpdator,
                                                     propagator     = InDetPropagator,
                                                     Pixel_Manager  = InDetKeys.PixelManager(),
                                                     SCT_Manager    = InDetKeys.SCT_Manager(),
                                                     TRT_Manager    = InDetKeys.TRT_Manager())  

if jobproperties.Beam.beamType()=='cosmics' or jobproperties.Beam.beamType()=='singlebeam':
    InDetAlignMonResiduals_noTrig.tracksName = InDetKeys.Tracks()
else:
    InDetAlignMonResiduals_noTrig.tracksName = InDetKeys.ExtendedTracks()
    if InDetAlignMonReadESD and InDetFlags.doRefit: #use refitted tracks if reading ESD
        InDetAlignMonResiduals_noTrig.tracksName = InDetKeys.RefittedTracks()

ToolSvc += InDetAlignMonResiduals_noTrig
if (InDetFlags.doPrintConfigurables()):
    print InDetAlignMonResiduals_noTrig

#
# Efficiencies
#
from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonEfficiencies
InDetAlignMonEfficiencies_noTrig = IDAlignMonEfficiencies (name           = "InDetAlignMonEfficiencies_noTrig",
                                                           trackSelection = m_alignMonTrackSelectionTool[1],
                                                           HoleSearch     = InDetHoleSearchTool,
                                                           Pixel_Manager  = InDetKeys.PixelManager(),
                                                           SCT_Manager    = InDetKeys.SCT_Manager(),
                                                           TRT_Manager    = InDetKeys.TRT_Manager())    

if jobproperties.Beam.beamType()=='cosmics' or jobproperties.Beam.beamType()=='singlebeam':
    InDetAlignMonEfficiencies_noTrig.tracksName = InDetKeys.Tracks()
else:
    InDetAlignMonEfficiencies_noTrig.tracksName = InDetKeys.ExtendedTracks()
    if InDetAlignMonReadESD and InDetFlags.doRefit:#use refitted tracks if reading ESD
        InDetAlignMonEfficiencies_noTrig.tracksName = InDetKeys.RefittedTracks()

ToolSvc += InDetAlignMonEfficiencies_noTrig
if (InDetFlags.doPrintConfigurables()):
    print InDetAlignMonEfficiencies_noTrig

#
# Generic Tracks
#
from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonGenericTracks
InDetAlignMonGenericTracks_noTrig = IDAlignMonGenericTracks (name                = "InDetAlignMonGenericTracks_noTrig",
                                                             trackSelection      = m_alignMonTrackSelectionTool[1],
                                                             VxPrimContainerName = InDetKeys.xAODVertexContainer())   

if jobproperties.Beam.beamType()=='cosmics' or jobproperties.Beam.beamType()=='singlebeam':
    InDetAlignMonGenericTracks_noTrig.tracksName   = InDetKeys.Tracks()
    #InDetAlignMonGenericTracks_noTrig. tracksName = InDetKeys.PixelTracks()
else:
    InDetAlignMonGenericTracks_noTrig.tracksName = InDetKeys.ExtendedTracks()
    if InDetAlignMonReadESD and InDetFlags.doRefit: #use refitted tracks if reading ESD
        InDetAlignMonGenericTracks_noTrig.tracksName = InDetKeys.RefittedTracks()

ToolSvc += InDetAlignMonGenericTracks_noTrig
if (InDetFlags.doPrintConfigurables()):
    print InDetAlignMonGenericTracks_noTrig

#
# Track Segments
#
if jobproperties.Beam.beamType()=='cosmics':

    #
    # Upper Vs Lower
    #
    from InDetTrackSplitterTool.InDetTrackSplitterToolConf import InDet__InDetTrackSplitterTool
    from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonTrackSegments

    m_trackSplitter          = []
    m_trackSplitterName      = ["TrackSplitterTool_Combined","TrackSplitterTool_Pixel","TrackSplitterTool_SCT","TrackSplitterTool_Si","TrackSplitterTool_TRT"]
    m_upperTracksName        = ["Combined_Upper","Pixel_Upper","SCT_Upper","Si_Upper","TRT_Upper"]
    m_lowerTracksName        = ["Combined_Lower","Pixel_Lower","SCT_Lower","Si_Lower","TRT_Lower"]

    m_trackSegmentsUpLow     = []
    m_trackSegmentsUpLowName = ["InDetAlignMonTrackSegments_Combined","InDetAlignMonTrackSegments_PixUpLow","InDetAlignMonTrackSegments_SCTUpLow",
                                "InDetAlignMonTrackSegments_SiUpLow","InDetAlignMonTrackSegments_TRTUpLow"]

    m_inputTracksUpLow       = [InDetKeys.Tracks(),InDetKeys.PixelTracks(),InDetKeys.SCTTracks(),"ResolvedTracks",InDetKeys.TRTTracks()]

    m_d0Range                = [   50,   50,   50,   50, 800]
    m_deltaD0                = [  0.8,  0.8,  0.8,  0.8,   8]
    m_deltaD02D              = [    5,    5,    5,    5,  10]
    m_deltaPhi               = [0.005,0.005,0.005,0.005,0.01]
    m_deltaPhi2D             = [ 0.02, 0.02, 0.02, 0.02,0.05]
    m_deltaQoverPt           = [ 0.05,  0.2, 0.05, 0.04, 0.1]
    m_deltaQoverPt2D         = [ 0.05,  0.2, 0.05, 0.04, 0.1]

    for i in range(5):
        m_trackSplitter.append(InDet__InDetTrackSplitterTool(name                  = m_trackSplitterName[i],
                                                             TrackFitter           = InDetTrackFitter,
                                                             OutputUpperTracksName = m_upperTracksName[i],
                                                             OutputLowerTracksName = m_lowerTracksName[i]))
        
        ToolSvc += m_trackSplitter[i]
        if (InDetFlags.doPrintConfigurables()):
            print m_trackSplitter[i]

        m_trackSegmentsUpLow.append(IDAlignMonTrackSegments(name                   = m_trackSegmentsUpLowName[i],
                                                            InputTracksName        = m_inputTracksUpLow[i],
                                                            UpperTracksName        = m_upperTracksName[i],
                                                            LowerTracksName        = m_lowerTracksName[i],
                                                            UseCTBSplitTracks      = False,
                                                            TrackSplitter          = m_trackSplitter[i],#Need the even ones
                                                            trackSelectionUp       = m_alignMonTrackSelectionTool[2*i+1],
                                                            trackSelectionLow      = m_alignMonTrackSelectionTool[2*i+1],
                                                            trackSumTool           = InDetTrackSummaryTool,
                                                            DeltaD0Range           = m_deltaD0[i],
                                                            DeltaD0Range2D         = m_deltaD02D[i],
                                                            DeltaPhiRange          = m_deltaPhi[i],
                                                            DeltaPhiRange2D        = m_deltaPhi2D[i],
                                                            DeltaQoverPtRange      = m_deltaQoverPt[i],
                                                            DeltaQoverPtRange2D    = m_deltaQoverPt2D[i],
                                                            D0Range                = m_d0Range[i]))

        ToolSvc += m_trackSegmentsUpLow[i]
        if (InDetFlags.doPrintConfigurables()):
            print m_trackSegmentsUpLow[i]

    #
    # Subdetector Vs Subdetector
    #
    m_trackSegments_Sub     = []
    m_trackSegmentsName_Sub = ["InDetAlignMonTrackSegments_PixSCT","InDetAlignMonTrackSegments_SiTRT"]
    m_inputTracks_Sub       = [InDetKeys.Tracks(),InDetKeys.Tracks()]
    m_upperTracksName_Sub   = [InDetKeys.PixelTracks(),"ResolvedTracks"]
    m_trackSelectionUpper   = [2,6]
    m_lowerTracksName_Sub   = [InDetKeys.SCTTracks(),InDetKeys.TRTTracks()]
    m_trackSelectionLower   = [4,8]
    m_d0Range_Sub           = [   50, 800]
    m_deltaD0_Sub           = [  0.8,   8]
    m_deltaD02D_Sub         = [    5,  10]
    m_deltaPhi_Sub          = [0.005,0.01]
    m_deltaPhi2D_Sub        = [ 0.02,0.05]
    m_deltaQoverPt_Sub      = [ 0.05, 0.1]
    m_deltaQoverPt2D_Sub    = [ 0.05, 0.1]

    for i in range(2):
        m_trackSegments_Sub.append(IDAlignMonTrackSegments(name = m_trackSegmentsName_Sub[i],
                                                           InputTracksName         = m_inputTracks_Sub[i],
                                                           UpperTracksName         = m_upperTracksName_Sub[i],
                                                           LowerTracksName         = m_lowerTracksName_Sub[i],
                                                           trackSelectionUp        = m_alignMonTrackSelectionTool[m_trackSelectionUpper[i]],
                                                           trackSelectionLow       = m_alignMonTrackSelectionTool[m_trackSelectionLower[i]],
                                                           trackSumTool            = InDetTrackSummaryTool,
                                                           DeltaD0Range            = m_deltaD0_Sub[i],
                                                           DeltaD0Range2D          = m_deltaD02D_Sub[i],
                                                           DeltaPhiRange           = m_deltaPhi_Sub[i],
                                                           DeltaPhiRange2D         = m_deltaPhi2D_Sub[i],
                                                           DeltaQoverPtRange       = m_deltaQoverPt_Sub[i],
                                                           DeltaQoverPtRange2D     = m_deltaQoverPt2D_Sub[i],
                                                           D0Range                 = m_d0Range_Sub[i]))

        ToolSvc += m_trackSegments_Sub[i]
        if (InDetFlags.doPrintConfigurables()):
            print m_trackSegments_Sub[i]

## only do trigger-aware monitoring if monTrigDecTool known by ToolSvc
#if DQMonFlags.useTrigger():
#    if not hasattr(ToolSvc, DQMonFlags.nameTrigDecTool()):
#        print "InDetAlignmentMonitoring_InDetRec_jobOptions.py: trigger decision tool not found, including it now"
if not hasattr(ToolSvc, 'monTrigDecTool') or not doTriggerAwareMonitoring:
    print "InDetAlignmentMonitoring_InDetRec_jobOptions.py: trigger decision tool not found or monitoring disabled: don't run trigger-aware monitoring"  
elif jobproperties.Beam.beamType()=='cosmics' or jobproperties.Beam.beamType()=='singlebeam':
    print "InDetAlignmentMonitoring_InDetRec_jobOptions.py: cosmics or singlebeam beamType: don't run trigger-aware monitoring"
else:
    InDetAlignMonSivsTRT = IDAlignMonSivsTRT (name           = "InDetAlignMonSivsTRT",
                                              trackSelection = m_alignMonTrackSelectionTool[1])

    InDetAlignMonResiduals = IDAlignMonResiduals (name           = "InDetAlignMonResiduals",
                                                  trackSelection = m_alignMonTrackSelectionTool[1],
                                                  tracksName     = InDetKeys.ExtendedTracks(),
                                                  Pixel_Manager  = InDetKeys.PixelManager(),
                                                  SCT_Manager    = InDetKeys.SCT_Manager(),
                                                  TRT_Manager    = InDetKeys.TRT_Manager())   

    InDetAlignMonEfficiencies = IDAlignMonEfficiencies (name           = "InDetAlignMonEfficiencies",
                                                        trackSelection = m_alignMonTrackSelectionTool[1],
                                                        tracksName     = InDetKeys.ExtendedTracks(),
                                                        HoleSearch     = InDetHoleSearchTool,
                                                        Pixel_Manager  = InDetKeys.PixelManager(),
                                                        SCT_Manager    = InDetKeys.SCT_Manager(),
                                                        TRT_Manager    = InDetKeys.TRT_Manager())    

    InDetAlignMonGenericTracks = IDAlignMonGenericTracks (name                = "InDetAlignMonGenericTracks",
                                                          trackSelection      = m_alignMonTrackSelectionTool[1],
                                                          tracksName          = InDetKeys.ExtendedTracks(),
                                                          VxPrimContainerName =  InDetKeys.xAODVertexContainer())

    InDetAlignMonBeamSpot = InDetAlignMonBeamSpot (name                          = "InDetAlignMonBeamSpot",
                                                   vxContainerName               = InDetKeys.PrimaryVertices(),
                                                   vxContainerWithBeamConstraint = InDetFlags.useBeamConstraint())

    if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
        InDetAlignMonSivsTRT.FilterTools.append(monFilledBunchFilterTool)
        InDetAlignMonResiduals.FilterTools.append(monFilledBunchFilterTool)
        InDetAlignMonEfficiencies.FilterTools.append(monFilledBunchFilterTool)
        InDetAlignMonGenericTracks.FilterTools.append(monFilledBunchFilterTool)
        InDetAlignMonBeamSpot.FilterTools.append(monFilledBunchFilterTool)

    InDetAlignMonSivsTRT.TrigDecisionTool         = monTrigDecTool
    InDetAlignMonResiduals.TrigDecisionTool       = monTrigDecTool
    InDetAlignMonEfficiencies.TrigDecisionTool    = monTrigDecTool
    InDetAlignMonGenericTracks.TrigDecisionTool   = monTrigDecTool
    InDetAlignMonBeamSpot.TrigDecisionTool        = monTrigDecTool


    if rec.doHeavyIon():
        InDetAlignMonSivsTRT.TriggerChain             = "HLT_j30_ion_L1TE50"
        InDetAlignMonResiduals.TriggerChain           = "HLT_j30_ion_L1TE50"
        InDetAlignMonEfficiencies.TriggerChain        = "HLT_j30_ion_L1TE50"
        InDetAlignMonGenericTracks.TriggerChain       = "HLT_j30_ion_L1TE50"
        InDetAlignMonBeamSpot.TriggerChain            = "HLT_j30_ion_L1TE50"
    else:
        InDetAlignMonSivsTRT.TriggerChain             = "HLT_mu24_imedium"
        InDetAlignMonResiduals.TriggerChain           = "HLT_mu24_imedium"
        InDetAlignMonEfficiencies.TriggerChain        = "HLT_mu24_imedium"
        InDetAlignMonGenericTracks.TriggerChain       = "HLT_mu24_imedium"
        InDetAlignMonBeamSpot.TriggerChain            = "HLT_mu24_imedium"

    InDetAlignMonSivsTRT.triggerChainName         = "TriggerAwareMon"
    InDetAlignMonResiduals.triggerChainName       = "TriggerAwareMon"
    InDetAlignMonEfficiencies.triggerChainName    = "TriggerAwareMon"
    InDetAlignMonGenericTracks.triggerChainName   = "TriggerAwareMon"
    InDetAlignMonBeamSpot.histFolder              = "IDAlignMon/BeamSpot/TriggerAwareMon"

    ToolSvc += InDetAlignMonResiduals
    if (InDetFlags.doPrintConfigurables()):
        print InDetAlignMonResiduals
    ToolSvc += InDetAlignMonEfficiencies
    if (InDetFlags.doPrintConfigurables()):
        print InDetAlignMonEfficiencies
    ToolSvc += InDetAlignMonGenericTracks
    if (InDetFlags.doPrintConfigurables()):
        print InDetAlignMonGenericTracks
    ToolSvc += InDetAlignMonBeamSpot
    if (InDetFlags.doPrintConfigurables()):
        print InDetAlignMonBeamSpot
    ToolSvc += InDetAlignMonSivsTRT
    if (InDetFlags.doPrintConfigurables()):
        print InDetAlignMonSivsTRT
      
## add an AthenaMonManager algorithm to the list of algorithms to be ran
#from DataQualityTools.DQMonFlags import DQMonFlags
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

if jobproperties.Beam.beamType()=='cosmics':
    InDetAlignMonManager.DataType = "cosmics"

    for i in range(5):
        InDetAlignMonManager.AthenaMonTools += [ m_trackSegmentsUpLow[i] ]
    for i in range(2):
        InDetAlignMonManager.AthenaMonTools += [ m_trackSegments_Sub[i] ]
        
elif jobproperties.Beam.beamType()=='collisions':
    InDetAlignMonManager.AthenaMonTools     += [ InDetAlignMonSivsTRT_noTrig ]
    InDetAlignMonManager.AthenaMonTools     += [ InDetAlignMonBeamSpot_noTrig ]

InDetAlignMonManager.AthenaMonTools         += [ InDetAlignMonResiduals_noTrig ]
InDetAlignMonManager.AthenaMonTools         += [ InDetAlignMonEfficiencies_noTrig ]
InDetAlignMonManager.AthenaMonTools         += [ InDetAlignMonGenericTracks_noTrig ]

if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
    InDetAlignMonSivsTRT_noTrig.FilterTools.append(monFilledBunchFilterTool)
    InDetAlignMonResiduals_noTrig.FilterTools.append(monFilledBunchFilterTool)
    InDetAlignMonEfficiencies_noTrig.FilterTools.append(monFilledBunchFilterTool)
    InDetAlignMonGenericTracks_noTrig.FilterTools.append(monFilledBunchFilterTool)
    InDetAlignMonBeamSpot_noTrig.FilterTools.append(monFilledBunchFilterTool)


if InDetAlignMonDoTruth:
    InDetAlignMonManager.AthenaMonTools     += [ InDetAlignMonTruthComparison ]
if not hasattr(ToolSvc, 'monTrigDecTool') or not doTriggerAwareMonitoring:
    print "InDetAlignmentMonitoring_InDetRec_jobOptions.py: trigger decision tool not found: don't run trigger-aware monitoring" 
elif jobproperties.Beam.beamType()=='cosmics' or jobproperties.Beam.beamType()=='singlebeam':
    print "singlebeam or cosmics: don't run trigger-aware monitoring"
else:
    InDetAlignMonManager.AthenaMonTools     += [ InDetAlignMonResiduals ]
    InDetAlignMonManager.AthenaMonTools     += [ InDetAlignMonEfficiencies ]
    InDetAlignMonManager.AthenaMonTools     += [ InDetAlignMonGenericTracks ]
    InDetAlignMonManager.AthenaMonTools     += [ InDetAlignMonBeamSpot ]
    InDetAlignMonManager.AthenaMonTools     += [ InDetAlignMonSivsTRT ]

## Setup the output histogram file(s):
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
if InDetAlignMonDoOutput:
    THistSvc         = Service( "THistSvc" )
    histOutput       = "IDAlignMon DATAFILE='./IDAlignMon.root' OPT='RECREATE'"
    THistSvc.Output += [histOutput]
    InDetAlignMonManager.FileKey = "IDAlignMon"

topSequence += InDetAlignMonManager
if (InDetFlags.doPrintConfigurables()):
    print InDetAlignMonManager
