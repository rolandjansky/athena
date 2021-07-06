from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax = -1 
athenaCommonFlags.SkipEvents = 0
from AthenaCommon.GlobalFlags import globalflags

import AthenaCommon.AtlasUnixStandardJob

# use auditors
from AthenaCommon.AppMgr import ServiceMgr

from GaudiCommonSvc.GaudiCommonSvcConf import AuditorSvc

ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
#ChronoStatSvc = Service ( "ChronoStatSvc")
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
#MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )
theApp.AuditAlgorithms=True

from RecExConfig.RecFlags import rec
rec.doFileMetaData.set_Value_and_Lock(True)
rec.doAOD.set_Value_and_Lock(False)
rec.readESD.set_Value_and_Lock(False)
rec.readAOD.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteTAGCOM.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doCBNT.set_Value_and_Lock(False)
rec.doHist.set_Value_and_Lock(False)
rec.doDumpTES.set_Value_and_Lock(False)
rec.doPerfMon.set_Value_and_Lock(False)
rec.doEdmMonitor.set_Value_and_Lock(False)
rec.doNameAuditor.set_Value_and_Lock(False)
rec.doJiveXML.set_Value_and_Lock(False)
rec.doEgamma.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)
#rec.doMuonCombined.set_Value_and_Lock(False)
#rec.doTile.set_Value_and_Lock(False)
#rec.doMuon.set_Value_and_Lock(False)
rec.doTrigger.set_Value_and_Lock(False)
rec.doAODCaloCells.set_Value_and_Lock(False)
#rec.doCalo.set_Value_and_Lock(False)
#rec.doLArg.set_Value_and_Lock(False)
rec.doLucid.set_Value_and_Lock(False)
rec.doJetMissingETTag.set_Value_and_Lock(False)
rec.doAODall.set_Value_and_Lock(False)
rec.doZdc.set_Value_and_Lock(False)


from AthenaCommon.JobProperties import jobproperties
#jobproperties.Global.DetDescrVersion = DetDescrVersion

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion, GeoModelInit
GeoModelSvc = ServiceMgr.GeoModelSvc
GeoModelSvc.IgnoreTagDifference = True
DetDescrVersion = "ATLAS-GEO-20-00-01"
#jobproperties.global.DetDescrVersion = DetDescrVersion
DetDescrVersion = globalflags.DetDescrVersion()
GeoModelSvc.AtlasVersion = DetDescrVersion


from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()


import AthenaPoolCnvSvc.ReadAthenaPool

ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/testbeam/muontbh8/scratch03/Monica2/data12_8TeV.00205113.physics_Muons.merge.AOD.f449_m1163/data12_8TeV.00205113.physics_Muons.merge.AOD.f449_m1163._lb0590._0001.1"]
#ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/testbeam/muontbh8/scratch03/Monica2/data09_cos.00136379.physics_IDCosmic.merge.AOD.f160_m213/data09_cos.00136379.physics_IDCosmic.merge.AOD.f160_m213._0001.1"]




#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
from IOVDbSvc.CondDB import conddb
IOVDbSvc.OutputLevel = 3
IOVDbSvc.DBInstance="COOLOFL_DCS"
include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")
#--------------------------------------------------------------
# Load IHistSvc
#--------------------------------------------------------------
from GaudiSvc.GaudiSvcConf import THistSvc

ServiceMgr += THistSvc()


ServiceMgr.THistSvc.Output  += ["file1 DATAFILE='ConditionsSummaryMeta.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.OutputLevel = DEBUG



##-----------------------------------------------------------------
## MDT
#-----------------------------------------------------------------
# Dead Chambers

from MuonCondSvc.MuonCondSvcConf import MDT_DCSConditionsSvc
ServiceMgr +=MDT_DCSConditionsSvc()

from MuonCondTool.MuonCondToolConf import MDT_DCSConditionsTool
MDT_DCSConditionsTool = MDT_DCSConditionsTool("MDT_DCSConditionsTool")
MDT_DCSConditionsTool.OutputLevel = FATAL 
#MessageSvc.OutputLevel = DEBUG

MDT_DCSConditionsTool.DropChamberFolder = "/MDT/DCS/DROPPEDCH"
MDT_DCSConditionsTool.HVFolder = "/MDT/DCS/PSHVMLSTATE"
MDT_DCSConditionsTool.LVFolder = "/MDT/DCS/PSLVCHSTATE"
MDT_DCSConditionsTool.JTAGFolder = "/MDT/DCS/JTAGCHSTATE"

ToolSvc += MDT_DCSConditionsTool

dbConn="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200"
folder="/MDT/DCS/DROPPEDCH"


ServiceMgr.IOVDbSvc.dbConnection=dbConn
ServiceMgr.IOVDbSvc.Folders+=[folder+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]
ServiceMgr.IOVDbSvc.Folders+=["/MDT/DCS/PSHVMLSTATE"+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]
ServiceMgr.IOVDbSvc.Folders+=["/MDT/DCS/PSLVCHSTATE"+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]
ServiceMgr.IOVDbSvc.Folders+=["/MDT/DCS/JTAGCHSTATE"+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]



from MuonCondSvc.MuonCondSvcConf import MDTCondSummarySvc
ServiceMgr +=MDTCondSummarySvc()


MDTCondSummary=ServiceMgr.MDTCondSummarySvc
MDTCondSummary.ConditionsServices=["MDT_DCSConditionsSvc","MDT_DeadTubeConditionsSvc"]

## #----------------------------------------------------------------------------------------------------
## ### RPC Staff
## '''
## from MuonCondSvc.MuonCondSvcConf import RPC_STATUSConditionsSvc
## ServiceMgr +=RPC_STATUSConditionsSvc()

## from MuonCondTool.MuonCondToolConf import RpcDetectorStatusDbTool
## RpcDetectorStatusDbTool = RpcDetectorStatusDbTool("RpcDetectorStatusDbTool")
## RpcDetectorStatusDbTool.OutputLevel = DEBUG 
## MessageSvc.OutputLevel = DEBUG
## RpcDetectorStatusDbTool.RpcDqFolder = "/OFFLINE/FINAL"
## ToolSvc += RpcDetectorStatusDbTool

## dbConn_Rpc="oracle://intr;schema=ATLAS_COOL_RPCDQ;dbname=RPC_DQA"
## folder_Rpc="/OFFLINE/FINAL"

## ServiceMgr.IOVDbSvc.dbConnection=dbConn_Rpc
## ServiceMgr.IOVDbSvc.Folders+=[folder_Rpc+" <tag>RecoCondDB</tag> <dbConnection>"+dbConn_Rpc+"</dbConnection>"]

## from MuonCondSvc.MuonCondSvcConf import RPCCondSummarySvc
## ServiceMgr +=RPCCondSummarySvc()


## RPCCondSummary=ServiceMgr.RPCCondSummarySvc
## RPCCondSummary.ConditionsServices=["RPC_STATUSConditionsSvc"]

## ##-----------------------------------------------------------------------------
## ### TGC Staff

## from MuonCondSvc.MuonCondSvcConf import TGC_STATUSConditionsSvc
## ServiceMgr +=TGC_STATUSConditionsSvc()

## from MuonCondTool.MuonCondToolConf import TGC_STATUSConditionsTool
## TGC_STATUSConditionsTool = TGC_STATUSConditionsTool("TGC_STATUSConditionsTool")
## TGC_STATUSConditionsTool.OutputLevel = DEBUG 
## MessageSvc.OutputLevel = DEBUG

## TGC_STATUSConditionsTool.TgcDqFolder = "/TGC/1/DetectorStatus"

## ToolSvc += TGC_STATUSConditionsTool

## folderTGC="/TGC/1/DetectorStatus"
## dbConnTGC="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TGC;dbname=COMP200"#;user=ATLAS_COOL_READER_U"

## ServiceMgr.IOVDbSvc.dbConnection=dbConnTGC
## #ServiceMgr.IOVDbSvc.DBInstance="COOLONL_TGC"
## ServiceMgr.IOVDbSvc.Folders+=[folderTGC+" <tag>Tgc1DetectorStatus-V1-0</tag> <dbConnection>"+dbConnTGC+"</dbConnection>"]

## from MuonCondSvc.MuonCondSvcConf import TGCCondSummarySvc
## ServiceMgr +=TGCCondSummarySvc()


## TGCCondSummary=ServiceMgr.TGCCondSummarySvc
## TGCCondSummary.ConditionsServices=["TGC_STATUSConditionsSvc"]



## '''

##-----------------------------------------------------------------
## CSC
#-----------------------------------------------------------------
# Dead Chambers

from MuonCondSvc.MuonCondSvcConf import CSC_DCSConditionsSvc
ServiceMgr +=CSC_DCSConditionsSvc()

from MuonCondTool.MuonCondToolConf import CSC_DCSConditionsTool
CSC_DCSConditionsTool = CSC_DCSConditionsTool("CSC_DCSConditionsTool")
CSC_DCSConditionsTool.OutputLevel = VERBOSE 
#MessageSvc.OutputLevel = DEBUG


CSC_DCSConditionsTool.HVFolder = "/CSC/DCS/LAYERSTATE"
CSC_DCSConditionsTool.ChamberFolder = "/CSC/DCS/ENABLEDCHAMBERS"

ToolSvc += CSC_DCSConditionsTool

dbConn="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200"
folder="/CSC/DCS/LAYERSTATE"


ServiceMgr.IOVDbSvc.dbConnection=dbConn

ServiceMgr.IOVDbSvc.Folders+=["/CSC/DCS/LAYERSTATE"+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]
ServiceMgr.IOVDbSvc.Folders+=["/CSC/DCS/ENABLEDCHAMBERS"+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]

from MuonCondSvc.MuonCondSvcConf import CSCCondSummarySvc
ServiceMgr +=CSCCondSummarySvc()


CSCCondSummary=ServiceMgr.CSCCondSummarySvc
CSCCondSummary.ConditionsServices=["CSC_DCSConditionsSvc"]
#__________________________________________________________________





from MuonCondTest.MuonCondTestConf import MuonConditionsHistoSummary
job+= MuonConditionsHistoSummary()


#import AthenaCommon.AtlasUnixGeneratorJob

#ServiceMgr.EventSelector.RunNumber  = 138460 #1204110576 seconds epoch
#import time, calendar
#time in seconds , now
#ServiceMgr.EventSelector.InitialTimeStamp  = calendar.timegm(time.gmtime())
#ServiceMgr.EventSelector.InitialTimeStamp  =  594682#found valid in db browser?
theApp.EvtMax                   =  1 

ServiceMgr.MessageSvc.Format           = "% F%40W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = FATAL
