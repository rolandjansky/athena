include.block ("ElectronPhotonTagTools/PhotonTagTool_jobOptions.py")

############ Photon selecton options #################

from ElectronPhotonTagTools.ElectronPhotonTagToolsConf import \
  PhotonTagTool as ConfiguredPhotonTagTool
PhotonTagTool = ConfiguredPhotonTagTool(
     Container         = "PhotonCollection",
     EtCut             = 10.0*GeV,
     CaloIsoCutValues  = [ 0.15, 0.30, 3.0*GeV, 5.0*GeV ],
     TrackIsoCutValues = [ 0.15, 0.30, 3.0*GeV, 5.0*GeV ] )
ToolSvc += PhotonTagTool
