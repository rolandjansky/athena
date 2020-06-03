include.block ("JetMissingEtTagTools/JetTagTool_jobOptions.py")

#====================================================================
# Check if we have Monte Carlo or real data, based on the inputFileSummary
#====================================================================
inputIsSimulation = False
if inputFileSummary.has_key("evt_type"):
	eventTypeList = inputFileSummary.get("evt_type")
	if eventTypeList.__contains__("IS_SIMULATION") :
		print "Detected that the input file is a simulated dataset"
		inputIsSimulation = True
	else:
		print "Detected that the input file is real data"
		pass

calibseq = "JetArea_Residual_EtaJES_GSC"
evs = "Kt4EMTopoOriginEventShape"
isdata = False
# options for data
if not inputIsSimulation:
	calibseq += "_Insitu"
	isdata = True

# override for cosmics where neither PVs nor track moments are available
from AthenaCommon.BeamFlags import jobproperties
if jobproperties.Beam.beamType == 'cosmics':
	print "Working on a cosmics file, use alternate JetCalib config"
	calibseq = "JetArea_EtaJES"
	evs = "Kt4EMTopoEventShape"
	
JetCalibTool = CfgMgr.JetCalibrationTool("JetCalib",
					 JetCollection = "AntiKt4EMTopo",
					 ConfigFile    = "JES_data2016_data2015_Recommendation_Dec2016.config",
					 CalibSequence = calibseq,
					 IsData = isdata,
					 RhoKey=evs
					 )

ToolSvc += JetCalibTool

JetVertexTaggerTool = CfgMgr.JetVertexTaggerTool('JVT')
ToolSvc += JetVertexTaggerTool

METMakerTool = CfgMgr.met__METMaker('METMaker',
				    DoRemoveMuonJets    = True,
				    DoSetMuonJetEMScale = True,
				    JetJvtMomentName    = 'Jvt',
				    CustomJetJvtCut     = 0.59,
				    CustomJetJvtPtMax   = 60e3
				    )
ToolSvc += METMakerTool

FlvTagCutDefinitionsFileName = "xAODBTaggingEfficiency/13TeV/2017-21-13TeV-MC16-CDI-2018-06-24_v1.root"
MaxEta         = 2.5
MinPt          = 20000.
JetAuthor      = "AntiKt4EMTopoJets"
TaggerName     = "MV2c10"

BTagTool_FixedCutBEff_60 = CfgMgr.BTaggingSelectionTool('FixedCutBEff_60',
							MaxEta         = MaxEta,
							MinPt          = MinPt,
							JetAuthor      = JetAuthor,
							TaggerName     = TaggerName,
							FlvTagCutDefinitionsFileName = FlvTagCutDefinitionsFileName,
							OperatingPoint = "FixedCutBEff_60")
ToolSvc += BTagTool_FixedCutBEff_60

BTagTool_FixedCutBEff_70 = CfgMgr.BTaggingSelectionTool('FixedCutBEff_70',
							MaxEta         = MaxEta,
							MinPt          = MinPt,
							JetAuthor      = JetAuthor,
							TaggerName     = TaggerName,
							FlvTagCutDefinitionsFileName = FlvTagCutDefinitionsFileName,
							OperatingPoint = "FixedCutBEff_70")
ToolSvc += BTagTool_FixedCutBEff_70

BTagTool_FixedCutBEff_85 = CfgMgr.BTaggingSelectionTool('FixedCutBEff_85',
							MaxEta         = MaxEta,
							MinPt          = MinPt,
							JetAuthor      = JetAuthor,
							TaggerName     = TaggerName,
							FlvTagCutDefinitionsFileName = FlvTagCutDefinitionsFileName,
							OperatingPoint = "FixedCutBEff_85")
ToolSvc += BTagTool_FixedCutBEff_85

#FlvTagCutDefinitionsFileName = "xAODBTaggingEfficiency/13TeV/2015-PreRecomm-13TeV-MC12-CDI-October23_v1.root"
#BTagTool_FlatBEff_60 = CfgMgr.BTaggingSelectionTool('FlatBEff_60',
#						    MaxEta         = MaxEta,
#						    MinPt          = MinPt,
#						    JetAuthor      = JetAuthor,
#						    TaggerName     = TaggerName,
#						    FlvTagCutDefinitionsFileName = FlvTagCutDefinitionsFileName,
#						    OperatingPoint = "FlatBEff_60")
#ToolSvc += BTagTool_FlatBEff_60
#
#BTagTool_FlatBEff_70 = CfgMgr.BTaggingSelectionTool('FlatBEff_70',
#						    MaxEta         = MaxEta,
#						    MinPt          = MinPt,
#						    JetAuthor      = JetAuthor,
#						    TaggerName     = TaggerName,
#						    FlvTagCutDefinitionsFileName = FlvTagCutDefinitionsFileName,
#						    OperatingPoint = "FlatBEff_70")
#ToolSvc += BTagTool_FlatBEff_70
#
#BTagTool_FlatBEff_77 = CfgMgr.BTaggingSelectionTool('FlatBEff_77',
#						    MaxEta         = MaxEta,
#						    MinPt          = MinPt,
#						    JetAuthor      = JetAuthor,
#						    TaggerName     = TaggerName,
#						    FlvTagCutDefinitionsFileName = FlvTagCutDefinitionsFileName,
#						    OperatingPoint = "FlatBEff_77")
#ToolSvc += BTagTool_FlatBEff_77

from JetMissingEtTagTools.JetMissingEtTagToolsConf import JetMetTagTool as ConfiguredJetMissingEtTagTool
JetMissingEtTagTool=ConfiguredJetMissingEtTagTool(
	JetCalibrationTool  = JetCalibTool,
	JetVertexTaggerTool = JetVertexTaggerTool,
	JetContainer        = "AntiKt4EMTopoJets",
	EtCut               = 40.0*GeV,
	EtCutSkim           = 20.0*GeV,
	UseEMScale          = False,
	isSimulation        = inputIsSimulation,
	JetCalibContainer      = "AntiKt4TopoJets_TAGcalib",
	JetCalibContainerSkim  = "AntiKt4TopoJets_TAGcalibskim",
	ElectronsContainerSkim = "Electrons_TAG_skim",
	PhotonsContainerSkim   = "Photons_TAG_skim",
	TausContainerSkim      = "TauJets_TAG_skim",
	MuonsContainerSkim     = "Muons_TAG_skim",
	METMaker               = METMakerTool,
	METCoreName             = "MET_Core_AntiKt4EMTopo",
	METMapName            = "METAssoc_AntiKt4EMTopo",
	METContainer        = "MET_Reference_AntiKt4Topo_TAGcalibskim",
	METFinalName        = "FinalClus",
	METJetName          = "RefJet",
	METMuonsName        = "Muons",
	METSoftClusName     = "SoftClus",
	METRefTauName       = "RefTau",
	METRefEleName       = "RefEle",
	METRefGammaName     = "RefGamma",
	METPVSoftTrkName    = "PVSoftTrk",
	METFinalTrkName     = "FinalTrk",
	FixedCutBEff60      = BTagTool_FixedCutBEff_60,
	FixedCutBEff70      = BTagTool_FixedCutBEff_70,
	FixedCutBEff85      = BTagTool_FixedCutBEff_85,
	DoJVT               = jetFlags.useVertices(),
#	FlatBEff60          = BTagTool_FlatBEff_60,
#	FlatBEff70          = BTagTool_FlatBEff_70,
#	FlatBEff77          = BTagTool_FlatBEff_77
	#OutputLevel = 2,
	)

ToolSvc += JetMissingEtTagTool
