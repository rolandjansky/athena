# jobOptions file for LAr Cell Reconstruction from LArRawChannel 
# with full corrections. 

# SUBCALO enum LAREM LARHEC LARFCAL TILE NSUBCALO NOT_VALID
from CaloIdentifier import SUBCALO

# Properties of LArCellRec:
theApp.Dlls += [ "FastCaloSim" ]

ToolSvc = Service( "ToolSvc" )

CaloCellMakerFS=Algorithm( "CaloCellMakerFS" )

if not 'doCaloNoise'  in dir():
    doCaloNoise=True

#add cell maker tools to top cell maker alg (defined elsewhere)
CaloCellMakerFS.CaloCellMakerToolNames += ["EmptyCellBuilderTool" ]
CaloCellMakerFS.CaloCellMakerToolNames += ["FastShowerCellBuilderTool" ]
if doCaloNoise:
    CaloCellMakerFS.CaloCellMakerToolNames += ["AddNoiseCellBuilderTool" ]
#ToolSvc.FastShowerCellBuilderTool.OutputLevel=VERBOSE
#ToolSvc.FastShowerCellBuilderTool.OutputLevel=DEBUG
#ToolSvc.FastShowerCellBuilderTool.OutputLevel=INFO



# protect the from straight-line
if not 'doStraightToCalo' in dir():
    doSaveStraightToCalo = False
else:
    doSaveStraightToCalo = doStraightToCalo

# RUngeKutta for electrons
doStraightToCalo = False

#CaloCellMakerFS.FastShowerCellBuilderTool.OutputLevel=VERBOSE
#ToolSvc.FastShowerCellBuilderTool.OutputLevel=VERBOSE

include ('CaloDetDescr/CaloDepth_joboptions.py' )
include ('TrkExTools/ExtrapolationConfiguration.py')

TTC_ExtrapolatorInstance = 'FSElectronTTCExtrapolator'
include( "TrackToCalo/ExtrapolToCaloTool_joboptions.py" )
# JobOptions to extrapolate a track to Calorimeters
# configure private ExtrapolTrackToCaloTool tool
ToolSvc.FastShowerCellBuilderTool.FSElectronExtrapolTrackToCaloTool.ExtrapolatorName = TTC_Extrapolator.name()
ToolSvc.FastShowerCellBuilderTool.FSElectronExtrapolTrackToCaloTool.ExtrapolatorInstanceName = TTC_Extrapolator.instance()
#specify here the extrapolation depth
ToolSvc.FastShowerCellBuilderTool.FSElectronExtrapolTrackToCaloTool.CaloDepthTool.DepthChoice = "middle"
ToolSvc.FastShowerCellBuilderTool.FSElectronExtrapolTrackToCaloTool.OutputLevel=INFO

TTC_ExtrapolatorInstance = 'FSElectronTTCExtrapolatorEntrance'
include( "TrackToCalo/ExtrapolToCaloTool_joboptions.py" )
ToolSvc.FastShowerCellBuilderTool.FSElectronExtrapolTrackToCaloToolEntrance.ExtrapolatorName = TTC_Extrapolator.name()
ToolSvc.FastShowerCellBuilderTool.FSElectronExtrapolTrackToCaloToolEntrance.ExtrapolatorInstanceName = TTC_Extrapolator.instance()
ToolSvc.FastShowerCellBuilderTool.FSElectronExtrapolTrackToCaloToolEntrance.CaloDepthTool.DepthChoice = "entrance"
ToolSvc.FastShowerCellBuilderTool.FSElectronExtrapolTrackToCaloToolEntrance.OutputLevel=INFO

TTC_ExtrapolatorInstance = 'FSTTCExtrapolator'
include( "TrackToCalo/ExtrapolToCaloTool_joboptions.py" )
ToolSvc.FastShowerCellBuilderTool.ExtrapolatorName = TTC_Extrapolator.name()
ToolSvc.FastShowerCellBuilderTool.ExtrapolatorInstanceName = TTC_Extrapolator.instance()

#Propagator for photons
doStraightToCalo = True

TTC_ExtrapolatorInstance = 'FSPhotonTTCExtrapolator'
include( "TrackToCalo/ExtrapolToCaloTool_joboptions.py" )
ToolSvc.FastShowerCellBuilderTool.FSPhotonExtrapolTrackToCaloTool.ExtrapolatorName = TTC_Extrapolator.name()
ToolSvc.FastShowerCellBuilderTool.FSPhotonExtrapolTrackToCaloTool.ExtrapolatorInstanceName = TTC_Extrapolator.instance()
#specify here the extrapolation depth
ToolSvc.FastShowerCellBuilderTool.FSPhotonExtrapolTrackToCaloTool.CaloDepthTool.DepthChoice = "middle"
ToolSvc.FastShowerCellBuilderTool.FSPhotonExtrapolTrackToCaloTool.OutputLevel=INFO

TTC_ExtrapolatorInstance = 'FSPhotonTTCExtrapolatorEntrance'
include( "TrackToCalo/ExtrapolToCaloTool_joboptions.py" )
ToolSvc.FastShowerCellBuilderTool.FSPhotonExtrapolTrackToCaloToolEntrance.ExtrapolatorName = TTC_Extrapolator.name()
ToolSvc.FastShowerCellBuilderTool.FSPhotonExtrapolTrackToCaloToolEntrance.ExtrapolatorInstanceName = TTC_Extrapolator.instance()
ToolSvc.FastShowerCellBuilderTool.FSPhotonExtrapolTrackToCaloToolEntrance.CaloDepthTool.DepthChoice = "entrance"
ToolSvc.FastShowerCellBuilderTool.FSPhotonExtrapolTrackToCaloToolEntrance.OutputLevel=INFO

# reset the StraightToCalo after use
doStraightToCalo = doSaveStraightToCalo

#CaloCellMakerFS.FastShowerCellBuilderTool.OutputLevel=DEBUG
