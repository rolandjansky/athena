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

CalibrationSetup="aroj"

from AthenaCommon.BeamFlags import jobproperties

if jobproperties.Beam.beamType() == 'cosmics' or jobproperties.Beam.beamType() == 'singlebeam':
	CalibrationSetup="aj"

from JetRec.JetRecCalibrationFinder import jrcf
JetCalibrationTool = jrcf.find("AntiKt", 0.4, "LCTopo", CalibrationSetup, "reco", "auto")
#if not hasattr(ToolSvc, JetCalibrationTool):
ToolSvc += JetCalibrationTool 

from JetMissingEtTagTools.JetMissingEtTagToolsConf import JetMetTagTool as ConfiguredJetMissingEtTagTool
if rec.doHeavyIon():
	JetMissingEtTagTool=ConfiguredJetMissingEtTagTool(
		JetContainer        = "antikt4HIItrEM_TowerJets",
		EtCut               = 10.0*GeV,
		isSimulation        = inputIsSimulation,
		UseEMScale          = False)
else:
        JetMissingEtTagTool=ConfiguredJetMissingEtTagTool(
		JetContainer        = "AntiKt4LCTopoJets",
		UseEMScale          = False,
		EtCut               = 40.0*GeV,
		isSimulation        = inputIsSimulation,
		JetCalibrationTool  = JetCalibrationTool,
		JetCalibContainer   = "AntiKt4LCTopoJets_TAGcalib",
		METContainer        = "MET_Reference_AntiKt4LCTopo_TAGcalib",
        METFinalName        = "FinalClus"
        #OutputLevel = 2,
        )

ToolSvc += JetMissingEtTagTool
