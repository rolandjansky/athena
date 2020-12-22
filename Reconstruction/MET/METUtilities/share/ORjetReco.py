from JetRec.JetRecFlags import jetFlags
jetFlags.usePFlow = True

#########################################
# PseudoJet algorithms

gtrackget = CfgMgr.PseudoJetAlgorithm(
  "orgtrackget",
  InputContainer = "JetSelectedTracks",
  Label = "GhostTrack",
  OutputContainer = "ORPseudoJetGhostTracks",
  SkipNegativeEnergy = True,
  #GhostScale = 0.0,
  #OutputLevel=VERBOSE,
)
topSequence += gtrackget

pflowget = CfgMgr.PseudoJetAlgorithm(
  "orpflowget",
  InputContainer = "OverlapRemovedCHSParticleFlowObjects",
  Label = "EMPFlow", 
  OutputContainer = "ORPseudoJetPFlow",
  SkipNegativeEnergy = True,
  #GhostScale = 0.0,
  #OutputLevel=VERBOSE,
)

topSequence += pflowget

#########################################
# Modifiers

# Jet vertex fraction with selection.
JVFTool = CfgMgr.JetVertexFractionTool(
  "jvfOR",
  VertexContainer = "PrimaryVertices",
  AssociatedTracks = "GhostTrack",
  TrackVertexAssociation = "JetTrackVtxAssoc",
  TrackParticleContainer  = "InDetTrackParticles",
  #TrackSelector = trackselloose,
  JVFName = "JVF",
  K_JVFCorrScale = 0.01,
  #Z0Cut = 3.0,
  PUTrkPtCut = 30000.0,
)

JVFTool.JetContainer="AntiKt4OverlapRemovedEMPFlowJets"
ToolSvc += JVFTool

# Jet vertex tagger.
JVTTool = CfgMgr.JetVertexTaggerTool(
  "jvtOR",
  VertexContainer = "PrimaryVertices",
  # AssociatedTracks = "GhostTrack",
  # TrackVertexAssociation = tvassoc.TrackVertexAssociation,
  # TrackSelector = trackselloose,
  JVTName = "Jvt",

)

JVTTool.JetContainer="AntiKt4OverlapRemovedEMPFlowJets"

ToolSvc += JVTTool

# Jet track info.
JetTrackMomentsTool = CfgMgr.JetTrackMomentsTool(
  "trkmomsOR",
  JetContainer="AntiKt4OverlapRemovedEMPFlowJets",
  VertexContainer = "PrimaryVertices",
  AssociatedTracks = "GhostTrack",
  TrackVertexAssociation = "JetTrackVtxAssoc",
  TrackMinPtCuts = [500, 1000],
  #TrackSelector = trackselloose,
)

ToolSvc += JetTrackMomentsTool

# Jet track vector sum info
JetTrackSumMomentsTool = CfgMgr.JetTrackSumMomentsTool(
  "trksummomsOR",
  JetContainer="AntiKt4OverlapRemovedEMPFlowJets",
  VertexContainer = "PrimaryVertices",
  AssociatedTracks = "GhostTrack",
  TrackVertexAssociation = "JetTrackVtxAssoc",#tvassoc.TrackVertexAssociation,
  #InputContainer  = "InDetTrackParticles",
  RequireTrackPV = True,
  #TrackSelector = trackselloose,
)

ToolSvc+=JetTrackSumMomentsTool 


constitfourmom_pflow = CfgMgr.JetConstitFourMomTool(
  "constitfourmom_pflowOR",
  JetScaleNames = ["DetectorEtaPhi"],
  AltConstitColls = [""],
  AltConstitScales = [0],
  AltJetScales = ["JetConstitScaleMomentum"]
  )
ToolSvc+=constitfourmom_pflow

jetens = CfgMgr.JetCaloEnergies("jetensOR",
  JetContainer="AntiKt4OverlapRemovedEMPFlowJets"
)
ToolSvc+=jetens

ecpfsfrac = CfgMgr.JetECPSFractionTool("ecpsfracOR",
  JetContainer="AntiKt4OverlapRemovedEMPFlowJets"
)
ToolSvc+=ecpfsfrac

width= CfgMgr.JetWidthTool("widthOR",
  JetContainer="AntiKt4OverlapRemovedEMPFlowJets"
)
ToolSvc+=width

jetfilter=CfgMgr.JetFilterTool("jetfilter", PtMin=10000)
ToolSvc+=jetfilter

#########################################
# Calibration

from JetRec.JetRecCalibrationFinder import jrcf

JetRecCalibrationFinder = CfgMgr.JetCalibrationTool("JetCalibOR", 
		JetCollection="AntiKt4EMPFlow", 
		ConfigFile="JES_MC15cRecommendation_PFlow_Aug2016_rel21.config", 
		CalibSequence="JetArea_Residual_AbsoluteEtaJES", 
		RhoKey="Kt4EMPFlowEventShape",
		CalibArea="00-04-77")
ToolSvc+=JetRecCalibrationFinder


#########################################
# Set up the jet finder
JetFinder_AntiKt4 = CfgMgr.JetFinder("AntiKt4OverlapRemovedEMPFlowJetsFinder",
                                     JetAlgorithm = "AntiKt",
                                     JetRadius = 0.4,
                                     GhostArea = 0.01,
                                     PtMin = 5000,
                                     OutputLevel=INFO,
                                     )

#########################################
# Setup a jet builder 

JetBuilder_AntiKt4 = CfgMgr.JetFromPseudojet("jblda", Attributes = ["ActiveArea", "ActiveArea4vec"],
                                             OutputLevel=INFO)
JetFinder_AntiKt4.JetBuilder = JetBuilder_AntiKt4

#########################################
# Setup a JetRecTool 

JetRecTool = CfgMgr.JetRecTool("AntiKt4OREMPFlowJets",
			       InputPseudoJets = ["ORPseudoJetGhostTracks","ORPseudoJetPFlow"],
                               OutputLevel=INFO)
JetRecTool.JetFinder = JetFinder_AntiKt4
JetRecTool.OutputContainer = "AntiKt4OverlapRemovedEMPFlowJets"
JetRecTool.JetModifiers = [constitfourmom_pflow,jetens,JetTrackMomentsTool,JetTrackSumMomentsTool,JetRecCalibrationFinder,JVFTool,JVTTool,jetfilter,ecpfsfrac,width]
#JetRecTool.JetModifiers = [constitfourmom_pflow,jetens,JetTrackMomentsTool,JetTrackSumMomentsTool,JVFTool,JVTTool,jetfilter,ecpfsfrac,width]

#########################################
# JetAlgorithm
topSequence += CfgMgr.JetAlgorithm("jetalgAntiKt4OREMPFlowJets",Tools = [JetRecTool])

