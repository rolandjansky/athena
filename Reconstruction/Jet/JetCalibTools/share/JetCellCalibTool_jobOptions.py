# Add this to your JetRec Algorithm to configure this tool

from JetCalibTools.JetCalibToolsConf import *
from JetRec.JetCalibrationConfig import H1Calibration
from CaloClusterCorrection.CaloClusterCorrectionConf import H1WeightToolCSC12Generic
from AthenaCommon.AppMgr import ToolSvc

# Define the Cell Calibration Tool
calibT = JetCellCalibTool('MyCellCalibrator')
tname =  finder + editParm(mainparam) + input
cellcalibtool = H1WeightToolCSC12Generic("H1Weight"+tname)

# now build database info :
(key, folder, tag ) = H1Calibration.getCalibDBParams(finder,mainparam,input)
cellcalibtool.detStoreKey = key
# add folder to database :
H1Calibration.loadCaloFolder(folder,tag)
ToolSvc += cellcalibtool
calibT.CellCalibrator = cellcalibtool

# Now add this calibration tool to your Jet Tool Chain
# For example:
### myJetCalibTool.Calibrator = calibT
# and configure the other properties
# Note that the tool can also be added as a "moment", for example:
### add_jetMoments_to_JetAlg(myJetAlg, [myJetCalibTool])

