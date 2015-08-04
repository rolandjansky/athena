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

CalibrationSetup="aroj"

if jobproperties.Beam.beamType() == 'cosmics' or jobproperties.Beam.beamType() == 'singlebeam':
	CalibrationSetup="aj"

from JetRec.JetRecCalibrationFinder import jrcf
JetCalibrationTool = jrcf.find("AntiKt", 0.4, "LCTopo", CalibrationSetup, "reco", "Kt4")
ToolSvc += JetCalibrationTool 

if rec.doHeavyIon():
	JetMissingEtTagTool=ConfiguredJetMissingEtTagTool(
		JetContainer        = "antikt4HIItrEM_TowerJets",
		UseEMScale          = False,
		EtCut               = 40.0*GeV,
		isSimulation        = inputIsSimulation,
		JetCalibrationTool  = JetCalibrationTool,
		JetCalibContainer   = "AntiKt4TopoJets_TAGcalib",
		METContainer        = "MET_Reference_AntiKt4Topo_TAGcalib",
		METFinalName        = "FinalClus"
		#OutputLevel = 2,
		)
else:
	JetMissingEtTagTool=ConfiguredJetMissingEtTagTool(
		JetContainer        = "AntiKt4EMTopoJets",
		UseEMScale          = False,
		EtCut               = 40.0*GeV,
		isSimulation        = inputIsSimulation,
		JetCalibrationTool  = JetCalibrationTool,
		JetCalibContainer   = "AntiKt4TopoJets_TAGcalib",
		METContainer        = "MET_Reference_AntiKt4Topo_TAGcalib",
		METFinalName        = "FinalClus"
		#OutputLevel = 2,
		)

ToolSvc += JetMissingEtTagTool
