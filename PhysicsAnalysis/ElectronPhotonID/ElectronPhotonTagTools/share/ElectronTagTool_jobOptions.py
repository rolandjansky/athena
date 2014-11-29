include.block ("ElectronPhotonTagTools/ElectronTagTool_jobOptions.py")

########### Electron selection options ################

from ElectronPhotonTagTools.ElectronPhotonTagToolsConf import \
     ElectronTagTool as ConfiguredElectronTagTool
ElectronTagTool = ConfiguredElectronTagTool(
     Container         = ["ElectronCollection","FwdElectrons"],
     EtCut             = 7.0*GeV,
     # CaloIsoCutValues  = [ 0.15, 0.30, 3.0*GeV, 5.0*GeV ],
     # TrackIsoCutValues = [ 0.15, 0.30, 3.0*GeV, 5.0*GeV ] )
     CaloIsoCutValues  = [ ],
     TrackIsoCutValues = [ ] )
ToolSvc += ElectronTagTool
