include.block ("ElectronPhotonTagTools/ElectronTagTool_jobOptions.py")

########### Electron selection options ################
from ElectronPhotonSelectorTools.ConfiguredAsgElectronLikelihoodTools import ConfiguredAsgElectronLikelihoodTool
from ROOT import LikeEnum
LooseLHSelector = ConfiguredAsgElectronLikelihoodTool("LooseLHSelector", LikeEnum.Loose, ConfigFile= "ElectronPhotonSelectorTools/offline/dc14b_20150121/ElectronLikelihoodLooseOfflineConfig2015.conf")
ToolSvc+=LooseLHSelector
MediumLHSelector = ConfiguredAsgElectronLikelihoodTool("MediumLHSelector", LikeEnum.Medium, ConfigFile= "ElectronPhotonSelectorTools/offline/dc14b_20150121/ElectronLikelihoodMediumOfflineConfig2015.conf")
ToolSvc+=MediumLHSelector
TightLHSelector = ConfiguredAsgElectronLikelihoodTool("TightLHSelector", LikeEnum.Tight, ConfigFile="ElectronPhotonSelectorTools/offline/dc14b_20150121/ElectronLikelihoodTightOfflineConfig2015.conf")
ToolSvc+=TightLHSelector

from ElectronPhotonTagTools.ElectronPhotonTagToolsConf import \
     ElectronTagTool as ConfiguredElectronTagTool
ElectronTagTool = ConfiguredElectronTagTool(
     Container         = ["Electrons"],
     EtCut             = 7.0*GeV,
     CaloIsoCutValues  = [ 0.15, 0.30, 3.0*GeV, 5.0*GeV , 0.007, 0.022],
     TrackIsoCutValues = [ 0.15, 0.30, 3.0*GeV, 5.0*GeV , 0.007, 0.022],
     TightLHSelector   = TightLHSelector,
     MediumLHSelector  = MediumLHSelector,
     LooseLHSelector   = LooseLHSelector  )
ToolSvc += ElectronTagTool
