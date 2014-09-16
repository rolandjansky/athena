if not 'LARAUTOCORRTOTALTOOL_HAS_BEEN_INCLUDED' in dir():
  LARAUTOCORRTOTALTOOL_HAS_BEEN_INCLUDED =1

print " WARNING obsolete jobOption. Please use python class to configure LArAutoCorrTotalTool"

from AthenaCommon.AppMgr import ToolSvc
from LArRecUtils.LArRecUtilsConf import LArAutoCorrTotalTool
LArAutoCorrTotalTool = LArAutoCorrTotalTool("LArAutoCorrTotalTool")
ToolSvc += LArAutoCorrTotalTool

# Set data type (needed to decide wether to retrieve LArNoise or LArPedestal for the noise)
from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='data':
    LArAutoCorrTotalTool.IsMC=False
