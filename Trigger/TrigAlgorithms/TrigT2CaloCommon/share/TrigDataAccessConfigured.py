
include.block('TrigT2CaloCommon/TrigDataAccessConfigured.py')
from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.GlobalFlags import globalflags
from RecExConfig.RecFlags import rec
from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigDataAccess
from AthenaCommon.AppMgr import ToolSvc
ToolSvc+=TrigDataAccess()
ToolSvc.TrigDataAccess.loadFullCollections = ( TriggerFlags.doEF() or TriggerFlags.doHLT() )
ToolSvc.TrigDataAccess.loadAllSamplings    = ( TriggerFlags.doLVL2() or TriggerFlags.doHLT() )

#from CaloTools.CaloLumiBCIDToolDefault import CaloLumiBCIDToolDefault
#theCaloLumiBCIDTool = CaloLumiBCIDToolDefault()
#ToolSvc+=theCaloLumiBCIDTool
#ToolSvc.TrigDataAccess.CaloLumiBCIDTool = theCaloLumiBCIDTool

transientBS = (rec.readRDO() and not globalflags.InputFormat()=='bytestream')
if ( transientBS or TriggerFlags.writeBS() ):
  if ( not hasattr(svcMgr.ToolSvc,'LArRawDataContByteStreamTool') ):
    from LArByteStream.LArByteStreamConfig import LArRawDataContByteStreamToolConfig
    svcMgr.ToolSvc+=LArRawDataContByteStreamToolConfig()
  svcMgr.ToolSvc.LArRawDataContByteStreamTool.DSPRunMode=4
  svcMgr.ToolSvc.LArRawDataContByteStreamTool.RodBlockVersion=10

