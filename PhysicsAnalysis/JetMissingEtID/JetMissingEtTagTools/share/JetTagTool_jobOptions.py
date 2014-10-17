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
        #OutputLevel = 2,
        )

ToolSvc += JetMissingEtTagTool
