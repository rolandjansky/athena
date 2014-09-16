include.block("LArRecUtils/LArADC2MeVTool_jobOptions.py")

print " WARNING obsolete jobOption. Please use python class to configure LArADC2MeVTool"

from AthenaCommon.AppMgr import ToolSvc
from LArRecUtils.LArRecUtilsConf import LArADC2MeVTool
theLArADC2MeVTool = LArADC2MeVTool("LArADC2MeVTool")

# Set data type (needed to decide wether to retrieve LArNoise or LArPedestal for the noise)
from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='data':
    theLArADC2MeVTool.MCSym=False
else:
    theLArADC2MeVTool.MCSym=True

theLArADC2MeVTool.UseMphysOverMcal = False
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
for lines in svcMgr.IOVDbSvc.Folders :
    if (lines.__contains__("MphysOverMcal")) :
        theLArADC2MeVTool.UseMphysOverMcal = True

ToolSvc+=theLArADC2MeVTool
