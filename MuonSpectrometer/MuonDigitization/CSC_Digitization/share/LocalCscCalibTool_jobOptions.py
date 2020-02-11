include.block( "CscCalibTools/CscCalibTool_jobOptions.py" )

from AthenaCommon.AppMgr import ToolSvc

from CscCalibTools.CscCalibToolsConf import \
     CscCalibTool as ConfiguredCscCalibTool 
CscCalibTool = ConfiguredCscCalibTool (
               Slope=0.19,
               Noise=3.5,
               Pedestal=2048.0,
               ReadFromDatabase=True,
               SlopeFromDatabase=False,
               integrationNumber=12.0,
               integrationNumber2=11.66,
               samplingTime=50.0,
               signalWidth=14.40922,
               timeOffset=71.825,
               IsOnline = athenaCommonFlags.isOnline()
               )
#               timeOffset=46.825) + 25 SimHIT digit time

ToolSvc += CscCalibTool

