
include.block('TrigT2CaloCommon/TrigDataAccessConfigured.py')
from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.GlobalFlags import globalflags
from RecExConfig.RecFlags import rec
from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigDataAccess
from AthenaCommon.AppMgr import ToolSvc
TDA=TrigDataAccess()
ToolSvc+=TDA
ToolSvc.TrigDataAccess.loadFullCollections = ( TriggerFlags.doEF() or TriggerFlags.doHLT() )
ToolSvc.TrigDataAccess.loadAllSamplings    = ( TriggerFlags.doLVL2() or TriggerFlags.doHLT() )
AOC=True
ToolSvc.TrigDataAccess.ApplyOffsetCorrection=AOC # Later we should find a better way to play with this
#AOD=ToolSvc.TrigDataAccess.ApplyOffsetCorrection # why this is not working?!

if ( globalflags.DatabaseInstance == "COMP200" ) :
  ToolSvc.TrigDataAccess.ApplyOffsetCorrection=False
  print "TrigDataAccessConfigured.py : Not possible to run BCID offset"
else:
  if ( AOC ) :
      print 'TrigDataAccessConfigured.py: will configure the HLT to apply offset correction'
      if globalflags.DataSource()=='data':
            if athenaCommonFlags.isOnline:
                  conddb.addFolder("LAR_ONL","/LAR/ElecCalibFlat/OFC")
                  from LArRecUtils.LArRecUtilsConf import LArFlatConditionSvc
                  theLArCondSvc=LArFlatConditionSvc()
                  svcMgr+=theLArCondSvc
                  svcMgr.ProxyProviderSvc.ProviderNames += [ "LArFlatConditionSvc" ]
                  theLArCondSvc.OFCInput="/LAR/ElecCalibFlat/OFC"
      
      from CaloTools.CaloLumiBCIDToolDefault import CaloLumiBCIDToolDefault
      theCaloLumiBCIDTool = CaloLumiBCIDToolDefault()
      ToolSvc+=theCaloLumiBCIDTool
      ToolSvc.TrigDataAccess.CaloLumiBCIDTool = theCaloLumiBCIDTool

transientBS = (rec.readRDO() and not globalflags.InputFormat()=='bytestream')
if ( transientBS or TriggerFlags.writeBS() ):
  if ( not hasattr(svcMgr.ToolSvc,'LArRawDataContByteStreamTool') ):
    from LArByteStream.LArByteStreamConfig import LArRawDataContByteStreamToolConfig
    svcMgr.ToolSvc+=LArRawDataContByteStreamToolConfig()
  svcMgr.ToolSvc.LArRawDataContByteStreamTool.DSPRunMode=4
  svcMgr.ToolSvc.LArRawDataContByteStreamTool.RodBlockVersion=10

