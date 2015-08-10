include.block ("JetMissingEtTagTools/JetTagTool_jobOptions.py")

########### Jet tag options ################
from RecExConfig.RecFlags import rec

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

from JetMissingEtTagTools.JetMissingEtTagToolsConf import JetMetTagTool as ConfiguredJetMissingEtTagTool
from AthenaCommon.BeamFlags import jobproperties

if rec.doHeavyIon() or  jobproperties.Beam.beamType() == 'cosmics' or jobproperties.Beam.beamType() == 'singlebeam':

	CalibrationSetup="aroj"

	if jobproperties.Beam.beamType() == 'cosmics' or jobproperties.Beam.beamType() == 'singlebeam':
		CalibrationSetup="aj"

	from JetRec.JetRecCalibrationFinder import jrcf
	JetCalibrationTool = jrcf.find("AntiKt", 0.4, "LCTopo", CalibrationSetup, "reco", "Kt4")
	ToolSvc += JetCalibrationTool 

	if rec.doHeavyIon():
		JetMissingEtTagTool=ConfiguredJetMissingEtTagTool(
			JetCalibrationTool  = JetCalibrationTool,
			JetContainer        = "antikt4HIItrEM_TowerJets",
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
			JetCalibContainer       = "AntiKt4TopoJets_TAGcalib",
			JetCalibContainerSkim   = "AntiKt4TopoJets_TAGcalibskim",
			EtCut               = 40.0*GeV,
			EtCutSkim           = 20.0*GeV,
			UseEMScale          = False,
			isSimulation        = inputIsSimulation
			#OutputLevel = 2,
			)
	else:
		JetMissingEtTagTool=ConfiguredJetMissingEtTagTool(
			JetCalibrationTool  = JetCalibrationTool,
			JetContainer        = "AntiKt4LCTopoJets",
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

			JetCalibContainer       = "AntiKt4TopoJets_TAGcalib",
			JetCalibContainerSkim   = "AntiKt4TopoJets_TAGcalibskim",
			EtCut               = 40.0*GeV,
			EtCutSkim           = 20.0*GeV,
			UseEMScale          = False,
			isSimulation        = inputIsSimulation
			
			#OutputLevel = 2,
			)
		pass
	pass
else:

	# ME: use EM jets for 2015 since this is calibrated

	JetCalibTool = CfgMgr.JetCalibrationTool(
		"JetCalib",
		JetCollection = "AntiKt4EMTopo",
		ConfigFile    = "JES_MC15Prerecommendation_April2015.config",
		RhoKey        = "Kt4EMTopoEventShape"
		)
	if inputIsSimulation:
		JetCalibTool.CalibSequence="JetArea_Residual_Origin_EtaJES_GSC"
		JetCalibTool.IsData=False
	else:
		JetCalibTool.CalibSequence="JetArea_Residual_Origin_EtaJES_GSC_Insitu"
		JetCalibTool.IsData=True
	ToolSvc += JetCalibTool

	JetMissingEtTagTool=ConfiguredJetMissingEtTagTool(
		JetCalibrationTool  = JetCalibTool,
		JetContainer        = "AntiKt4EMTopoJets",
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
		JetCalibContainer       = "AntiKt4TopoJets_TAGcalib",
		JetCalibContainerSkim   = "AntiKt4TopoJets_TAGcalibskim",
		EtCut               = 40.0*GeV,
		EtCutSkim           = 20.0*GeV,
		UseEMScale          = False,
		isSimulation        = inputIsSimulation
		#OutputLevel = 2,
		)
	pass

ToolSvc += JetMissingEtTagTool
