include.block ("ElectronPhotonTagTools/ElectronTagTool_jobOptions.py")

########### Electron LH selection options ################
from ElectronPhotonSelectorTools.ConfiguredAsgElectronLikelihoodTools import ConfiguredAsgElectronLikelihoodTool
from ROOT import LikeEnum
VeryLooseLHSelector = ConfiguredAsgElectronLikelihoodTool("VeryLooseLHSelector", LikeEnum.VeryLoose)
ToolSvc += VeryLooseLHSelector
LooseLHSelector     = ConfiguredAsgElectronLikelihoodTool("LooseLHSelector",     LikeEnum.Loose)
ToolSvc += LooseLHSelector
MediumLHSelector    = ConfiguredAsgElectronLikelihoodTool("MediumLHSelector",    LikeEnum.Medium)
ToolSvc += MediumLHSelector
TightLHSelector     = ConfiguredAsgElectronLikelihoodTool("TightLHSelector",     LikeEnum.Tight)
ToolSvc += TightLHSelector

########### Electron cut based selection options ################
from ElectronPhotonSelectorTools.ConfiguredAsgElectronIsEMSelectors import ConfiguredAsgElectronIsEMSelector
from ROOT import egammaPID

#Loose
ElectronIsEMSelectorLoose = ConfiguredAsgElectronIsEMSelector("ElectronIsEMSelectorLoose",   egammaPID.ElectronIDLoosePP)
ToolSvc += ElectronIsEMSelectorLoose

#Medium
ElectronIsEMSelectorMedium = ConfiguredAsgElectronIsEMSelector("ElectronIsEMSelectorMedium", egammaPID.ElectronIDMediumPP)
ToolSvc += ElectronIsEMSelectorMedium

#Tight
ElectronIsEMSelectorTight = ConfiguredAsgElectronIsEMSelector("ElectronIsEMSelectorTight",   egammaPID.ElectronIDTightPP)
ToolSvc += ElectronIsEMSelectorTight

########### Electron Isolation options ################

isSimulation = False
if inputFileSummary.has_key("evt_type"):
	eventTypeList = inputFileSummary.get("evt_type")
	if eventTypeList.__contains__("IS_SIMULATION") :
		print "ElectronTagTool: Detected that the input file is a simulated dataset"
		isSimulation = True
	else:
		print "ElectronTagTool: Detected that the input file is real data"
		pass

IsolationCorrectionTool          = CfgMgr.CP__IsolationCorrectionTool( "ElectronIsolationCorrectionTool" )
IsolationCorrectionTool.IsMC     = isSimulation
ToolSvc += IsolationCorrectionTool

LooseTrackOnlyIsoTool            = CfgMgr.CP__IsolationSelectionTool( "ElectronLooseTrackOnlyIsolationSelectionTool" )
LooseTrackOnlyIsoTool.ElectronWP = "LooseTrackOnly"
ToolSvc += LooseTrackOnlyIsoTool
LooseIsoTool                     = CfgMgr.CP__IsolationSelectionTool( "ElectronLooseIsolationSelectionTool" )
LooseIsoTool.ElectronWP          = "Loose"
ToolSvc += LooseIsoTool
TightIsoTool                     = CfgMgr.CP__IsolationSelectionTool( "ElectronTightIsolationSelectionTool" )
TightIsoTool.ElectronWP          = "Tight"
ToolSvc += TightIsoTool
GradientIsoTool                  = CfgMgr.CP__IsolationSelectionTool( "ElectronGradientIsolationSelectionTool" )
GradientIsoTool.ElectronWP       = "Gradient"
ToolSvc += GradientIsoTool
GradientLooseIsoTool             = CfgMgr.CP__IsolationSelectionTool( "ElectronGradientLooseIsolationSelectionTool" )
GradientLooseIsoTool.ElectronWP  = "GradientLoose"
ToolSvc += GradientLooseIsoTool
FixedCutTightTrackOnlyIsoTool            = CfgMgr.CP__IsolationSelectionTool( "ElectronFixedCutTightTrackOnlyIsolationSelectionTool" )
FixedCutTightTrackOnlyIsoTool.ElectronWP = "FixedCutTightTrackOnly"
ToolSvc += FixedCutTightTrackOnlyIsoTool
FixedCutLooseIsoTool                     = CfgMgr.CP__IsolationSelectionTool( "ElectronFixedCutLooseIsolationSelectionTool" )
FixedCutLooseIsoTool.ElectronWP          = "FixedCutLoose"
ToolSvc += FixedCutLooseIsoTool
FixedCutTightIsoTool                     = CfgMgr.CP__IsolationSelectionTool( "ElectronFixedCutTightIsolationSelectionTool" )
FixedCutTightIsoTool.ElectronWP          = "FixedCutTight"
ToolSvc += FixedCutTightIsoTool

from ElectronPhotonTagTools.ElectronPhotonTagToolsConf import \
     ElectronTagTool as ConfiguredElectronTagTool
ElectronTagTool = ConfiguredElectronTagTool(
     Container                       = ["Electrons"],
     ElectronMETContainerName        = "Electrons_TAG_skim",
     EtCut                           = 7.0*GeV,
     EtconeIsoCutValues              = [ 0.15, 0.30, 3.0*GeV, 5.0*GeV], 
     PtconeIsoCutValues              = [ 0.15, 0.30, 3.0*GeV, 5.0*GeV], 
     TightLHSelector                 = TightLHSelector,
     MediumLHSelector                = MediumLHSelector,
     LooseLHSelector                 = LooseLHSelector,
     VeryLooseLHSelector             = VeryLooseLHSelector,
     ElectronIsEMSelectorLoose       = ElectronIsEMSelectorLoose,
     ElectronIsEMSelectorMedium      = ElectronIsEMSelectorMedium,
     ElectronIsEMSelectorTight       = ElectronIsEMSelectorTight,
     IsolationCorrectionTool         = IsolationCorrectionTool,
     LooseTrackOnlyIsolation         = LooseTrackOnlyIsoTool,
     LooseIsolation                  = LooseIsoTool,
     TightIsolation                  = TightIsoTool,
     GradientIsolation               = GradientIsoTool,
     GradientLooseIsolation          = GradientLooseIsoTool,
     FixedCutTightTrackOnlyIsolation = FixedCutTightTrackOnlyIsoTool,
     FixedCutLooseIsolation          = FixedCutLooseIsoTool,
     FixedCutTightIsolation          = FixedCutTightIsoTool      
     )

# ElectronTagTool.EgammaCalibrationAndSmearingTool.randomRunNumber = 308047
# ElectronTagTool.EgammaCalibrationAndSmearingTool.ESModel =  "es2016data_mc15c"

# ToolSvc += ElectronTagTool
