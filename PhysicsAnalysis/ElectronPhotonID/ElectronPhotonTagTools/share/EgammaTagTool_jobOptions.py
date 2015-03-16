include.block ("ElectronPhotonTagTools/EgammaTagTool_jobOptions.py")

########### egamma analysis tag options ################

from ElectronPhotonTagTools.ElectronPhotonTagToolsConf import \
     EgammaTagTool as ConfiguredEgammaTagTool
EgammaTagTool = ConfiguredEgammaTagTool(
     ElectronContainer = "Electrons",
     PhotonContainer   = "Photons",
     ElectronPtCut     = 20.0*GeV,
     PhotonPtCut       = 10.0*GeV,
     MassZLow          = 45.*GeV,
     MassZHigh         = 180.*GeV,
     MissingEtObject   = "MET_RefFinal",
     MissingEtCut      = 25.*GeV, 
     WElectronPtCut    = 15.*GeV )
ToolSvc += EgammaTagTool
