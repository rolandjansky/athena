#include.block("PixelConditionsServices/PixelCalibSvc_jobOptions.py")

#Read from Oracle DB
#from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
#DBReplicaSvc = DBReplicaSvc(UseCOOLSQLite=False)
#svcMgr += DBReplicaSvc

#setup folder and tag
from IOVDbSvc.CondDB import conddb
conddb.addFolder("PIXEL_OFL","/PIXEL/PixReco")

#Configure PixelRecoDbTool
from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTool
ToolSvc += PixelRecoDbTool()
ToolSvc.PixelRecoDbTool.InputSource = 2




