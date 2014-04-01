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

# track collection to monitor

tracks = "ExtendedTracks"
if Cosmics:
	tracks = "CombinedInDetTracks"
# if we're running Si only (TRT is off) we need to use different track collection
if not newInDetAlignAlg_Options["useTRT"]:
	tracks = "ResolvedTracks"

if newInDetAlignAlg_Options["doBSConstraint"] == False and newInDetAlignAlg_Options["doPVConstraint"] == False:
	trackCollections = [tracks,"AlignTracks","AlignTracksNormalRefitted"]
elif newInDetAlignAlg_Options["doBSConstraint"] == True and newInDetAlignAlg_Options["doPVConstraint"] == False:
	trackCollections = [tracks,"AlignTracks","AlignTracksNormalRefitted","AlignTracksBeamspotConstrained"]
elif newInDetAlignAlg_Options["doBSConstraint"] == False and newInDetAlignAlg_Options["doPVConstraint"] == True:
	trackCollections = [tracks,"AlignTracks","AlignTracksNormalRefitted","AlignTracksVertexConstrained"]
else:
	trackCollections = [tracks,"AlignTracks","AlignTracksNormalRefitted","AlignTracksBeamspotConstrained","AlignTracksVertexConstrained"]
from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import InDetAlignMon__TrackSelectionTool

# All tracks selection
allSelection = InDetAlignMon__TrackSelectionTool(
	name = "InDetAlignMonTrackSelectionTool",
	PassAllTracks = True, ## Uncomment this line to bypass track slection
)
ToolSvc += allSelection

# Align tracks selection
if newInDetAlignAlg_Options["useTrackSelector"]:
	alignTrackSelection = InDetAlignMon__TrackSelectionTool(
							name = "InDetAlignMonAlignTrackSelectionTool",
							#PassAllTracks = True, ## Uncomment this line to bypass track slection
							TrackSelectorTool = trackSelector
							)
	ToolSvc += alignTrackSelection


from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonResiduals
from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonGenericTracks
from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonEfficiencies

for trackCollection in trackCollections:
	# Selected tracks
	print "Loading monitoring"
	InDetAlignMonResiduals = IDAlignMonResiduals(
		name = "InDetAlignMonResiduals_"+trackCollection,
		trackSelection = allSelection,
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
	
	ToolSvc += InDetAlignMonResiduals
	InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonResiduals ]

	InDetAlignMonGenericTracks = IDAlignMonGenericTracks(
		name = "InDetAlignMonGenericTracks_"+trackCollection,
		trackSelection = allSelection,
		tracksName = trackCollection,
		useExtendedPlots = True,
		triggerChainName = "all",
		VxPrimContainerName = InDetKeys.PrimaryVertices()
	)
	ToolSvc += InDetAlignMonGenericTracks
	InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonGenericTracks ]

	InDetAlignMonSelectedTracksEfficiencies = IDAlignMonEfficiencies (
		name = "InDetAlignMonEfficiencies_"+trackCollection,
		trackSelection = allSelection,
		tracksName = trackCollection,
		triggerChainName = "all",
		#HoleSearch = InDetExtendedHoleSearchTool,
		Pixel_Manager = InDetKeys.PixelManager(),
		SCT_Manager = InDetKeys.SCT_Manager(),
		TRT_Manager = InDetKeys.TRT_Manager()
	)
	ToolSvc += InDetAlignMonSelectedTracksEfficiencies
	InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonSelectedTracksEfficiencies ]


if newInDetAlignAlg_Options["useTrackSelector"]:
	InDetAlignMonResiduals = IDAlignMonResiduals(
		name = "InDetAlignMonResiduals_"+tracks+"_sel",
		trackSelection = alignTrackSelection,
		tracksName = tracks,
		useExtendedPlots = True,
		triggerChainName = "alignSelection",
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
	ToolSvc += InDetAlignMonResiduals
	InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonResiduals ]

	InDetAlignMonGenericTracks = IDAlignMonGenericTracks(
		name = "InDetAlignMonGenericTracks_"+tracks+"_sel",
		trackSelection = alignTrackSelection,
		tracksName = tracks,
		useExtendedPlots = True,
		triggerChainName = "alignSelection",
		VxPrimContainerName = InDetKeys.PrimaryVertices()
	)
	ToolSvc += InDetAlignMonGenericTracks
	InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonGenericTracks ]

	InDetAlignMonSelectedTracksEfficiencies = IDAlignMonEfficiencies (
		name = "InDetAlignMonEfficiencies_"+tracks+"_sel",
		trackSelection = alignTrackSelection,
		tracksName = tracks,
		triggerChainName = "alignSelection",
		#HoleSearch = InDetExtendedHoleSearchTool,
		Pixel_Manager = InDetKeys.PixelManager(),
		SCT_Manager = InDetKeys.SCT_Manager(),
		TRT_Manager = InDetKeys.TRT_Manager()
	)
	ToolSvc += InDetAlignMonSelectedTracksEfficiencies
	InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonSelectedTracksEfficiencies ]

if not Cosmics:
	from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import InDetAlignMonBeamSpot
	InDetAlignMonBeamSpot_noTrig = InDetAlignMonBeamSpot (name                           = "InDetAlignMonBeamSpot_noTrig",
														  extrapolator                   = InDetExtrapolator,
														  vxContainerName                = InDetKeys.PrimaryVertices(),
														  vxContainerWithBeamConstraint  = InDetFlags.useBeamConstraint())
	ToolSvc += InDetAlignMonBeamSpot_noTrig
	InDetAlignMonManager.AthenaMonTools += [ InDetAlignMonBeamSpot_noTrig ]

if Cosmics:		
	InDetAlignMonManager.DataType = "cosmics"
	m_alignMonTrackSelectorTool  = []
	m_alignMonTrackSelectionTool = []
	# Cosmic running   
	#

	m_trackSelectorToolName		  = ["InDetAlignCosmicTrackSelectorTool","InDetAlignCosmicTrackSelector_Half","PixTrackSelectorTool",
										"PixUpLowTrackSelectorTool","SCTTrackSelectorTool","SCTUpLowTrackSelectorTool","SiTrackSelectorTool",
										"SiUpSiLowTrackSelectorTool","TRTonlyTrackSelectorTool","TRTUpTRTLowTrackSelectorTool"]
	m_nHitBLayer					 = [ 0, 1,2,1, 0,0, 2,1, 0, 0]
	m_nHitPix						= [ 3, 3,5,3, 0,0, 5,3, 0, 0]
	m_nHitSct						= [ 8, 8,0,0,14,8,14,8, 0, 0]
	m_nHitTrt						= [30,25,0,0, 0,0, 0,0,30,30]

	m_alignMonTrackSelectionToolName = ["InDetAlignMonCosmicsTrackSelectionTool","InDetAlignMonTrackSelectionTool_Half","PixTrackSelectionTool",
										"PixUpLowTrackSelectionTool","SCTTrackSelectionTool","SCTUpLowTrackSelectionTool","SiTrackSelectionTool",
										"SiUpSiLowTrackSelectionTool","TRTonlyTrackSelectionTool","TRTUpTRTLowTrackSelectionTool" ]
	from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
	for i in range(10):	   
		m_alignMonTrackSelectorTool.append(InDet__InDetDetailedTrackSelectorTool(name			 = m_trackSelectorToolName[i],
																				pTMin			= 0,#1 GeV
																				IPd0Max		  = 1000.0,#no cut on d0 yet
																				IPz0Max		  = 1200.0,#actual cut is on sin(theta)*z0
																				nHitBLayer	   = m_nHitBLayer[i],
																				nHitPix		  = m_nHitPix[i],
																				nHitSct		  = m_nHitSct[i],
																				nHitSi		   = 0,
																				nHitTrt		  = m_nHitTrt[i],
																				nSharedSct	   = 0,
																				TrackSummaryTool = InDetTrackSummaryTool,
																				Extrapolator	 = InDetExtrapolator))
		ToolSvc += m_alignMonTrackSelectorTool[i]
		if (InDetFlags.doPrintConfigurables()):
			print m_alignMonTrackSelectorTool[i]
	
	   
		m_alignMonTrackSelectionTool.append(InDetAlignMon__TrackSelectionTool(name				= m_alignMonTrackSelectionToolName[i],
																			  ## Uncomment this line to bypass track slection
																			  #PassAllTracks	  = True,
																			  #DoEventPhaseCut	= True,
																			  TrackSelectorTool   = m_alignMonTrackSelectorTool[i]))

		ToolSvc += m_alignMonTrackSelectionTool[i]
		if (InDetFlags.doPrintConfigurables()):
			print m_alignMonTrackSelectionTool[i]
	
	# Upper Vs Lower
	#
	from InDetTrackSplitterTool.InDetTrackSplitterToolConf import InDet__InDetTrackSplitterTool
	from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import IDAlignMonTrackSegments

	m_trackSplitter		  = []
	m_trackSplitterName	  = ["TrackSplitterTool_Combined","TrackSplitterTool_Pixel","TrackSplitterTool_SCT","TrackSplitterTool_Si","TrackSplitterTool_TRT"]
	m_upperTracksName		= ["Combined_Upper","Pixel_Upper","SCT_Upper","Si_Upper","TRT_Upper"]
	m_lowerTracksName		= ["Combined_Lower","Pixel_Lower","SCT_Lower","Si_Lower","TRT_Lower"]

	m_trackSegmentsUpLow	 = []
	m_trackSegmentsUpLowName = ["InDetAlignMonTrackSegments_Combined","InDetAlignMonTrackSegments_PixUpLow","InDetAlignMonTrackSegments_SCTUpLow",
								"InDetAlignMonTrackSegments_SiUpLow","InDetAlignMonTrackSegments_TRTUpLow"]
	
	m_inputTracksUpLow	   = [InDetKeys.Tracks(),InDetKeys.PixelTracks(),InDetKeys.SCTTracks(),"ResolvedTracks",InDetKeys.TRTTracks()]

	m_d0Range				= [   100,   100,   100,   100, 800]
	m_deltaD0				= [  0.2,  0.8,  0.8,  0.2,   8]
	m_deltaD02D			  = [	5,	5,	5,	5,  10]
	m_deltaPhi			   = [0.002,0.005,0.005,0.002,0.01]
	m_deltaPhi2D			 = [ 0.02, 0.02, 0.02, 0.02,0.05]
	m_deltaQoverPt		   = [ 0.02,  0.2, 0.05, 0.02, 0.1]
	m_deltaQoverPt2D		 = [ 0.05,  0.2, 0.05, 0.04, 0.1]

	for i in range(5):
		m_trackSplitter.append(InDet__InDetTrackSplitterTool(name				  = m_trackSplitterName[i],
															 TrackFitter		   = InDetTrackFitter,
															 OutputUpperTracksName = m_upperTracksName[i],
															 OutputLowerTracksName = m_lowerTracksName[i]))
	   
		ToolSvc += m_trackSplitter[i]
		if (InDetFlags.doPrintConfigurables()):
			print m_trackSplitter[i]

		m_trackSegmentsUpLow.append(IDAlignMonTrackSegments(name				   = m_trackSegmentsUpLowName[i],
															InputTracksName		= m_inputTracksUpLow[i],
															UpperTracksName		= m_upperTracksName[i],
															LowerTracksName		= m_lowerTracksName[i],
															UseCTBSplitTracks	  = False,
															TrackSplitter		  = m_trackSplitter[i],#Need the even ones
															trackSelectionUp	   = m_alignMonTrackSelectionTool[2*i+1],
															trackSelectionLow	  = m_alignMonTrackSelectionTool[2*i+1],
															trackSumTool		   = InDetTrackSummaryTool,
															DeltaD0Range		   = m_deltaD0[i],
															DeltaD0Range2D		 = m_deltaD02D[i],
															DeltaPhiRange		  = m_deltaPhi[i],
															DeltaPhiRange2D		= m_deltaPhi2D[i],
															DeltaQoverPtRange	  = m_deltaQoverPt[i],
															DeltaQoverPtRange2D	= m_deltaQoverPt2D[i],
															D0Range				= m_d0Range[i]))

		ToolSvc += m_trackSegmentsUpLow[i]
		if (InDetFlags.doPrintConfigurables()):
			print m_trackSegmentsUpLow[i]

	#
	# Subdetector Vs Subdetector
	#
	m_trackSegments_Sub	 = []
	m_trackSegmentsName_Sub = ["InDetAlignMonTrackSegments_PixSCT","InDetAlignMonTrackSegments_SiTRT"]
	m_inputTracks_Sub	   = [InDetKeys.Tracks(),InDetKeys.Tracks()]
	m_upperTracksName_Sub   = [InDetKeys.PixelTracks(),"ResolvedTracks"]
	m_trackSelectionUpper   = [2,6]
	m_lowerTracksName_Sub   = [InDetKeys.SCTTracks(),InDetKeys.TRTTracks()]
	m_trackSelectionLower   = [4,8]
	m_d0Range_Sub		   = [   50, 800]
	m_deltaD0_Sub		   = [  0.8,   8]
	m_deltaD02D_Sub		 = [	5,  10]
	m_deltaPhi_Sub		  = [0.005,0.01]
	m_deltaPhi2D_Sub		= [ 0.02,0.05]
	m_deltaQoverPt_Sub	  = [ 0.05, 0.1]
	m_deltaQoverPt2D_Sub	= [ 0.05, 0.1]

	for i in range(2):
		m_trackSegments_Sub.append(IDAlignMonTrackSegments(name = m_trackSegmentsName_Sub[i],
														   InputTracksName		 = m_inputTracks_Sub[i],
														   UpperTracksName		 = m_upperTracksName_Sub[i],
														   LowerTracksName		 = m_lowerTracksName_Sub[i],
														   trackSelectionUp		= m_alignMonTrackSelectionTool[m_trackSelectionUpper[i]],
														   trackSelectionLow	   = m_alignMonTrackSelectionTool[m_trackSelectionLower[i]],
														   trackSumTool			= InDetTrackSummaryTool,
														   DeltaD0Range			= m_deltaD0_Sub[i],
														   DeltaD0Range2D		  = m_deltaD02D_Sub[i],
														   DeltaPhiRange		   = m_deltaPhi_Sub[i],
														   DeltaPhiRange2D		 = m_deltaPhi2D_Sub[i],
														   DeltaQoverPtRange	   = m_deltaQoverPt_Sub[i],
														   DeltaQoverPtRange2D	 = m_deltaQoverPt2D_Sub[i],
														   D0Range				 = m_d0Range_Sub[i]))

		ToolSvc += m_trackSegments_Sub[i]
		if (InDetFlags.doPrintConfigurables()):
			print m_trackSegments_Sub[i]
	
	for i in range(5):
		InDetAlignMonManager.AthenaMonTools += [ m_trackSegmentsUpLow[i] ]
	for i in range(2):
		InDetAlignMonManager.AthenaMonTools += [ m_trackSegments_Sub[i] ]
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()

THistSvc         = Service( "THistSvc" )

histOutput       = DQMonFlags.monManFileKey()+" DATAFILE='monitoring.root' OPT='RECREATE'" 
THistSvc.Output += [histOutput]
#InDetAlignMonManager.FileKey = "IDAlignMon"
topSequence += InDetAlignMonManager
print InDetAlignMonManager
