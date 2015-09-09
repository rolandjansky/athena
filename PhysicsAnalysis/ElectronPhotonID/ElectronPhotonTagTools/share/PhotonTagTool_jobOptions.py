include.block ("ElectronPhotonTagTools/PhotonTagTool_jobOptions.py")

###############SHOWER SHAPE FUDGING IN MC######################

DF_ElectronPhotonShowerShapeFudgeTool = None

from PyUtils import AthFile
af = AthFile.fopen(svcMgr.EventSelector.InputCollections[0]) # opens the first file from the InputCollections list
af.fileinfos # this is a dict of dicts, take a look at what's available! Below are some examples:

isMC = 'IS_SIMULATION' in af.fileinfos['evt_type']
beam_energy    = af.fileinfos['beam_energy']
conditions_tag = af.fileinfos['conditions_tag'] #useful for figuring out which mc production this is
isFullsim = False
if isMC: 
    simulationFlavour = af.fileinfos['metadata']['/Simulation/Parameters']['SimulationFlavour']
    isFullsim = simulationFlavour in ('default', 'MC12G4', 'FullG4') # is this a full sim dataset?
    
print "EGammaCommon: isMC = ", isMC
if isMC: 
    print "EGammaCommon: isFullsim = ", isFullsim

if isFullsim:
#from AthenaCommon.GlobalFlags import globalflags
#print "globalflags.DataSource(): ", globalflags.DataSource()
#if globalflags.DataSource()=='geant4':
    from ElectronPhotonShowerShapeFudgeTool.ElectronPhotonShowerShapeFudgeToolConf import ElectronPhotonShowerShapeFudgeTool
    DF_ElectronPhotonShowerShapeFudgeTool = ElectronPhotonShowerShapeFudgeTool("PhotonShowerShapeFudgeTool")
    DF_ElectronPhotonShowerShapeFudgeTool = ElectronPhotonShowerShapeFudgeTool(Preselection=16)
    ToolSvc += DF_ElectronPhotonShowerShapeFudgeTool
    print DF_ElectronPhotonShowerShapeFudgeTool

############ Photon selecton options #################
from IsolationSelection.IsolationSelectionConf import CP__IsolationSelectionTool
cone20IsoTool = CfgMgr.CP__IsolationSelectionTool( "PhotonCone20IsolationSelectionTool" )
cone20IsoTool.PhotonWP = "Cone20"
ToolSvc += cone20IsoTool
cone40IsoTool = CfgMgr.CP__IsolationSelectionTool( "PhotonCone40IsolationSelectionTool" )
cone40IsoTool.PhotonWP = "Cone40"
ToolSvc += cone40IsoTool
cone40CaloOnlyIsoTool = CfgMgr.CP__IsolationSelectionTool( "PhotonCone40CaloOnlyIsolationSelectionTool" )
cone40CaloOnlyIsoTool.PhotonWP = "Cone40CaloOnly"
ToolSvc += cone40CaloOnlyIsoTool

########### Photon cut based selection options ################
from ROOT import egammaPID

# Loose
from ElectronPhotonSelectorTools.ConfiguredAsgPhotonIsEMSelectors import ConfiguredAsgPhotonIsEMSelector
PhotonIsEMSelectorLoose = ConfiguredAsgPhotonIsEMSelector("PhotonIsEMSelectorLoose", egammaPID.PhotonIDLoose)
ToolSvc += PhotonIsEMSelectorLoose

# Tight
PhotonIsEMSelectorTight = ConfiguredAsgPhotonIsEMSelector("PhotonIsEMSelectorTight", egammaPID.PhotonIDTight)
ToolSvc += PhotonIsEMSelectorTight


from ElectronPhotonTagTools.ElectronPhotonTagToolsConf import PhotonTagTool as ConfiguredPhotonTagTool
PhotonTagTool = ConfiguredPhotonTagTool(
    Container            = "Photons",
    EtCut                = 10.0*GeV,
    EtconeIsoCutValues   = [ 0.15, 0.30, 3.0*GeV, 5.0*GeV ],
    PtconeIsoCutValues   = [ 0.15, 0.30, 3.0*GeV, 5.0*GeV ],
    isFullsim            = isFullsim,
    PhotonShowerFudgeTool= DF_ElectronPhotonShowerShapeFudgeTool,
    LooseSelector        = PhotonIsEMSelectorLoose,
    TightSelector        = PhotonIsEMSelectorTight,
    cone20IsoTool        = cone20IsoTool,
    cone40IsoTool        = cone40IsoTool,
    cone40CaloOnlyIsoTool= cone40CaloOnlyIsoTool
    )

PhotonTagTool.EgammaCalibrationAndSmearingTool.ESModel =  "es2015PRE"

ToolSvc += PhotonTagTool
